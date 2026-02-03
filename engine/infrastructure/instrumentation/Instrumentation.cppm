export module Rhodo.Infrastructure.Instrumentation;

export import Rhodo.Infrastructure.Instrumentation.Memory;
export import Rhodo.Infrastructure.Instrumentation.Signals;
export import Rhodo.Infrastructure.Instrumentation.Profiling;

namespace rhodo::infrastructure::instrumentation {
    export void initialize() {
        memory::initialize();
        signals::initialize();
        profiling::initialize();
    }

    export void shutdown() {
        profiling::shutdown();
        signals::shutdown();
        memory::shutdown();
    }
}
