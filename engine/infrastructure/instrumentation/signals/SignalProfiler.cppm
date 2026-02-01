module;
#include <source_location>
#include <string_view>

export module Rhodo.Infrastructure.Instrumentation.Signals;

import Rhodo.Core.Signals;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::signals {

struct SignalInstrumentationTag {
    static auto Config() -> rhodo::core::logger::LoggerConfig {
        return {
            .name = "Signals",
            .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
            .default_level = rhodo::core::logger::LogLevel::TraceL2
        };
    }
};
using Log = rhodo::core::logger::Logger<SignalInstrumentationTag>;

void OnEmitBegin(const std::string_view name, const std::source_location& loc) noexcept {
    Log::TraceL2("Signal Begin: %s, Loc: %s:%d", name.data(), loc.file_name(), loc.line());
}

void OnEmitEnd() noexcept {
    Log::TraceL2("Signal End");
}

export void Initialize() {
    rhodo::core::signals::SetEmitBeginHook(OnEmitBegin);
    rhodo::core::signals::SetEmitEndHook(OnEmitEnd);
}

export void Shutdown() {
    rhodo::core::signals::ClearHooks();
}

}
