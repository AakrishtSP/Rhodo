#include <array>
#include <vector>
import Rhodo.Logger;

auto main() -> int {
  rhodo::logger::coreLogger core_logger;
  rhodo::logger::appLogger app_logger;

  rhodo::logger::coreLogger::info("dasd");

  core_logger.info("Hello World!");

  constexpr int kValueA{5};
  std::array value_v{1, 2, 3};
  std::vector value_vv{1, 2, 3};
  app_logger.info("int a {}", kValueA);
  app_logger.info("array v {}", value_v);
  app_logger.info("vec vv {}", value_vv);

  return 0;
}
