module;
#include <source_location>
#include <iostream>

export module Rhodo.Infrastructure.Instrumentation.Memory;

import Rhodo.Core.Memory;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::memory {
    struct MemoryInstrumentationTag {
        static auto config() -> rhodo::core::logger::LoggerConfig {
            return {
                .name = "Memory",
                .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
                .default_level = rhodo::core::logger::LogLevel::TraceL3
            };
        }
    };

    using Log = rhodo::core::logger::Logger<MemoryInstrumentationTag>;

    void on_allocate(void *const ptr, const size_t size, const rhodo::core::memory::MemoryCategory cat,
                     const std::source_location &loc) noexcept {
        Log::trace_l3("Alloc: %p, Size: %zu, Cat: %d, Loc: %s:%d", ptr, size, static_cast<int>(cat), loc.file_name(),
                      loc.line());
    }

    void on_free(void *const ptr, const size_t size, const rhodo::core::memory::MemoryCategory cat) noexcept {
        Log::trace_l3("Free: %p, Size: %zu, Cat: %d", ptr, size, static_cast<int>(cat));
    }

    export void initialize() {
        rhodo::core::memory::set_allocation_hook(on_allocate);
        rhodo::core::memory::set_deallocation_hook(on_free);
    }

    export void shutdown() {
        rhodo::core::memory::clear_hooks();
    }
}
