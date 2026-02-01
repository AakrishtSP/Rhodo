#include <array>
#include <string_view>
#include <vector>
import Rhodo.Core.Logger;

#ifdef RH_ENABLE_INSTRUMENTATION
import Rhodo.Infrastructure.Instrumentation;
import Rhodo.Infrastructure.Debug;
#endif

auto main() -> int {
#ifdef RH_ENABLE_INSTRUMENTATION
  rhodo::infrastructure::instrumentation::Initialize();
  const rhodo::infrastructure::debug::ScopedProfile p("Main");
  rhodo::core::logger::AppLogger::Info("Infrastructure Initialized");
#endif

  rhodo::core::logger::CoreLogger::Info("Hello World!");

  constexpr int kValueA{5};
  constexpr std::array kValueV{1, 2, 3};
  const std::vector value_vv{1, 2, 3};
  rhodo::core::logger::AppLogger::Info("int a {}", kValueA);
  rhodo::core::logger::AppLogger::Info("array v {}", kValueV);
  rhodo::core::logger::AppLogger::Info("vec vv {}", value_vv);

#ifdef RH_ENABLE_INSTRUMENTATION
  rhodo::infrastructure::instrumentation::Shutdown();
  rhodo::core::logger::AppLogger::Info("Infrastructure deInitialized");
#endif

  return 0;
}
