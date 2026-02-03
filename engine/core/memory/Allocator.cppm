module;
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <source_location>

export module Rhodo.Core.Memory:Allocator;

import :Categories;
import :Hooks;

export namespace rhodo::core::memory {

// Allocation Header (Fixed Alignment Issue)
struct alignas(std::max_align_t) AllocationHeader {
  uint32_t magic{};
  size_t size{};      // Changed from uint32_t - no 4GB limit
  uint32_t offset{};  // Distance from this header to raw allocation
  MemoryCategory category{};

  static constexpr uint32_t k_magic = 0xDEADC0DE;
  static constexpr uint32_t k_freed_magic = 0xDEADF00D;

  static auto from_user_ptr(void* user_ptr) noexcept -> AllocationHeader* {
    // NOLINTBEGIN(*-pro-type-reinterpret-cast,
    // *-pro-bounds-pointer-arithmetic)
    //   → Safe: recovering header we placed immediately before user_ptr
    return reinterpret_cast<AllocationHeader*>(static_cast<char*>(user_ptr) -
                                               sizeof(AllocationHeader));
    // NOLINTEND(*-pro-type-reinterpret-cast,
    // *-pro-bounds-pointer-arithmetic)
  }

  [[nodiscard]] auto is_valid() const noexcept -> bool {
    return magic == k_magic;
  }

  auto mark_freed() noexcept -> void { magic = k_freed_magic; }
};

// Guard Bytes (Optional - Debug Only)
#if defined(RHODO_DEBUG) && defined(RHODO_MEMORY_GUARDS)
namespace detail {
constexpr uint8_t kGuardByte = 0xFD;
constexpr size_t kGuardSize = 16;

inline auto FillGuard(void* ptr) noexcept -> void {
  std::memset(ptr, kGuardByte, kGuardSize);
}

inline auto CheckGuard(void* ptr, const char* location) noexcept -> bool {
  auto* bytes = static_cast<uint8_t*>(ptr);
  for (size_t i = 0; i < kGuardSize; ++i) {
    if (bytes[i] != kGuardByte) {
      // Guard corruption detected - in a real engine, log/assert here
      return false;
    }
  }
  return true;
}
}  // namespace detail
#else
namespace detail {
constexpr size_t k_guard_size = 0;
inline auto fill_guard(void* /*ptr*/) noexcept -> void {}
inline auto check_guard(void* /*ptr*/, const char* /*location*/) noexcept
    -> bool {
  return true;
}
}  // namespace detail
#endif

// -----------------------------------------------------------------------------
// INTERNAL / CORE API (No Instrumentation)
// -----------------------------------------------------------------------------

namespace internal {

inline auto allocate_core(const size_t size, const size_t alignment,
                          const MemoryCategory category) noexcept -> void* {
  // Validate alignment is power of 2
  assert(std::has_single_bit(alignment) && "Alignment must be power of 2");

  // Ensure header is properly aligned
  constexpr size_t k_header_align = alignof(AllocationHeader);
  const size_t effective_alignment =
      (alignment > k_header_align) ? alignment : k_header_align;

  // Calculate total space:
  // [padding][Header][guard][user_data][guard]
  constexpr size_t k_header_size = sizeof(AllocationHeader);
  const size_t total_needed =
      size + k_header_size + (2 * detail::k_guard_size) + effective_alignment;

  // Raw allocation
  // NOLINTBEGIN(*-no-malloc, *-owning-memory)
  void* raw_ptr = std::malloc(total_needed);  // NOLINT(*-const-correctness)
  if (raw_ptr == nullptr) [[unlikely]] {
    return nullptr;
  }
  // NOLINTEND(*-no-malloc, *-owning-memory)

  // Calculate aligned user address
  // User pointer must be aligned AND have space for header + front guard
  // NOLINTBEGIN(*-pro-type-reinterpret-cast)
  const auto raw_addr = reinterpret_cast<uintptr_t>(raw_ptr);
  // NOLINTEND(*-pro-type-reinterpret-cast)
  const uintptr_t min_user_addr =
      raw_addr + k_header_size + detail::k_guard_size;
  const uintptr_t aligned_user_addr =
      (min_user_addr + effective_alignment - 1) & ~(effective_alignment - 1);

  // Place header immediately before front guard
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  auto* header = reinterpret_cast<AllocationHeader*>(
      aligned_user_addr - detail::k_guard_size - k_header_size);
  // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  header->magic = AllocationHeader::k_magic;
  header->size = size;
  // NOLINTBEGIN(*-pro-type-reinterpret-cast)
  header->offset =
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(header) - raw_addr);
  // NOLINTEND(*-pro-type-reinterpret-cast)

  assert(header->offset < kTotalNeeded &&
         "Offset overflow - alignment too large");

  header->category = category;

  // Fill guard bytes
  if constexpr (detail::k_guard_size > 0) {
    // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
    auto* front_guard =
        reinterpret_cast<void*>(aligned_user_addr - detail::k_guard_size);
    auto* back_guard = reinterpret_cast<void*>(aligned_user_addr + size);
    // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
    detail::fill_guard(front_guard);
    detail::fill_guard(back_guard);
  }
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  auto* user_ptr = reinterpret_cast<void*>(aligned_user_addr);
  // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)

  return user_ptr;
}

inline auto deallocate_core(void* user_ptr) noexcept -> void {
  if (user_ptr == nullptr) {
    return;
  }

  // Retrieve and validate header
  AllocationHeader* header = AllocationHeader::from_user_ptr(user_ptr);

  if (!header->is_valid()) [[unlikely]] {
    // Corruption or double-free detected
    assert(false && "Memory corruption or double-free detected!");
    return;
  }

  // Check guard bytes
  if constexpr (detail::k_guard_size > 0) {
    // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
    void* front_guard = static_cast<char*>(user_ptr) - detail::k_guard_size;
    void* back_guard = static_cast<char*>(user_ptr) + header->size;
    // NOLINTEND(*-pro-bounds-pointer-arithmetic)

    bool const front_ok = detail::check_guard(front_guard, "front");
    if (bool const back_ok = detail::check_guard(back_guard, "back");
        !front_ok || !back_ok) [[unlikely]] {
      // Buffer under/overflow detected
      assert(false && "Buffer overflow/underflow detected!");
    }
  }

  // Calculate original raw pointer
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-pro-bounds-pointer-arithmetic)
  void* raw_ptr = reinterpret_cast<char*>(header) - header->offset;
  // NOLINTEND(*-pro-type-reinterpret-cast, *-pro-bounds-pointer-arithmetic)

  header->mark_freed();
  std::free(raw_ptr);  // NOLINT(*-owning-memory, *-no-malloc)
}

}  // namespace internal

// -----------------------------------------------------------------------------
// PUBLIC API (With Instrumentation)
// -----------------------------------------------------------------------------

// Core Allocation Function
inline auto allocate(const size_t size, const size_t alignment,
                     const MemoryCategory category = MemoryCategory::Generic,
                     const std::source_location& loc =
                         std::source_location::current()) noexcept -> void* {
  // 1. Perform the actual allocation
  void* ptr = internal::allocate_core(size, alignment, category);

  // 2. Instrument it (if successful)
  if (ptr != nullptr) {
    notify_allocation(ptr, size, category, loc);
  }

  return ptr;
}

// Core Deallocation Function
inline auto deallocate(void* user_ptr) noexcept -> void {
  if (user_ptr == nullptr) {
    return;
  }

  // We need to peek at the header to get size/category for the hook
  // BEFORE we free the memory.

  // 1. Instrument it
  if (const AllocationHeader* header =
          AllocationHeader::from_user_ptr(user_ptr);
      header->is_valid()) {
    notify_deallocation(user_ptr, header->size, header->category);
  }

  // 2. Perform the actual deallocation
  internal::deallocate_core(user_ptr);
}

// RAII Wrapper (Renamed per feedback)
template <typename T>
class TrackedBuffer {
  T* m_ptr_{nullptr};
  MemoryCategory m_category_{MemoryCategory::Generic};

 public:
  TrackedBuffer() = default;

  explicit TrackedBuffer(
      const size_t count, const size_t alignment = alignof(T),
      const MemoryCategory cat = MemoryCategory::Generic,
      const std::source_location& loc = std::source_location::current())
      : m_ptr_(
            static_cast<T*>(allocate(sizeof(T) * count, alignment, cat, loc))),
        m_category_(cat) {
    // m_ptr_ = static_cast<T*>(Allocate(sizeof(T) * count, alignment, cat,
    // loc));
  }

  ~TrackedBuffer() {
    if (m_ptr_) {
      Deallocate(m_ptr_);
    }
  }

  // Move-only semantics
  TrackedBuffer(TrackedBuffer&& other) noexcept
      : m_ptr_(other.m_ptr_), m_category_(other.m_category_) {
    other.m_ptr_ = nullptr;
  }

  auto operator=(TrackedBuffer&& other) noexcept -> TrackedBuffer& {
    if (this != &other) {
      if (m_ptr_) {
        Deallocate(m_ptr_);
      }
      m_ptr_ = other.m_ptr_;
      m_category_ = other.m_category_;
      other.m_ptr_ = nullptr;
    }
    return *this;
  }

  TrackedBuffer(const TrackedBuffer&) = delete;
  auto operator=(const TrackedBuffer&) -> TrackedBuffer& = delete;

  // Access operators
  auto get() const noexcept -> T* { return m_ptr_; }
  auto operator->() const noexcept -> T* { return m_ptr_; }
  auto operator*() const noexcept -> T& { return *m_ptr_; }
  // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
  auto operator[](size_t idx) const noexcept -> T& {
    return m_ptr_[idx];
  }  // NOLINTEND(*-pro-bounds-pointer-arithmetic)

  explicit operator bool() const noexcept { return m_ptr_ != nullptr; }
};

}  // namespace rhodo::core::memory
