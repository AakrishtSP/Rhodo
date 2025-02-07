#pragma once

#include "Rhodo/Events/Event.h"
#include "rhpch.h"
#include "Rhodo/Events/Input/KeyCodes.h"


namespace Rhodo {
    class KeyboardEvent : public Event {
    public:
        [[nodiscard]] KeyCode getKeyCode() const { return m_keyCode; }
        EVENT_CLASS_CATEGORY(
            EventCategory::EventCategoryKeyboard | EventCategory::EventCategoryInput)

    protected:
        explicit KeyboardEvent(const KeyCode keyCode) : m_keyCode(keyCode) {
        }

        KeyCode m_keyCode;
    };

    class KeyPressedEvent final : public KeyboardEvent {
    public:
        explicit KeyPressedEvent(const KeyCode keyCode, const bool isRepeat = false)
            : KeyboardEvent(keyCode), m_repeat(isRepeat) {
        }

        [[nodiscard]] bool getRepeat() const { return m_repeat; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeat << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        bool m_repeat;
    };


    class KeyReleasedEvent final : public KeyboardEvent {
    public:
        explicit KeyReleasedEvent(const KeyCode keycode)
            : KeyboardEvent(keycode) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_keyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent final : public KeyboardEvent {
    public:
        explicit KeyTypedEvent(const KeyCode keycode)
            : KeyboardEvent(keycode) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_keyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
