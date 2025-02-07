// Rhodo/src/Rhodo/Events/EventSystems.h

#pragma once

#include "Rhodo/Events/Event.h"
#include "rhpch.h"

namespace Rhodo {
    // Event System
    class EventSystem {
    public:
        using Listener = std::function<void(const Event &)>;

        struct SubscriptionToken {
            size_t eventType; // hash of the event type
            size_t listenerId;
        };

        SubscriptionToken Subscribe(size_t eventType, Listener listener);

        void Unsubscribe(SubscriptionToken token);

        void DispatchImmediately(const Event &event);

        void QueueEvent(scope<Event> event);

        void ProcessQueue();

    private:
        // m_listener = eventType -> listenerId, listener
        std::unordered_map<size_t, std::vector<std::pair<size_t, Listener> > > m_listeners;
        std::queue<scope<Event> > m_eventQueue;
        size_t m_nextId = 0;
    };
}


