#pragma once
#include "Rhodo/Core/Window.h"

#include "Rhodo/Core/Core.h"

namespace Rhodo {
	class Application {
	public:
		Application();

		virtual ~Application();

		void run() const;

	private:
		scope<Window> m_window;
		bool m_running;
	};

	// To be defined in CLIENT
	Application *createApplication();
}
