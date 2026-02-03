module;
#include <source_location>
#include <string_view>

export module Rhodo.Infrastructure.Debug:Profiling;

export namespace rhodo::infrastructure::debug {

using ProfileBeginFn = void (*)(std::string_view name,
                                const std::source_location& loc);
using ProfileEndFn = void (*)();

inline ProfileBeginFn g_profile_begin = nullptr;
inline ProfileEndFn g_profile_end = nullptr;

inline void set_profiler_hooks(const ProfileBeginFn begin,
                             const ProfileEndFn end) {
  if (begin != nullptr) {
    g_profile_begin = begin;
  }
  if (end != nullptr) {
    g_profile_end = end;
  }
}

class ScopedProfile {
 public:
  explicit ScopedProfile(
      const std::string_view name = "",
      const std::source_location& loc = std::source_location::current()) {
    if (g_profile_begin != nullptr) {
      // If name is empty, use function name from location
      std::string_view const final_name =
          name.empty() ? loc.function_name() : name;
      g_profile_begin(final_name, loc);
    }
  }

  ScopedProfile(ScopedProfile&&) = delete;
  auto operator=(ScopedProfile&&) -> ScopedProfile& = delete;

  ~ScopedProfile() {
    if (g_profile_end != nullptr) {
      g_profile_end();
    }
  }

  ScopedProfile(const ScopedProfile&) = delete;
  auto operator=(const ScopedProfile&) -> ScopedProfile& = delete;
};

}  // namespace rhodo::infrastructure::debug
