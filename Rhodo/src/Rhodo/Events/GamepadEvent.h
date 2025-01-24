#pragma once

#include "Rhodo/Events/Event.h"
#include "rhpch.h"
#include "Rhodo/Events/Input/GamepadCodes.h"

namespace Rhodo {
    class GamepadEvent : public Event {
    public:
        explicit GamepadEvent(uint32_t gamepadID)
            : m_gamepadID(gamepadID) {
        }

        [[nodiscard]] uint32_t getGamepadID() const { return m_gamepadID; }

        // Static methods to manage the connected gamepad count
        static void incrementConnectedCount() { ++s_connectedGamepadCount; }
        static void decrementConnectedCount() { --s_connectedGamepadCount; }
        static uint32_t getConnectedCount() { return s_connectedGamepadCount; }

        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGamepad | EventCategory::EventCategoryInput)

    protected:
        uint32_t m_gamepadID; // Identifier for the gamepad

    private:
        static inline uint32_t s_connectedGamepadCount = 0; // Static counter for connected gamepads
    };

    class GamepadButtonEvent : public GamepadEvent {
    public:
        [[nodiscard]] GamepadButtonCode getButtonCode() const { return m_buttonCode; }

    protected:
        GamepadButtonEvent(uint32_t gamepadID, GamepadButtonCode buttonCode)
            : GamepadEvent(gamepadID), m_buttonCode(buttonCode) {
        }

        GamepadButtonCode m_buttonCode;
    };

    class GamepadButtonPressedEvent final : public GamepadButtonEvent {
    public:
        GamepadButtonPressedEvent(uint32_t gamepadID, GamepadButtonCode buttonCode)
            : GamepadButtonEvent(gamepadID, buttonCode) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadButtonPressedEvent: Gamepad " << m_gamepadID << ", Button " << m_buttonCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadButtonPressed)
    };

    class GamepadButtonReleasedEvent final : public GamepadButtonEvent {
    public:
        GamepadButtonReleasedEvent(uint32_t gamepadID, GamepadButtonCode buttonCode)
            : GamepadButtonEvent(gamepadID, buttonCode) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadButtonReleasedEvent: Gamepad " << m_gamepadID << ", Button " << m_buttonCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadButtonReleased)
    };

    class GamepadAxisMovedEvent final : public GamepadEvent {
    public:
        GamepadAxisMovedEvent(uint32_t gamepadID, int axis, float position)
            : GamepadEvent(gamepadID), m_axis(axis), m_position(position) {
        }

        [[nodiscard]] int getAxis() const { return m_axis; }
        [[nodiscard]] float getPosition() const { return m_position; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadAxisMovedEvent: Gamepad " << m_gamepadID << ", Axis " << m_axis
                    << ", Position " << m_position;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadAxisMoved)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGamepad | EventCategory::EventCategoryGamepadAxis)

    private:
        int m_axis;
        float m_position;
    };

    class GamepadTriggerMovedEvent final : public GamepadEvent {
    public:
        GamepadTriggerMovedEvent(uint32_t gamepadID, int trigger, float pressure)
            : GamepadEvent(gamepadID), m_trigger(trigger), m_pressure(pressure) {
        }

        [[nodiscard]] int getTrigger() const { return m_trigger; }
        [[nodiscard]] float getPressure() const { return m_pressure; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadTriggerMovedEvent: Gamepad " << m_gamepadID << ", Trigger " << m_trigger
                    << ", Pressure " << m_pressure;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadTriggerMoved)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGamepad | EventCategory::EventCategoryGamepadTrigger)

    private:
        int m_trigger;
        float m_pressure;
    };

    class GamepadConnectedEvent final : public GamepadEvent {
    public:
        explicit GamepadConnectedEvent(uint32_t gamepadID)
            : GamepadEvent(gamepadID) {
            incrementConnectedCount(); // Increment the count when a gamepad is connected
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadConnectedEvent: Gamepad " << m_gamepadID;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadConnected)
    };

    class GamepadDisconnectedEvent final : public GamepadEvent {
    public:
        explicit GamepadDisconnectedEvent(uint32_t gamepadID)
            : GamepadEvent(gamepadID) {
            decrementConnectedCount(); // Decrement the count when a gamepad is disconnected
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "GamepadDisconnectedEvent: Gamepad " << m_gamepadID;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::GamepadDisconnected)
    };
}
