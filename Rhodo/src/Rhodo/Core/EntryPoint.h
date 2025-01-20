#pragma once

#include "Rhodo/Core/Core.h"
#include "Rhodo/Core/Application.h"

#ifdef RH_PLATFORM_WINDOWS

extern Rhodo::Application* Rhodo::CreateApplication();

int main(int argc, char** argv)
{
	Rhodo::Application* app = Rhodo::CreateApplication();
	app->Run();
	delete app;
}

#endif