#include <array>
#include <vector>
import Rhodo.Logger;

int main() {
    Rhodo::Logger::init();
    Rhodo::Logger::info("Hello World!");

    int a = 5;
    std::array v{1,2,3};
    std::vector vv{1,2,3};
    Rhodo::Logger::info_core("int a {}",a);
    Rhodo::Logger::info_core("array v {}",v);
    Rhodo::Logger::info_core("vec vv {}",vv);

    Rhodo::Logger::flush();
    return 0;
}
