module;
#include <array>
#include <atomic>
#include <source_location>

export module Rhodo.Debug:Memory;

import Rhodo.Memory;
import Rhodo.Logger;
import :Config;

export namespace rhodo::debug::memory {

// Global Counters (Always On)
inline std::atomic<size_t> g_total_allocated{0};
inline std::atomic<size_t> g_total_freed{0};
inline std::atomic<size_t> g_allocation_count{0};
inline std::atomic<size_t> g_deallocation_count{0};

// Category Statistics
struct CategoryStats {
  std::atomic<size_t> allocated{0};
  std::atomic<size_t> freed{0};
  std::atomic<size_t> alloc_count{0};
  std::atomic<size_t> free_count{0};
};

inline std::array<CategoryStats, rhodo::memory::CategoryCount()>
    g_category_stats;

// Frame Delta Tracking
struct FrameStats {
  size_t bytes_allocated{0};
  size_t bytes_freed{0};
  size_t alloc_count{0};
  size_t free_count{0};
};

class FrameTracker {
  static constexpr size_t kMaxFrames = 120;
  std::array<FrameStats, kMaxFrames> m_history_{};
  size_t m_frame_index_{0};

  size_t m_last_total_alloc_{0};
  size_t m_last_total_free_{0};
  size_t m_last_alloc_count_{0};
  size_t m_last_free_count_{0};

 public:
  auto NewFrame() noexcept -> void {
    size_t const kCurrentAlloc =
        g_total_allocated.load(std::memory_order_relaxed);
    size_t const kCurrentFree = g_total_freed.load(std::memory_order_relaxed);
    size_t const kCurrentAllocCount =
        g_allocation_count.load(std::memory_order_relaxed);
    size_t const kCurrentFreeCount =
        g_deallocation_count.load(std::memory_order_relaxed);

    m_history_[m_frame_index_] =
        FrameStats{.bytes_allocated = kCurrentAlloc - m_last_total_alloc_,
                   .bytes_freed = kCurrentFree - m_last_total_free_,
                   .alloc_count = kCurrentAllocCount - m_last_alloc_count_,
                   .free_count = kCurrentFreeCount - m_last_free_count_};

    m_last_total_alloc_ = kCurrentAlloc;
    m_last_total_free_ = kCurrentFree;
    m_last_alloc_count_ = kCurrentAllocCount;
    m_last_free_count_ = kCurrentFreeCount;

    m_frame_index_ = (m_frame_index_ + 1) % kMaxFrames;
  }

  [[nodiscard]] auto GetFrameStats(size_t frames_ago) const noexcept
      -> const FrameStats& {
    size_t const kIdx =
        (m_frame_index_ + kMaxFrames - frames_ago - 1) % kMaxFrames;
    return m_history_[kIdx];
  }

  [[nodiscard]] auto GetAverageChurn(size_t frame_count) const noexcept
      -> size_t {
    frame_count = (frame_count > kMaxFrames) ? kMaxFrames : frame_count;
    size_t total = 0;
    for (size_t i = 0; i < frame_count; ++i) {
      total += GetFrameStats(i).bytes_allocated;
    }
    return (frame_count > 0) ? (total / frame_count) : 0;
  }
};

inline FrameTracker g_frame_tracker;

// Leak Detection Registry (Static Array - No Allocation)
// Uses Tombstone states to handle linear probing correctly
template <size_t Capacity = 4096>
class AllocationRegistry {
  enum class SlotState : uint8_t {
    Empty,     // Never used
    Occupied,  // Currently in use
    Deleted    // Tombstone - was occupied, now freed
  };

  struct Entry {
    void* ptr{nullptr};
    size_t size{0};
    rhodo::memory::MemoryCategory category{
        rhodo::memory::MemoryCategory::Generic};
    std::source_location loc;
    std::atomic<SlotState> state{SlotState::Empty};
  };

  std::array<Entry, Capacity> m_entries_;
  std::atomic<size_t> m_active_count_{0};
  std::atomic<size_t> m_overflow_count_{0};  // Track registry saturation

 public:
  auto Register(void* ptr, size_t size, rhodo::memory::MemoryCategory cat,
                const std::source_location& loc) noexcept -> bool {
    size_t const kHash = std::hash<void*>{}(ptr);
    const size_t kIdx = kHash % Capacity;

    // Linear probing - can overwrite Empty or Deleted slots
    for (size_t i = 0; i < Capacity; ++i) {
      size_t probe_idx = (kIdx + i) % Capacity;
      SlotState state =
          m_entries_[probe_idx].state.load(std::memory_order_acquire);

      // Try to claim Empty or Deleted slots
      if (state == SlotState::Empty || state == SlotState::Deleted) {
        if (SlotState expected = state;
            m_entries_[probe_idx].state.compare_exchange_strong(
                expected, SlotState::Occupied, std::memory_order_acquire)) {
          // Successfully claimed slot
          m_entries_[probe_idx].ptr = ptr;
          m_entries_[probe_idx].size = size;
          m_entries_[probe_idx].category = cat;
          m_entries_[probe_idx].loc = loc;
          m_active_count_.fetch_add(1, std::memory_order_relaxed);
          return true;
        }
        // CAS failed - another thread claimed it, continue probing
      }
      // If Occupied, continue probing
    }

    // Registry full - track this so we know leaks might be invisible
    m_overflow_count_.fetch_add(1, std::memory_order_relaxed);
    return false;
  }

  auto Unregister(void* ptr) noexcept -> bool {
    size_t const kHash = std::hash<void*>{}(ptr);
    const size_t kIdx = kHash % Capacity;

    // Linear probing - must continue through Deleted slots
    for (size_t i = 0; i < Capacity; ++i) {
      size_t probe_idx = (kIdx + i) % Capacity;
      SlotState state =
          m_entries_[probe_idx].state.load(std::memory_order_acquire);

      // Stop only at Empty (never used)
      if (state == SlotState::Empty) {
        return false;  // Not found
      }

      // If Occupied and pointer matches, mark as Deleted
      if (state == SlotState::Occupied && m_entries_[probe_idx].ptr == ptr) {
        m_entries_[probe_idx].state.store(SlotState::Deleted,
                                          std::memory_order_release);
        m_active_count_.fetch_sub(1, std::memory_order_relaxed);
        return true;
      }

      // If Deleted, continue probing (critical for correctness)
    }

    return false;  // Searched entire table
  }

  auto DumpLeaks() const noexcept -> void {
    logger::CoreLogger::Warning("[MEMORY] Leak Detection Report:");
    size_t leak_count = 0;
    size_t leak_bytes = 0;

    // Scan entire table for Occupied slots
    for (const auto& entry : m_entries_) {
      if (entry.state.load(std::memory_order_relaxed) == SlotState::Occupied) {
        ++leak_count;
        leak_bytes += entry.size;
        logger::CoreLogger::Warning("  LEAK: {} bytes in {} at {}:{}",
                                    entry.size,
                                    rhodo::memory::CategoryName(entry.category),
                                    entry.loc.file_name(), entry.loc.line());
      }
    }

    size_t overflow = m_overflow_count_.load(std::memory_order_relaxed);

    if (leak_count == 0 && overflow == 0) {
      logger::CoreLogger::Info("[MEMORY] No leaks detected!");
    } else {
      if (leak_count > 0) {
        logger::CoreLogger::Error("[MEMORY] Total: {} leaks, {} bytes",
                                  leak_count, leak_bytes);
      }
      if (overflow > 0) {
        logger::CoreLogger::Warning(
            "[MEMORY] Registry overflow: {} allocations not tracked (increase "
            "capacity)",
            overflow);
      }
    }
  }

  // Debug helper - check registry health
  auto GetStats() const noexcept -> void {
    size_t empty_count = 0;
    size_t occupied_count = 0;
    size_t deleted_count = 0;

    for (const auto& entry : m_entries_) {
      switch (entry.state.load(std::memory_order_relaxed)) {
        case SlotState::Empty:
          ++empty_count;
          break;
        case SlotState::Occupied:
          ++occupied_count;
          break;
        case SlotState::Deleted:
          ++deleted_count;
          break;
        default:;
      }
    }

    logger::CoreLogger::Info(
        "[MEMORY] Registry: {} occupied, {} deleted, {} empty (load factor: "
        "{:.1f}%)",
        occupied_count, deleted_count, empty_count,
        (occupied_count + deleted_count) * 100.0 / Capacity);
  }
};

inline AllocationRegistry<4096> g_leak_registry;

// Hook Implementations (Private)
namespace detail {
auto g_on_allocation = [](void* ptr, size_t size,
                          rhodo::memory::MemoryCategory cat,
                          const std::source_location& loc) noexcept -> void {
  if constexpr (rhodo::kIsDebug) {
    // Global counters
    g_total_allocated.fetch_add(size, std::memory_order_relaxed);
    g_allocation_count.fetch_add(1, std::memory_order_relaxed);

    // Category tracking
    auto idx = rhodo::memory::CategoryIndex(cat);
    g_category_stats[idx].allocated.fetch_add(size, std::memory_order_relaxed);
    g_category_stats[idx].alloc_count.fetch_add(1, std::memory_order_relaxed);

    // Leak registry
    g_leak_registry.Register(ptr, size, cat, loc);
  }
};

auto g_on_deallocation =
    [](void* ptr, size_t size,
       rhodo::memory::MemoryCategory cat) noexcept -> void {
  if constexpr (rhodo::kIsDebug) {
    g_total_freed.fetch_add(size, std::memory_order_relaxed);
    g_deallocation_count.fetch_add(1, std::memory_order_relaxed);

    auto idx = rhodo::memory::CategoryIndex(cat);
    g_category_stats[idx].freed.fetch_add(size, std::memory_order_relaxed);
    g_category_stats[idx].free_count.fetch_add(1, std::memory_order_relaxed);

    g_leak_registry.Unregister(ptr);
  }
};
}  // namespace detail

// PUBLIC API: Hook Installation (Call Once at Startup)
export inline auto InstallMemoryHooks() noexcept -> void {
  rhodo::memory::SetAllocationHook(
      reinterpret_cast<rhodo::memory::AllocHookFn>(&detail::g_on_allocation));
  rhodo::memory::SetDeallocationHook(
      reinterpret_cast<rhodo::memory::FreeHookFn>(&detail::g_on_deallocation));

  logger::CoreLogger::Info("[DEBUG] Memory tracking hooks installed");
}

export inline auto UninstallMemoryHooks() noexcept -> void {
  rhodo::memory::ClearHooks();
  logger::CoreLogger::Info("[DEBUG] Memory tracking hooks removed");
}

// Query API
export inline auto TotalAllocated() noexcept -> size_t {
  return g_total_allocated.load(std::memory_order_relaxed);
}

export inline auto TotalFreed() noexcept -> size_t {
  return g_total_freed.load(std::memory_order_relaxed);
}

export inline auto NetAllocated() noexcept -> size_t {
  return TotalAllocated() - TotalFreed();
}

export inline auto CategoryNetAllocated(
    rhodo::memory::MemoryCategory cat) noexcept -> size_t {
  auto idx = rhodo::memory::CategoryIndex(cat);
  const auto& stats = g_category_stats[idx];
  return stats.allocated.load(std::memory_order_relaxed) -
         stats.freed.load(std::memory_order_relaxed);
}

export inline auto MarkFrameBoundary() noexcept -> void {
  if constexpr (rhodo::kIsDebug) {
    g_frame_tracker.NewFrame();
  }
}

// Reporting
export inline void PrintStats() noexcept {
  logger::CoreLogger::Info(
      "[MEMORY] Global: {} bytes ({} allocs), "
      "Freed: {} bytes ({} frees), Net: {} bytes",
      TotalAllocated(), g_allocation_count.load(std::memory_order_relaxed),
      TotalFreed(), g_deallocation_count.load(std::memory_order_relaxed),
      NetAllocated());

  logger::CoreLogger::Info("[MEMORY] By Category:");
  for (size_t i = 0; i < rhodo::memory::CategoryCount(); ++i) {
    auto cat = static_cast<rhodo::memory::MemoryCategory>(i);
    size_t net = CategoryNetAllocated(cat);
    if (net > 0) {
      logger::CoreLogger::Info("  {}: {} bytes active",
                               rhodo::memory::CategoryName(cat), net);
    }
  }

  // Frame stats
  auto last_frame = g_frame_tracker.GetFrameStats(0);
  auto avg_churn = g_frame_tracker.GetAverageChurn(60);
  logger::CoreLogger::Info(
      "[MEMORY] Last Frame: +{} bytes ({} allocs), -{} bytes ({} frees)",
      last_frame.bytes_allocated, last_frame.alloc_count,
      last_frame.bytes_freed, last_frame.free_count);
  logger::CoreLogger::Info("[MEMORY] Avg Churn (60f): {} bytes/frame",
                           avg_churn);
}

export inline void DumpLeaks() noexcept {
  if constexpr (rhodo::kIsDebug) {
    g_leak_registry.DumpLeaks();
  }
}

export inline void PrintRegistryStats() noexcept {
  if constexpr (rhodo::kIsDebug) {
    g_leak_registry.GetStats();
  }
}

}  // namespace rhodo::debug::memory
