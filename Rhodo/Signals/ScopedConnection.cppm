export module Rhodo.Signals:ScopedConnection;

import :Signal;

export namespace Rhodo
{
    template <typename... Args>
    class ScopedConnection
    {

    public:
        ScopedConnection() noexcept;
        ScopedConnection(Signal<Args...>& signal, slot_id id) noexcept;
        ~ScopedConnection() noexcept;

        // Move-only semantics
        ScopedConnection(ScopedConnection&& other) noexcept;
        ScopedConnection& operator=(ScopedConnection&& other) noexcept;
        ScopedConnection(const ScopedConnection&) = delete;
        ScopedConnection& operator=(const ScopedConnection&) = delete;


        auto disconnect() noexcept -> void ;
        [[nodiscard]] auto connected() const noexcept -> bool ;
        [[nodiscard]] explicit operator bool() const noexcept;
    private:
        Signal<Args...>* signal_ = nullptr;
        slot_id id_ = 0;
    };

    template<typename ... Args>
    ScopedConnection<Args...>::ScopedConnection() noexcept = default;

    template<typename ... Args>
    ScopedConnection<Args...>::ScopedConnection(Signal<Args...> &signal, const slot_id id) noexcept: signal_(&signal), id_(id) {
    }

    template<typename ... Args>
    ScopedConnection<Args...>::~ScopedConnection() noexcept {
        disconnect();
    }

    template<typename ... Args>
    ScopedConnection<Args...>::ScopedConnection(ScopedConnection &&other) noexcept: signal_(other.signal_), id_(other.id_) {
        other.signal_ = nullptr;
        other.id_ = 0;
    }

    template<typename ... Args>
    ScopedConnection<Args...> & ScopedConnection<Args...>::operator=(ScopedConnection &&other) noexcept {
        if (this != &other)
        {
            disconnect();
            signal_ = other.signal_;
            id_ = other.id_;
            other.signal_ = nullptr;
            other.id_ = 0;
        }
        return *this;
    }

    template<typename ... Args>
    auto ScopedConnection<Args...>::disconnect() noexcept -> void {
        if (signal_)
        {
            signal_->disconnect(id_);
            signal_ = nullptr;
            id_ = 0;
        }
    }

    template<typename ... Args>
    auto ScopedConnection<Args...>::connected() const noexcept -> bool {
        return signal_ != nullptr;
    }

    template<typename ... Args>
    ScopedConnection<Args...>::operator bool() const noexcept {
        return connected();
    }
}
