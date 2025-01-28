// Rhodo/src/PlatformSpecific/OS/Windows/GlfwWindow.h
#pragma once

#include "Rhodo/Core/Window.h"
#include <GLFW/glfw3.h>

namespace Rhodo {
    class GlfwWindow : public Window {
    public:
        explicit GlfwWindow(const WindowProps &props);

        virtual ~GlfwWindow();

        void onUpdate() override;

        [[nodiscard]] unsigned int getWidth() const override;

        [[nodiscard]] unsigned int getHeight() const override;

        // Window attributes
        void setEventCallback(const EventCallbackFn &callback) override;

        void setVSync(bool enabled) override;

        [[nodiscard]] bool isVSync() const override;

        [[nodiscard]] void *getNativeWindow() const override;

    private:
        virtual void init(const WindowProps &props);

        virtual void shutdown();

    private:
        GLFWwindow *m_window{};

        struct WindowData {
            std::string title;
            uint32_t width, height;
            bool VSync;
            EventCallbackFn eventCallback;

            explicit WindowData(const WindowProps &props, const bool Vsync = false) : title(props.title),
                width(props.width), height(props.height), VSync(Vsync) {
            }
        };

        WindowData m_data;
    };
} // namespace Rhodo

