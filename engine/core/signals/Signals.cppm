module;
#include <functional>
#include <string>
#include <utility>

export module Rhodo.Core.Signals;

export import :Signal;
export import :SignalHub;
export import :ScopedConnection;
export import :Hooks;
export namespace rhodo::core::signals {
constexpr auto Global() noexcept -> SignalHub& {
  static SignalHub hub;
  return hub;
}

template <typename... Args>
auto Get(const std::string& name) -> Signal<Args...>& {
  return Global().Get<Args...>(name);
}

template <typename... Args>
[[nodiscard]] auto Has(const std::string& name) noexcept -> bool {
  return Global().Has<Args...>(name);
}

template <typename... Args>
auto Remove(const std::string& name) -> void {
  Global().Remove<Args...>(name);
}

inline auto Clear() noexcept -> void {
  Global().Clear();
}

inline auto CleanupEmpty() -> void {
  Global().CleanupEmptySignals();
}

template <typename... Args>
[[nodiscard]] auto MakeScopedConnection(Signal<Args...>& signal,
                                        std::function<void(Args...)> callback)
    -> ScopedConnection<Args...> {
  auto signal_id = signal.Connect(std::move(callback));
  return ScopedConnection<Args...>(signal, signal_id);
}

template <typename... Args, typename T>
[[nodiscard]] auto MakeScopedConnection(Signal<Args...>& signal, T& obj,
                                        void (T::*method)(Args...))
    -> ScopedConnection<Args...> {
  auto signal_id = signal.Connect(obj, method);
  return ScopedConnection<Args...>(signal, signal_id);
}
}  // namespace rhodo::core::signals
