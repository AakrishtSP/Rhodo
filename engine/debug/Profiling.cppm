module;
#include <source_location>
#include <string_view>

export module Rhodo.Debug:Profiling;

export namespace rhodo::debug {

    using ProfileBeginFn = void(*)(std::string_view name, const std::source_location& loc);
    using ProfileEndFn = void(*)();

    inline ProfileBeginFn g_profile_begin = nullptr;
    inline ProfileEndFn g_profile_end = nullptr;

    inline void SetProfilerHooks(ProfileBeginFn begin, ProfileEndFn end) {
        g_profile_begin = begin;
        g_profile_end = end;
    }

    class ScopedProfile {
    public:
        explicit ScopedProfile(std::string_view name = "", 
                               const std::source_location& loc = std::source_location::current()) {
            if (g_profile_begin) {
                // If name is empty, use function name from location
                std::string_view final_name = name.empty() ? loc.function_name() : name;
                g_profile_begin(final_name, loc);
            }
        }

        ~ScopedProfile() {
            if (g_profile_end) {
                g_profile_end();
            }
        }

        ScopedProfile(const ScopedProfile&) = delete;
        ScopedProfile& operator=(const ScopedProfile&) = delete;
    };

}
