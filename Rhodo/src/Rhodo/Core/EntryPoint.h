#pragma once

#include "Rhodo/Core/Core.h"
#include "Rhodo/Core/Application.h"
#include "Rhodo/Core/Logger.h"

#if defined RH_PLATFORM_WINDOWS

extern Rhodo::Application* Rhodo::CreateApplication();

int main(int argc, char** argv)
{
	Rhodo::Logger::Init();
	RH_CORE_INFO("Initialized Logger");
	RH_TRACE("Initialized Logger");

	Rhodo::Application* app = Rhodo::CreateApplication();
	app->Run();
	delete app;
}

#endif