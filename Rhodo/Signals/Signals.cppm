module;
#include <functional>
#include <string>

export module Rhodo.Signals;

export import :Signal;
export import :SignalHub;
export import :ScopedConnection;

export namespace Rhodo::Signals
{
    constexpr auto global() noexcept -> SignalHub & {
        static SignalHub hub;
        return hub;
    }

    template <typename... Args>
    auto get(const std::string &name) -> Signal<Args...> & {
        return global().get<Args...>(name);
    }

    template <typename... Args>
    [[nodiscard]] bool has(const std::string &name) noexcept {
        return global().has<Args...>(name);
    }

    template <typename... Args>
    void remove(const std::string &name) {
        global().remove<Args...>(name);
    }

    inline void clear() noexcept {
        global().clear();
    }

    inline void cleanupEmpty() {
        global().cleanupEmptySignals();
    }

    template <typename... Args>
    [[nodiscard]] auto makeScopedConnection(
            Signal<Args...> &signal,
            std::function<void(Args...)> callback) -> ScopedConnection<Args...> {
        auto id = signal.connect(std::move(callback));
        return ScopedConnection<Args...>(signal, id);
    }

    template <typename... Args, typename T>
    [[nodiscard]] auto makeScopedConnection(
            Signal<Args...> &signal,
            T &obj,
            void (T::*method)(Args...)) -> ScopedConnection<Args...> {
        auto id = signal.connect(obj, method);
        return ScopedConnection<Args...>(signal, id);
    }
}
