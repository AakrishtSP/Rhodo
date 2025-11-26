module;
#include <string>
#include <functional>

export module Rhodo.Signals;

export import Rhodo.Signals.Signal;
export import Rhodo.Signals.SignalHub;
export import Rhodo.Signals.ScopedConnection;


export namespace Rhodo::Signals
{
    inline SignalHub& global() noexcept
    {
        static SignalHub hub;
        return hub;
    }

    template <typename... Args>
    Signal<Args...>& get(const std::string& name)
    {
        return global().get<Args...>(name);
    }

    template <typename... Args>
    [[nodiscard]] bool has(const std::string& name) noexcept
    {
        return global().has<Args...>(name);
    }

    template <typename... Args>
    void remove(const std::string& name)
    {
        global().remove<Args...>(name);
    }

    inline void clear() noexcept
    {
        global().clear();
    }

    inline void cleanup_empty()
    {
        global().cleanup_empty_signals();
    }

    template <typename... Args>
    [[nodiscard]] ScopedConnection<Args...> make_scoped_connection(
        Signal<Args...>& signal,
        std::function<void(Args...)> callback)
    {
        auto id = signal.connect(std::move(callback));
        return ScopedConnection<Args...>(signal, id);
    }

    template <typename... Args, typename T>
    [[nodiscard]] ScopedConnection<Args...> make_scoped_connection(
        Signal<Args...>& signal,
        T& obj,
        void (T::*method)(Args...))
    {
        auto id = signal.connect(obj, method);
        return ScopedConnection<Args...>(signal, id);
    }
}
