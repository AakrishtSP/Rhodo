//
// Created by asp on 1/24/2025.
//

#pragma once

namespace Rhodo {
    class PollingHandler {
    public:
        virtual ~PollingHandler();

        virtual void poll() = 0; // Poll events (input, application, etc.)
    };
} // Rhodo


