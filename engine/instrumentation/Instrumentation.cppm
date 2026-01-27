export module Rhodo.Instrumentation;

export import Rhodo.Instrumentation.Memory;
export import Rhodo.Instrumentation.Signals;
export import Rhodo.Instrumentation.Profiling;

namespace rhodo::instrumentation {

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
