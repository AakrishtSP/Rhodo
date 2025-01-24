#include <Rhodo.h>

class Examples final : public Rhodo::Application {
public:
	Examples() = default;

	~Examples() override = default;
};

Rhodo::Application *Rhodo::createApplication() {
	RH_INFO("Hello from Rhodo!");
	return new Examples();
}
