export module Rhodo.Memory:Categories;

import <array>;
import <cstdint>;
import <string_view>;

namespace rhodo::memory {

#define RHODO_MEMORY_CATEGORIES(X) \
  X(Generic)                       \
  X(Renderer)                      \
  X(Physics)                       \
  X(Audio)                         \
  X(ECS)                           \
  X(UI)                            \
  X(Scripting)                     \
  X(Streaming)                     \
  X(Animation)                     \
  X(Navigation)                    \
  X(Networking)

// ============================================================================
// Category Enum
// ============================================================================
export enum class MemoryCategory : uint8_t {
#define X(name) name,
  RHODO_MEMORY_CATEGORIES(X)
#undef X
  Count
};

namespace detail {
constexpr std::array kCategoryNames = {
#define X(name) std::string_view{#name},
    RHODO_MEMORY_CATEGORIES(X)
#undef X
};

static_assert(kCategoryNames.size() ==
                  static_cast<size_t>(MemoryCategory::Count),
              "Category name count must match enum count");
}  // namespace detail

export constexpr auto CategoryName(MemoryCategory cat) noexcept
    -> std::string_view {
  const auto kIdx = static_cast<size_t>(cat);
  if (kIdx >= static_cast<size_t>(MemoryCategory::Count)) {
    return "Invalid";
  }
  return detail::kCategoryNames[kIdx];
}

export constexpr auto CategoryIndex(MemoryCategory cat) noexcept -> size_t {
  return static_cast<size_t>(cat);
}

export constexpr auto CategoryCount() noexcept -> size_t {
  return static_cast<size_t>(MemoryCategory::Count);
}

}  // namespace rhodo::memory
