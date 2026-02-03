module;
#include <source_location>
#include <string_view>

export module Rhodo.Infrastructure.Instrumentation.Profiling;

import Rhodo.Infrastructure.Debug;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::profiling {
    struct ProfilerInstrumentationTag {
        static auto config() -> rhodo::core::logger::LoggerConfig {
            return {
                .name = "Profiler",
                .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
                .default_level = rhodo::core::logger::LogLevel::TraceL2
            };
        }
    };

    using Log = rhodo::core::logger::Logger<ProfilerInstrumentationTag>;

    static void on_profile_begin(const std::string_view name,
                                 const std::source_location &loc) {
        Log::trace_l2("Begin Scope: {} ({}:{})", name.data(), loc.file_name(),
                      loc.line());
    }

    static void on_profile_end() {
        Log::trace_l2("End Scope");
    }

    export void initialize() {
        debug::set_profiler_hooks(on_profile_begin, on_profile_end);
    }

    export void shutdown() {
        debug::set_profiler_hooks(nullptr, nullptr);
    }
} // namespace rhodo::infrastructure::instrumentation::profiling
