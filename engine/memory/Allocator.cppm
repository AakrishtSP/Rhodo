export module Rhodo.Memory:Allocator;

import :Categories;
import :Hooks;
import <cstdlib>;
import <cstdint>;
import <cassert>;
import <source_location>;
import <bit>;

export namespace rhodo::memory {

// Allocation Header (Fixed Alignment Issue)
struct alignas(std::max_align_t) AllocationHeader {
  uint32_t magic{};
  size_t size{};      // Changed from uint32_t - no 4GB limit
  uint32_t offset{};  // Distance from this header to raw allocation
  MemoryCategory category{};

  static constexpr uint32_t kMagic = 0xDEADC0DE;
  static constexpr uint32_t kFreedMagic = 0xDEADF00D;

  static auto FromUserPtr(void* user_ptr) noexcept -> AllocationHeader* {
    // NOLINTBEGIN(*-pro-type-reinterpret-cast,
    // *-pro-bounds-pointer-arithmetic)
    //   → Safe: recovering header we placed immediately before user_ptr
    return reinterpret_cast<AllocationHeader*>(static_cast<char*>(user_ptr) -
                                               sizeof(AllocationHeader));
    // NOLINTEND(*-pro-type-reinterpret-cast,
    // *-pro-bounds-pointer-arithmetic)
  }

  [[nodiscard]] auto IsValid() const noexcept -> bool {
    return magic == kMagic;
  }

  auto MarkFreed() noexcept -> void { magic = kFreedMagic; }
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
constexpr size_t kGuardSize = 0;
inline auto FillGuard(void* /*ptr*/) noexcept -> void {}
inline auto CheckGuard(void* /*ptr*/, const char* /*location*/) noexcept
    -> bool {
  return true;
}
}  // namespace detail
#endif

// Core Allocation Function
inline auto Allocate(const size_t size, const size_t alignment,
                     const MemoryCategory category = MemoryCategory::Generic,
                     const std::source_location& loc =
                         std::source_location::current()) noexcept -> void* {
  // Validate alignment is power of 2
  assert(std::has_single_bit(alignment) && "Alignment must be power of 2");

  // Ensure header is properly aligned
  constexpr size_t kHeaderAlign = alignof(AllocationHeader);
  const size_t kEffectiveAlignment =
      (alignment > kHeaderAlign) ? alignment : kHeaderAlign;

  // Calculate total space:
  // [padding][Header][guard][user_data][guard]
  constexpr size_t kHeaderSize = sizeof(AllocationHeader);
  const size_t kTotalNeeded =
      size + kHeaderSize + (2 * detail::kGuardSize) + kEffectiveAlignment;

  // Raw allocation
  // NOLINTBEGIN(*-no-malloc, *-owning-memory)
  void* raw_ptr = std::malloc(kTotalNeeded);  // NOLINT(*-const-correctness)
  if (raw_ptr == nullptr) [[unlikely]] {
    return nullptr;
  }
  // NOLINTEND(*-no-malloc, *-owning-memory)

  // Calculate aligned user address
  // User pointer must be aligned AND have space for header + front guard
  // NOLINTBEGIN(*-pro-type-reinterpret-cast)
  const auto kRawAddr = reinterpret_cast<uintptr_t>(raw_ptr);
  // NOLINTEND(*-pro-type-reinterpret-cast)
  const uintptr_t kMinUserAddr = kRawAddr + kHeaderSize + detail::kGuardSize;
  const uintptr_t kAlignedUserAddr =
      (kMinUserAddr + kEffectiveAlignment - 1) & ~(kEffectiveAlignment - 1);

  // Place header immediately before front guard
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  auto* header = reinterpret_cast<AllocationHeader*>(
      kAlignedUserAddr - detail::kGuardSize - kHeaderSize);
  // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  header->magic = AllocationHeader::kMagic;
  header->size = size;
  // NOLINTBEGIN(*-pro-type-reinterpret-cast)
  header->offset =
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(header) - kRawAddr);
  // NOLINTEND(*-pro-type-reinterpret-cast)

  assert(header->offset < kTotalNeeded &&
         "Offset overflow - alignment too large");

  header->category = category;

  // Fill guard bytes
  if constexpr (detail::kGuardSize > 0) {
    // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
    auto* front_guard =
        reinterpret_cast<void*>(kAlignedUserAddr - detail::kGuardSize);
    auto* back_guard = reinterpret_cast<void*>(kAlignedUserAddr + size);
    // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
    detail::FillGuard(front_guard);
    detail::FillGuard(back_guard);
  }
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
  auto* user_ptr = reinterpret_cast<void*>(kAlignedUserAddr);
  // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)

  // Notify observers (hook may be null - that's fine)
  NotifyAllocation(user_ptr, size, category, loc);

  return user_ptr;
}

// Core Deallocation Function
inline auto Deallocate(void* user_ptr) noexcept -> void {
  if (user_ptr == nullptr) {
    return;
  }

  // Retrieve and validate header
  AllocationHeader* header = AllocationHeader::FromUserPtr(user_ptr);

  if (!header->IsValid()) [[unlikely]] {
    // Corruption or double-free detected
    assert(false && "Memory corruption or double-free detected!");
    return;
  }

  // Check guard bytes
  if constexpr (detail::kGuardSize > 0) {
    // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
    void* front_guard = static_cast<char*>(user_ptr) - detail::kGuardSize;
    void* back_guard = static_cast<char*>(user_ptr) + header->size;
    // NOLINTEND(*-pro-bounds-pointer-arithmetic)

    bool const kFrontOk = detail::CheckGuard(front_guard, "front");
    if (bool const kBackOk = detail::CheckGuard(back_guard, "back");
        !kFrontOk || !kBackOk) [[unlikely]] {
      // Buffer under/overflow detected
      assert(false && "Buffer overflow/underflow detected!");
    }
  }

  // Calculate original raw pointer
  // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-pro-bounds-pointer-arithmetic)
  void* raw_ptr = reinterpret_cast<char*>(header) - header->offset;
  // NOLINTEND(*-pro-type-reinterpret-cast, *-pro-bounds-pointer-arithmetic)

  // Notify observers BEFORE freeing
  NotifyDeallocation(user_ptr, header->size, header->category);

  // Mark as freed (helps catch double-free)
  header->MarkFreed();

  // Free original allocation
  std::free(raw_ptr);  // NOLINT(*-owning-memory, *-no-malloc)
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
      : m_category_(cat) {
    m_ptr_ = static_cast<T*>(Allocate(sizeof(T) * count, alignment, cat, loc));
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
  auto Get() const noexcept -> T* { return m_ptr_; }
  auto operator->() const noexcept -> T* { return m_ptr_; }
  auto operator*() const noexcept -> T& { return *m_ptr_; }
  // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
  auto operator[](size_t idx) const noexcept -> T& {
    return m_ptr_[idx];
  }  // NOLINTEND(*-pro-bounds-pointer-arithmetic)

  explicit operator bool() const noexcept { return m_ptr_ != nullptr; }
};

}  // namespace rhodo::memory
