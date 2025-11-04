#pragma once

#include "Rhodo/Core/Core.h"
#include "Rhodo/Core/Application.h"
#include "Rhodo/Core/Logger.h"

int main(int argc, char **argv) {
	Rhodo::Logger::init();
	RH_CORE_INFO("Initialized Logger");
	RH_INFO("Initialized Logger");

#ifdef RH_PLATFORM_WINDOWS
	RH_CORE_INFO("Running on Windows");
#elif defined(RH_PLATFORM_LINUX)
	RH_CORE_INFO("Running on Linux");
#elif defined(RH_PLATFORM_APPLE)
	RH_CORE_INFO("Running on Apple");
#elif defined(RH_PLATFORM_ANDROID)
	RH_CORE_INFO("Running on Android");
#else
	RH_CORE_INFO("Running on an unknown platform");
#endif

	const Rhodo::Application *app = Rhodo::createApplication();
	app->run();
	delete app;
}
