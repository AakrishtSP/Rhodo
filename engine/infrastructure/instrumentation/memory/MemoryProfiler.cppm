module;
#include <source_location>
#include <iostream>

export module Rhodo.Infrastructure.Instrumentation.Memory;

import Rhodo.Core.Memory;
import Rhodo.Core.Logger;

namespace rhodo::infrastructure::instrumentation::memory {

struct MemoryInstrumentationTag {
    static auto Config() -> rhodo::core::logger::LoggerConfig {
        return {
            .name = "Memory",
            .sinks = {{rhodo::core::logger::SinkType::Console, "console"}},
            .default_level = rhodo::core::logger::LogLevel::TraceL3
        };
    }
};
using Log = rhodo::core::logger::Logger<MemoryInstrumentationTag>;

void OnAllocate(void* const ptr, const size_t size, const rhodo::core::memory::MemoryCategory cat, const std::source_location& loc) noexcept {
    Log::TraceL3("Alloc: %p, Size: %zu, Cat: %d, Loc: %s:%d", ptr, size, static_cast<int>(cat), loc.file_name(), loc.line());
}

void OnFree(void* const ptr, const size_t size, const rhodo::core::memory::MemoryCategory cat) noexcept {
    Log::TraceL3("Free: %p, Size: %zu, Cat: %d", ptr, size, static_cast<int>(cat));
}

export void Initialize() {
    rhodo::core::memory::SetAllocationHook(OnAllocate);
    rhodo::core::memory::SetDeallocationHook(OnFree);
}

export void Shutdown() {
    rhodo::core::memory::ClearHooks();
}

}
