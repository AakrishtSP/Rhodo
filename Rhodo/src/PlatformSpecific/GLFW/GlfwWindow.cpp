// Rhodo/src/PlatformSpecific/OS/Windows/GlfwWindow.cpp

#include "GlfwWindow.h"
#include "rhpch.h"

#include "Rhodo/Events/ApplicationEvent.h"
#include "Rhodo/Events/KeyboardEvent.h"
#include "Rhodo/Events/MouseEvent.h"

namespace Rhodo {
    static uint8_t s_GLFWWindowCount = 0;

    static void GLFWErrorCallback(int error, const char *description) {
        RH_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    GlfwWindow::GlfwWindow(const WindowProps &props): m_data(props) {
        GlfwWindow::init(props);
    }

    GlfwWindow::~GlfwWindow() {
        GlfwWindow::shutdown();
    }

    void GlfwWindow::onUpdate() {
    }

    unsigned int GlfwWindow::getWidth() const { return m_data.width; }

    unsigned int GlfwWindow::getHeight() const { return m_data.height; }

    void GlfwWindow::setEventCallback(const EventCallbackFn &callback) { m_data.eventCallback = callback; }

    void *GlfwWindow::getNativeWindow() const { return m_window; }

    void GlfwWindow::setVSync(const bool enabled) {
        glfwSwapInterval(enabled); // Enable vsync
    }

    bool GlfwWindow::isVSync() const {
        return m_data.VSync;
    }

    void GlfwWindow::init(const WindowProps &props) {
        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;

        RH_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (s_GLFWWindowCount == 0) {
            if (const int success = glfwInit(); !success) {
                RH_CORE_ERROR("Could not initialize GLFW!");
                RH_CORE_ASSERT(false, "Could not initialize GLFW!");
            }
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        m_window = glfwCreateWindow(static_cast<int>(m_data.width), static_cast<int>(m_data.height),
                                    m_data.title.c_str(), nullptr, nullptr);
        ++s_GLFWWindowCount;

        if (!m_window) {
            RH_CORE_ERROR("Failed to create GLFW window!");
            glfwTerminate();
            RH_CORE_ASSERT(false, "Failed to create GLFW window!");
        }

        // Make the window's context current
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, &m_data);
        setVSync(m_data.VSync); // Enable vsync

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, const int width, const int height) {
            WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            data.width = width;
            data.height = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            data.eventCallback(event);
        });
        glfwSetKeyCallback(m_window, [](GLFWwindow *window, const int key, const int scancode, const int action,
                                        const int mods) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, false);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, true);
                    data.eventCallback(event);
                    break;
                }
                default:
                    break;
            }
        });
        glfwSetCharCallback(m_window, [](GLFWwindow *window, const unsigned int keycode) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            KeyTypedEvent event(keycode);
            data.eventCallback(event);
        });

        glfwSetMouseButtonCallback(
            m_window, [](GLFWwindow *window, const int button, const int action, const int mods) {
                const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
                switch (action) {
                    case GLFW_PRESS: {
                        MouseButtonPressedEvent event(button);
                        data.eventCallback(event);
                        break;
                    }
                    case GLFW_RELEASE: {
                        MouseButtonReleasedEvent event(button);
                        data.eventCallback(event);
                        break;
                    }
                    default:
                        break;
                }
            });
        glfwSetScrollCallback(m_window, [](GLFWwindow *window, const double xoffset, const double yoffset) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            MouseScrolledEvent event(static_cast<float>(xoffset), static_cast<float>(yoffset));
            data.eventCallback(event);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, const double xpos, const double ypos) {
            const WindowData &data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
            MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
            data.eventCallback(event);
        });
    }

    void GlfwWindow::shutdown() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}
