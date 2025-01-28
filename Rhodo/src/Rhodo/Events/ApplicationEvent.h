// Rhodo/src/Rhodo/Events/ApplicationEvent.h

#pragma once

#include "Rhodo/Events/Event.h"
#include "rhpch.h"

namespace Rhodo {
    class WindowResizeEvent final : public Event {
    public:
        WindowResizeEvent(const unsigned int width, const unsigned int height)
            : m_width(width), m_height(height) {
        }

        [[nodiscard]] unsigned int getWidth() const { return m_width; }
        [[nodiscard]] unsigned int getHeight() const { return m_height; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_width << ", " << m_height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))

    private:
        unsigned int m_width, m_height;
    };

    class WindowCloseEvent final : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
    };

    class AppTickEvent final : public Event {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
    };

    class AppUpdateEvent final : public Event {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
    };

    class AppRenderEvent final : public Event {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
    };
}
