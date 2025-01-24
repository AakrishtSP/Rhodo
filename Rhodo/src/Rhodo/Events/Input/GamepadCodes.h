// Rhodo/src/Rhodo/Events/Input/GamepadCodes.h
#pragma once
#include "rhpch.h"

namespace Rhodo {
    using GamepadButtonCode = uint16_t;

    namespace Gamepad {
        enum : GamepadButtonCode {
            // From glfw3.h
            ButtonA = 0,
            ButtonB = 1,
            ButtonX = 2,
            ButtonY = 3,
            ButtonLeftBumper = 4,
            ButtonRightBumper = 5,
            ButtonBack = 6,
            ButtonStart = 7,
            ButtonGuide = 8,
            ButtonLeftThumb = 9,
            ButtonRightThumb = 10,
            ButtonDpadUp = 11,
            ButtonDpadRight = 12,
            ButtonDpadDown = 13,
            ButtonDpadLeft = 14,
            TriggerLeft = 15,
            TriggerRight = 16,

            // Aliases
            ButtonCross = ButtonA,
            ButtonCircle = ButtonB,
            ButtonSquare = ButtonX,
            ButtonTriangle = ButtonY,
            ButtonL1 = ButtonLeftBumper,
            ButtonR1 = ButtonRightBumper,
            ButtonSelect = ButtonBack,
            ButtonHome = ButtonGuide,
            ButtonL3 = ButtonLeftThumb,
            ButtonR3 = ButtonRightThumb,
            ButtonUp = ButtonDpadUp,
            ButtonRight = ButtonDpadRight,
            ButtonDown = ButtonDpadDown,
            ButtonLeft = ButtonDpadLeft,
            TriggerL2 = TriggerLeft,
            TriggerR2 = TriggerRight
        };
    }
}
