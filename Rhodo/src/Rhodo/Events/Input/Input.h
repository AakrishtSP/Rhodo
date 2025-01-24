//
// Created by asp on 1/24/2025.
//

#pragma once
#include "glm/glm.hpp"

namespace Rhodo {

    class InputManager {
    public:
        // Polling methods for keyboard input
        static bool isKeyPressed(int keycode);

        // Polling methods for mouse input
        static bool isMouseButtonPressed(int button);
        static glm::vec2 getMousePosition();
        static float getMouseX();
        static float getMouseY();

        // Polling methods for gamepad input
        static bool isGamepadButtonPressed(int button);
        static float getGamepadAxis(int axis);
        static float getGamepadTrigger(int trigger);

        // Add more polling methods as needed
    };

} // namespace Rhodo