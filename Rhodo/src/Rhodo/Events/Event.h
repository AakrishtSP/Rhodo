

#pragma once
#include "Rhodo/Core/Core.h"
#include <string>

namespace Rhodo {
    // Event Types
    enum class EventType {
        None = 0,

        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,

        AppTick, AppUpdate, AppRender,

        KeyPressed, KeyReleased, KeyTyped,

        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,

        GamepadConnected, GamepadDisconnected, GamepadButtonPressed, GamepadButtonReleased, GamepadAxisMoved,
        GamepadTriggerMoved,

        Custom
    };

    // Event Categories
    enum class EventCategory {
        None = 0,
        EventCategoryApplication = RH_BIT_LEFT_SHIFT(0),
        EventCategoryInput = RH_BIT_LEFT_SHIFT(1),
        EventCategoryKeyboard = RH_BIT_LEFT_SHIFT(2),
        EventCategoryMouse = RH_BIT_LEFT_SHIFT(3),
        EventCategoryMouseButton = RH_BIT_LEFT_SHIFT(4),
        EventCategoryGamepad = RH_BIT_LEFT_SHIFT(5),
        EventCategoryGamepadButton = RH_BIT_LEFT_SHIFT(6),
        EventCategoryGamepadAxis = RH_BIT_LEFT_SHIFT(7),
        EventCategoryGamepadTrigger = RH_BIT_LEFT_SHIFT(8),
        EventCategoryCustom = RH_BIT_LEFT_SHIFT(9)
    };

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
                               virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }\
                               virtual size_t getType() const override { return typeid(this).hash_code(); }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategory() const override { return category; }

    inline int operator|(EventCategory lhs, EventCategory rhs) {
        return static_cast<int>(lhs) | static_cast<int>(rhs);
    }

    inline int operator|(const int lhs, EventCategory rhs) {
        return lhs | static_cast<int>(rhs);
    }

    // Base Event Class
    class Event {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType getEventType() const = 0;

        [[nodiscard]] virtual size_t getType() const = 0;

        [[nodiscard]] virtual int getCategory() const = 0;

        [[nodiscard]] virtual const char *getName() const = 0;

        [[nodiscard]] virtual std::string toString() const { return getName(); }

        [[nodiscard]] bool isInCategory(EventCategory category) const {
            return getCategory() & static_cast<int>(category);
        }

        [[nodiscard]] bool isHandled() const { return m_handled; }

    protected:
        bool m_handled = false;
    };

    class CustomEvent : public Event {
    public:
        // Define a pure virtual method that must be implemented by custom events
        virtual void customEventLogic() const = 0;

        EVENT_CLASS_TYPE(Custom)
        EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryCustom))

        [[nodiscard]] std::string toString() const override {
            return "CustomEvent: Base for all custom events";
        }
    };
}
