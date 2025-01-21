#include <Rhodo.h>

class Example : public Rhodo::Application
{
public:
	Example()
	{
	}
	~Example()
	{
	}
};

Rhodo::Application* Rhodo::CreateApplication()
{
	return new Example();
}