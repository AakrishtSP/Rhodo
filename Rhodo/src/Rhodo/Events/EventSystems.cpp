
#include "Rhodo/Events/EventSystems.h"

namespace Rhodo {
    void BlockingEventSystem::subscribe(EventType type, const std::function<void(const Event &)> &listener) {
        m_listeners[type].push_back(listener);
    }

    void BlockingEventSystem::dispatch(const Event &event) {
        if (const auto it = m_listeners.find(event.getEventType()); it != m_listeners.end()) {
            for (const auto &listener: it->second) {
                listener(event); // Call listeners directly
            }
        }
    }


    void QueuedEventSystem::subscribe(EventType type, const std::function<void(const Event &)> &listener) {
        m_listeners[type].push_back(listener);
    }


    void QueuedEventSystem::queueEvent(const Event &event) {
        m_eventQueue.push(&event);
    }

    void QueuedEventSystem::processEvents() {
        while (!m_eventQueue.empty()) {
            const Event *event = m_eventQueue.front();
            m_eventQueue.pop();

            if (auto it = m_listeners.find(event->getEventType()); it != m_listeners.end()) {
                for (const auto &listener: it->second) {
                    listener(*event); // Process queued events
                }
            }
        }
    }

    void HybridEventSystem::subscribe(EventType type, const std::function<void(const Event &)> &listener) {
        m_listeners[type].push_back(listener);
    }

    void HybridEventSystem::dispatch(const Event &event) {
        if (const auto it = m_listeners.find(event.getEventType()); it != m_listeners.end()) {
            for (const auto &listener: it->second) {
                listener(event); // Call m_listeners directly
            }
        }
    }

    void HybridEventSystem::queueEvent(const Event &event) {
        m_eventQueue.push(&event);
    }

    void HybridEventSystem::processEvents() {
        while (!m_eventQueue.empty()) {
            const Event *event = m_eventQueue.front();
            m_eventQueue.pop();

            if (auto it = m_listeners.find(event->getEventType()); it != m_listeners.end()) {
                for (const auto &listener: it->second) {
                    listener(*event); // Process queued events
                }
            }
        }
    }

    // No-op for queue-related methods
    void BlockingEventSystem::queueEvent(const Event &event) {
    }

    void BlockingEventSystem::processEvents() {
    }

    // No-op for immediate dispatch
    void QueuedEventSystem::dispatch(const Event &event) {
    }
}



