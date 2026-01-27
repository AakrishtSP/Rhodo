module;
#include <source_location>
#include <string_view>

export module Rhodo.Instrumentation.Signals;

import Rhodo.Signals;
import Rhodo.Logger;

namespace rhodo::instrumentation::signals {

struct SignalInstrumentationTag {
    static auto Config() -> rhodo::logger::LoggerConfig {
        return {
            .name = "Signals",
            .sinks = {{rhodo::logger::SinkType::Console, "console"}},
            .default_level = rhodo::logger::LogLevel::TraceL2
        };
    }
};
using Log = rhodo::logger::Logger<SignalInstrumentationTag>;

void OnEmitBegin(const std::string_view name, const std::source_location& loc) noexcept {
    Log::TraceL2("Signal Begin: %s, Loc: %s:%d", name.data(), loc.file_name(), loc.line());
}

void OnEmitEnd() noexcept {
    Log::TraceL2("Signal End");
}

export void Initialize() {
    rhodo::signals::SetEmitBeginHook(OnEmitBegin);
    rhodo::signals::SetEmitEndHook(OnEmitEnd);
}

export void Shutdown() {
    rhodo::signals::ClearHooks();
}

}
