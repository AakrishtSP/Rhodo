module;
#include <array>
#include <cstdint>
#include <string_view>

export module Rhodo.Core.Memory:Categories;


namespace rhodo::core::memory {
// NOLINTBEGIN(*-macro-usage)
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
// NOLINTEND(*-macro-usage)

export enum class MemoryCategory : uint8_t {
  //NOLINTBEGIN(*-macro-usage)
#define X(name) name,
  RHODO_MEMORY_CATEGORIES(X)
#undef X
  //NOLINTEND(*-macro-usage)
  Count
};

namespace detail {
constexpr std::array k_category_names = {
// NOLINTBEGIN(*-macro-usage)
#define X(name) std::string_view{#name},
    RHODO_MEMORY_CATEGORIES(X)
#undef X
    // NOLINTEND(*-macro-usage)
};

static_assert(k_category_names.size() ==
                  static_cast<size_t>(MemoryCategory::Count),
              "Category name count must match enum count");
}  // namespace detail

export constexpr auto category_name(MemoryCategory cat) noexcept
    -> std::string_view {
  const auto id = static_cast<size_t>(cat);
  if (id >= static_cast<size_t>(MemoryCategory::Count)) {
    return "Invalid";
  }
  return detail::k_category_names
      [id];  // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
}

export constexpr auto category_index(MemoryCategory cat) noexcept -> size_t {
  return static_cast<size_t>(cat);
}

export constexpr auto category_count() noexcept -> size_t {
  return static_cast<size_t>(MemoryCategory::Count);
}

}  // namespace rhodo::core::memory
