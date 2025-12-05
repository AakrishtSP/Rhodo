#include <array>
#include <vector>
import Rhodo.Logger;

int main() {
    Rhodo::Logger::coreLogger coreLogger;
    Rhodo::Logger::appLogger appLogger;

    Rhodo::Logger::coreLogger::info("dasd");

    coreLogger.info("Hello World!");

    int a = 5;
    std::array v{1,2,3};
    std::vector vv{1,2,3};
    appLogger.info("int a {}",a);
    appLogger.info("array v {}",v);
    appLogger.info("vec vv {}",vv);

    // Rhodo::Logger::flush();
    return 0;
}
