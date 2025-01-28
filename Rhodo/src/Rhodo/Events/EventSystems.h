// Rhodo/src/Rhodo/Events/EventSystems.h

#pragma once

#include "Rhodo/Events/Event.h"
#include "rhpch.h"

namespace Rhodo {
    // Event System Interface
    class IEventSystem {
    public:
        virtual ~IEventSystem() = default;

        virtual void subscribe(EventType type, const std::function<void(const Event &)> &listener) = 0;

        virtual void dispatch(const Event &event) = 0;

        virtual void queueEvent(const Event &event) = 0;

        virtual void processEvents() = 0;
    };


    // Blocking Event System
    class BlockingEventSystem final : public IEventSystem {
    public:
        void subscribe(EventType type, const std::function<void(const Event &)> &listener) override;

        void dispatch(const Event &event) override;

        // No-op for queue-related methods
        void queueEvent(const Event &event) override;

        void processEvents() override;

    protected:
        std::unordered_map<EventType, std::vector<std::function<void(const Event &)> > > m_listeners;
    };

    // Queued Event System
    class QueuedEventSystem final : public IEventSystem {
    public:
        void subscribe(EventType type, const std::function<void(const Event &)> &listener) override;

        void dispatch(const Event &event) override;

        void queueEvent(const Event &event) override;

        void processEvents() override;

    protected:
        std::unordered_map<EventType, std::vector<std::function<void(const Event &)> > > m_listeners;
        std::queue<const Event *> m_eventQueue;
    };

    // Hybrid Event System
    class HybridEventSystem final : public IEventSystem {
    public:
        void subscribe(EventType type, const std::function<void(const Event &)> &listener) override;

        void dispatch(const Event &event) override;

        void queueEvent(const Event &event) override;

        void processEvents() override;

    protected:
        std::unordered_map<EventType, std::vector<std::function<void(const Event &)> > > m_listeners;
        std::queue<const Event *> m_eventQueue;
    };
}
