#pragma once

#include "Rhodo/Core/Core.h"
#include "Rhodo/Core/Application.h"
#include "Rhodo/Core/Logger.h"

int main(int argc, char **argv) {
	Rhodo::Logger::init();
	RH_CORE_INFO("Initialized Logger");
	RH_TRACE("Initialized Logger");

	Rhodo::Application *app = Rhodo::createApplication();
	app->run();
	delete app;
}
