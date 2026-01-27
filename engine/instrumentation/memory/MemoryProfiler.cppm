module;
#include <source_location>
#include <iostream>

export module Rhodo.Instrumentation.Memory;

import Rhodo.Memory;
import Rhodo.Logger;

namespace rhodo::instrumentation::memory {

struct MemoryInstrumentationTag {
    static auto Config() -> rhodo::logger::LoggerConfig {
        return {
            .name = "Memory",
            .sinks = {{rhodo::logger::SinkType::Console, "console"}},
            .default_level = rhodo::logger::LogLevel::TraceL3
        };
    }
};
using Log = rhodo::logger::Logger<MemoryInstrumentationTag>;

void OnAllocate(void* const ptr, const size_t size, const rhodo::memory::MemoryCategory cat, const std::source_location& loc) noexcept {
    Log::TraceL3("Alloc: %p, Size: %zu, Cat: %d, Loc: %s:%d", ptr, size, static_cast<int>(cat), loc.file_name(), loc.line());
}

void OnFree(void* const ptr, const size_t size, const rhodo::memory::MemoryCategory cat) noexcept {
    Log::TraceL3("Free: %p, Size: %zu, Cat: %d", ptr, size, static_cast<int>(cat));
}

export void Initialize() {
    rhodo::memory::SetAllocationHook(OnAllocate);
    rhodo::memory::SetDeallocationHook(OnFree);
}

export void Shutdown() {
    rhodo::memory::ClearHooks();
}

}
