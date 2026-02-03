module;
#include <source_location>
#include <string_view>

export module Rhodo.Infrastructure.Instrumentation.Signals;

import Rhodo.Core.Signals;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::signals {
struct SignalInstrumentationTag {
    static auto config() -> rhodo::core::logger::LoggerConfig {
        return {.name = "Signals",
                .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
                .default_level = rhodo::core::logger::LogLevel::TraceL2};
    }
};

using Log = rhodo::core::logger::Logger<SignalInstrumentationTag>;

void on_emit_begin(const std::string_view name,
                   const std::source_location& loc) noexcept {
    Log::trace_l2("Signal Begin: %s, Loc: %s:%d", name.data(), loc.file_name(),
                  loc.line());
}

void on_emit_end() noexcept {
    Log::trace_l2("Signal End");
}

export void initialize() {
    rhodo::core::signals::set_emit_begin_hook(on_emit_begin);
    rhodo::core::signals::set_emit_end_hook(on_emit_end);
}

export void shutdown() {
    rhodo::core::signals::clear_hooks();
}
}  // namespace rhodo::infrastructure::instrumentation::signals
