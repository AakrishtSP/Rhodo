#pragma once
#include "Rhodo/Core/Core.h"

namespace Rhodo {
	class RHODO_API Application {
	public:
		Application();

		virtual ~Application();

		static void run();
	};

	// To be defined in CLIENT
	Application *createApplication();
}
