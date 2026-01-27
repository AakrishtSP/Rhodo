#include <array>
#include <vector>
import Rhodo.Logger;
import Rhodo.Instrumentation;
import Rhodo.Debug;

auto main() -> int {
#ifdef RH_ENABLE_INSTRUMENTATION
  rhodo::instrumentation::Initialize();
#endif

  rhodo::debug::ScopedProfile p("Main");

  constexpr rhodo::logger::CoreLogger kCoreLogger;
  constexpr rhodo::logger::AppLogger kAppLogger;

  rhodo::logger::CoreLogger::Info("dash");

  rhodo::logger::CoreLogger::Info("Hello World!");

  constexpr int kValueA{5};
  constexpr std::array kValueV{1, 2, 3};
  const std::vector kValueVv{1, 2, 3};
  rhodo::logger::AppLogger::Info("int a {}", kValueA);
  rhodo::logger::AppLogger::Info("array v {}", kValueV);
  rhodo::logger::AppLogger::Info("vec vv {}", kValueVv);

#ifdef RH_ENABLE_INSTRUMENTATION
  rhodo::instrumentation::Shutdown();
#endif

  return 0;
}
