#include <array>
#include <vector>
import Rhodo.Core.Logger;

#ifdef RH_ENABLE_INSTRUMENTATION
import Rhodo.Infrastructure.Instrumentation;
import Rhodo.Infrastructure.Debug;
#endif

auto main() -> int {
#ifdef RH_ENABLE_INSTRUMENTATION
    instrumentation::initialize();
    const debug::ScopedProfile p("Main");
    logger::AppLogger::info("Infrastructure Initialized");
#endif

    logger::CoreLogger::info("Hello World!");

    constexpr int k_value_a{5};
    constexpr std::array k_value_v{1, 2, 3};
    const std::vector value_vv{1, 2, 3};
    logger::AppLogger::info("int a {}", k_value_a);
    logger::AppLogger::info("array v {}", k_value_v);
    logger::AppLogger::info("vec vv {}", value_vv);

#ifdef RH_ENABLE_INSTRUMENTATION
    instrumentation::shutdown();
    logger::AppLogger::info("Infrastructure deInitialized");
#endif

    return 0;
}
