export module Rhodo.Infrastructure.Instrumentation;

export import Rhodo.Infrastructure.Instrumentation.Memory;
export import Rhodo.Infrastructure.Instrumentation.Signals;
export import Rhodo.Infrastructure.Instrumentation.Profiling;

namespace rhodo::infrastructure::instrumentation {

export void Initialize() {
    memory::Initialize();
    signals::Initialize();
    profiling::Initialize();
}

export void Shutdown() {
    profiling::Shutdown();
    signals::Shutdown();
    memory::Shutdown();
}

}
