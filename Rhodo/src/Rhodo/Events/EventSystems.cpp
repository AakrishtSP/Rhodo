
#include "Rhodo/Events/EventSystems.h"

namespace Rhodo {
    EventSystem::SubscriptionToken EventSystem::Subscribe(const size_t eventType, Listener listener) {
        const size_t id = m_nextId++;
        m_listeners[eventType].emplace_back(id, std::move(listener));
        return {eventType, id,};
    }

    void EventSystem::Unsubscribe(SubscriptionToken token) {
        auto &listeners = m_listeners[token.eventType];
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                           [token](const auto &pair) { return pair.first == token.listenerId; }),
            listeners.end());
    }

    void EventSystem::DispatchImmediately(const Event &event) {
        std::vector<std::pair<size_t, Listener> > listeners;
        if (const auto it = m_listeners.find(event.getType()); it != m_listeners.end()) {
            listeners = it->second;
        }

        for (const auto &[id, listener]: listeners) {
            if (listener)
                listener(event);
        }
    }

    void EventSystem::QueueEvent(scope<Event> event) {
        m_eventQueue.push(std::move(event));
    }

    void EventSystem::ProcessQueue() {
        std::queue<scope<Event> > processing_queue; {
            processing_queue.swap(m_eventQueue);
        }

        while (!processing_queue.empty()) {
            auto &event = processing_queue.front();
            DispatchImmediately(*event);
            processing_queue.pop();
        }
    }
}



