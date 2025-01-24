// Rhodo/src/Rhodo/Events/Input/Input.h

#pragma once
#include "glm/glm.hpp"
#include "Rhodo/Events/Input/KeyCodes.h"
#include "Rhodo/Events/Input/MouseCodes.h"
#include "Rhodo/Events/Input/GamepadCodes.h"


namespace Rhodo {
    class Input {
    public:
        // Polling methods for keyboard input
        static bool isKeyPressed(KeyCode keycode);

        // Polling methods for mouse input
        static bool isMouseButtonPressed(MouseCode button);

        static glm::vec2 getMousePosition();

        static float getMouseX();

        static float getMouseY();

        // Polling methods for gamepad input
        static bool isGamepadButtonPressed(GamepadCode button);

        static float getGamepadAxis(GamepadCode axis);

        static float getGamepadTrigger(GamepadCode trigger);
    };
} // namespace Rhodo
