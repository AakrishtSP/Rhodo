#include "Window.h"

#ifdef RH_PLATFORM_WINDOWS || defined(RH_PLATFORM_LINUX)
#include "PlatformSpecific/GLFW/GlfwWindow.h"
#endif

namespace Rhodo {
    scope<Window> Window::create(const WindowProps &props) {
#if defined(RH_PLATFORM_WINDOWS) || defined(RH_PLATFORM_LINUX)
        return createScope<GlfwWindow>(props);
#else
        RH_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }
} // Rhodo
