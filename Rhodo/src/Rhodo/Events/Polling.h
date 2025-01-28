
#pragma once

namespace Rhodo {
    class PollingHandler {
    public:
        virtual ~PollingHandler();

        virtual void poll() = 0; // Poll events (input, application, etc.)
    };
} // Rhodo


