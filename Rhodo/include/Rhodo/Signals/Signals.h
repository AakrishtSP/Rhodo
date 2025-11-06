// Rhodo/include/Rhodo/Signals/Signals.h
#pragma once
#include "SignalHub.h"
#include "ScopedConnection.h"
#include "Signal.h"

namespace Rhodo
{
    // ============================================================================
    // Global signal access
    // ============================================================================
    namespace Signals {
        inline SignalHub& global() noexcept {
            static SignalHub hub;
            return hub;
        }

        template<typename... Args>
        Signal<Args...>& get(const std::string& name) {
            return global().get<Args...>(name);
        }

        template<typename... Args>
        [[nodiscard]] bool has(const std::string& name) noexcept {
            return global().has<Args...>(name);
        }

        template<typename... Args>
        void remove(const std::string& name) {
            global().remove<Args...>(name);
        }

        inline void clear() noexcept {
            global().clear();
        }

        inline void cleanup_empty() {
            global().cleanup_empty_signals();
        }

        // Helper functions to create scoped connections
        template<typename... Args>
        [[nodiscard]] ScopedConnection<Args...> make_scoped_connection(
            Signal<Args...>& signal,
            std::function<void(Args...)> callback)
        {
            auto id = signal.connect(std::move(callback));
            return ScopedConnection<Args...>(signal, id);
        }

        template<typename... Args, typename T>
        [[nodiscard]] ScopedConnection<Args...> make_scoped_connection(
            Signal<Args...>& signal,
            T& obj,
            void (T::*method)(Args...))
        {
            auto id = signal.connect(obj, method);
            return ScopedConnection<Args...>(signal, id);
        }
    }
}