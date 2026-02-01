module;
#include <source_location>
#include <string_view>

export module Rhodo.Infrastructure.Instrumentation.Profiling;

import Rhodo.Infrastructure.Debug;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::profiling {

struct ProfilerInstrumentationTag {
  static auto Config() -> rhodo::core::logger::LoggerConfig {
    return {.name = "Profiler",
            .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
            .default_level = rhodo::core::logger::LogLevel::TraceL2};
  }
};
using Log = rhodo::core::logger::Logger<ProfilerInstrumentationTag>;

static void OnProfileBegin(const std::string_view name,
                           const std::source_location& loc) {
  Log::TraceL2("Begin Scope: {} ({}:{})", name.data(), loc.file_name(),
               loc.line());
}

static void OnProfileEnd() {
  Log::TraceL2("End Scope");
}

export void Initialize() {
  debug::SetProfilerHooks(OnProfileBegin, OnProfileEnd);
}

export void Shutdown() {
  debug::SetProfilerHooks(nullptr, nullptr);
}

}  // namespace rhodo::infrastructure::instrumentation::profiling
