#pragma once

#include "Core.h"

namespace Rhodo
{
	class RHODO_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};
	// To be defined in CLIENT
	Application* CreateApplication();
}

