// Rhodo/include/Rhodo/Signals/ScopedConnection.h
#pragma once
#include "Rhodo/Signals/Signal.h"
namespace Rhodo
{
    // ============================================================================
    // ScopedConnection - RAII wrapper for automatic disconnection
    // ============================================================================
    template<typename... Args>
    class ScopedConnection {
        Signal<Args...>* signal_ = nullptr;
        typename Signal<Args...>::SlotID id_ = 0;

    public:
        ScopedConnection() noexcept = default;

        ScopedConnection(Signal<Args...>& signal, typename Signal<Args...>::SlotID id) noexcept
            : signal_(&signal), id_(id) {}

        ~ScopedConnection() noexcept {
            disconnect();
        }

        // Move-only semantics
        ScopedConnection(ScopedConnection&& other) noexcept
            : signal_(other.signal_), id_(other.id_) {
            other.signal_ = nullptr;
            other.id_ = 0;
        }

        ScopedConnection& operator=(ScopedConnection&& other) noexcept {
            if (this != &other) {
                disconnect();
                signal_ = other.signal_;
                id_ = other.id_;
                other.signal_ = nullptr;
                other.id_ = 0;
            }
            return *this;
        }

        ScopedConnection(const ScopedConnection&) = delete;
        ScopedConnection& operator=(const ScopedConnection&) = delete;

        void disconnect() noexcept {
            if (signal_) {
                signal_->disconnect(id_);
                signal_ = nullptr;
                id_ = 0;
            }
        }

        [[nodiscard]] bool connected() const noexcept {
            return signal_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return connected();
        }
    };


}