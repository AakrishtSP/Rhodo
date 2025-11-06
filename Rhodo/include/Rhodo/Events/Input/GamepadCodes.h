// Rhodo/src/Rhodo/Events/Input/GamepadCodes.h
#pragma once

namespace Rhodo {
    using GamepadIDCode = uint8_t;
    using GamepadButtonCode = uint16_t;
    using GamepadAxisCode = uint16_t;

    namespace Gamepad {
        // IDs
        enum class GamepadID :  GamepadIDCode   {
            Controller1 = 0,
            Controller2 = 1,
            Controller3 = 2,
            Controller4 = 3,
            MaxSupported = 4
        };

        // Buttons (from glfw3.h)
        enum class Button : GamepadButtonCode {
            ButtonA = 0,
            ButtonB = 1,
            ButtonX = 2,
            ButtonY = 3,
            LeftBumper = 4,
            RightBumper = 5,
            Back = 6,
            Start = 7,
            Guide = 8,
            LeftThumb = 9,
            RightThumb = 10,
            DpadUp = 11,
            DpadRight = 12,
            DpadDown = 13,
            DpadLeft = 14,

            // Platform aliases
            Cross = ButtonA,
            Circle = ButtonB,
            Square = ButtonX,
            Triangle = ButtonY,
            Share = Back,
            Options = Start
        };

        // Axes (from glfw3.h)
        enum class Axis : GamepadAxisCode {
            LeftX = 0,
            LeftY = 1,
            RightX = 2,
            RightY = 3,
            LeftTrigger = 4,
            RightTrigger = 5
        };
    }
}
