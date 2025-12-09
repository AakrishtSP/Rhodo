#include <array>
#include <vector>
import Rhodo.Logger;

auto main() -> int {
  constexpr rhodo::logger::CoreLogger kCoreLogger;
  constexpr rhodo::logger::AppLogger kAppLogger;

  rhodo::logger::CoreLogger::Info("dasd");

  rhodo::logger::CoreLogger::Info("Hello World!");

  constexpr int kValueA{5};
  std::array value_v{1, 2, 3};
  std::vector value_vv{1, 2, 3};
  rhodo::logger::AppLogger::Info("int a {}", kValueA);
  rhodo::logger::AppLogger::Info("array v {}", value_v);
  rhodo::logger::AppLogger::Info("vec vv {}", value_vv);

  return 0;
}
