#include "Application.h"


namespace Rhodo {
	Application::Application(): m_running(true) {
		const auto props = WindowProps("Rhodo Engine", 1440, 900);
		m_window = Window::create(props);
	}

	Application::~Application()
	= default;

	void Application::run() const {
		while (m_running) {
			m_window->onUpdate();
		}
	}
}
