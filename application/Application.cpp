#include <array>
#include <vector>
import Rhodo.Logger;

auto main() -> int {
  constexpr rhodo::logger::CoreLogger kCoreLogger;
  constexpr rhodo::logger::AppLogger kAppLogger;

  rhodo::logger::CoreLogger::Info("dasd");

  rhodo::logger::CoreLogger::Info("Hello World!");

  constexpr int kValueA{5};
  constexpr std::array kValueV{1, 2, 3};
  const std::vector kValueVv{1, 2, 3};
  rhodo::logger::AppLogger::Info("int a {}", kValueA);
  rhodo::logger::AppLogger::Info("array v {}", kValueV);
  rhodo::logger::AppLogger::Info("vec vv {}", kValueVv);

  return 0;
}
