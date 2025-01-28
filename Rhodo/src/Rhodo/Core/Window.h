// Rhodo/src/Rhodo/Core/Window.h
#pragma once

#include <utility>

#include "rhpch.h"
#include "Rhodo/Events/Event.h"

namespace Rhodo {
    struct WindowProps {
        std::string title;
        uint32_t width;
        uint32_t height;

        WindowProps() : title("Rhodo Engine"), width(1440), height(900) {
        }

        explicit WindowProps(std::string title,
                             const uint32_t width = 1440,
                             const uint32_t height = 900)
            : title(std::move(title)), width(width), height(height) {
        }
    };

    class RHODO_API Window {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        [[nodiscard]] virtual uint32_t getWidth() const = 0;

        [[nodiscard]] virtual uint32_t getHeight() const = 0;

        // Window attributes
        virtual void setEventCallback(const EventCallbackFn &callback) = 0;

        virtual void setVSync(bool enabled) = 0;

        [[nodiscard]] virtual bool isVSync() const = 0;

        [[nodiscard]] virtual void *getNativeWindow() const = 0;

        static scope<Window> create(const WindowProps &props = WindowProps());
    };
} // Rhodo
