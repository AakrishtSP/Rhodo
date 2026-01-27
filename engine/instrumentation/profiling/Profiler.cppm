module;
#include <source_location>
#include <string_view>

export module Rhodo.Instrumentation.Profiling;

import Rhodo.Debug;
import Rhodo.Logger;

namespace rhodo::instrumentation::profiling {

struct ProfilerInstrumentationTag {
    static auto Config() -> rhodo::logger::LoggerConfig {
        return {
            .name = "Profiler",
            .sinks = {{rhodo::logger::SinkType::Console, "console"}},
            .default_level = rhodo::logger::LogLevel::TraceL2
        };
    }
};
using Log = rhodo::logger::Logger<ProfilerInstrumentationTag>;

void OnProfileBegin(std::string_view name, const std::source_location& loc) {
    Log::TraceL2("Begin Scope: %s (%s:%d)", name.data(), loc.file_name(), loc.line());
}

void OnProfileEnd() {
    Log::TraceL2("End Scope");
}

export void Initialize() {
    rhodo::debug::SetProfilerHooks(OnProfileBegin, OnProfileEnd);
}

export void Shutdown() {
    rhodo::debug::SetProfilerHooks(nullptr, nullptr);
}

}
