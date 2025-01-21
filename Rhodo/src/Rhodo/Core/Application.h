#pragma once
#include "Rhodo/Core/Core.h"

namespace Rhodo
{
	class RHODO_API Application
	{
	public:
		Application();
		virtual ~Application();
		static void Run();
	};
	// To be defined in CLIENT
	Application* CreateApplication();
}