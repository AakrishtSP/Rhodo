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
constexpr auto global() noexcept -> SignalHub& {
  static SignalHub hub;
  return hub;
}

template <typename... Args>
auto get(const std::string& name) -> Signal<Args...>& {
  return global().get<Args...>(name);
}

template <typename... Args>
[[nodiscard]] auto has(const std::string& name) noexcept -> bool {
  return global().has<Args...>(name);
}

template <typename... Args>
auto remove(const std::string& name) -> void {
  global().remove<Args...>(name);
}

inline auto clear() noexcept -> void {
  global().clear();
}

inline auto cleanup_empty() -> void {
  global().cleanup_empty_signals();
}

template <typename... Args>
[[nodiscard]] auto make_scoped_connection(Signal<Args...>& signal,
                                          std::function<void(Args...)> callback)
    -> ScopedConnection<Args...> {
  auto signal_id = signal.Connect(std::move(callback));
  return ScopedConnection<Args...>(signal, signal_id);
}

template <typename... Args, typename T>
[[nodiscard]] auto make_scoped_connection(Signal<Args...>& signal, T& obj,
                                          void (T::*method)(Args...))
    -> ScopedConnection<Args...> {
  auto signal_id = signal.Connect(obj, method);
  return ScopedConnection<Args...>(signal, signal_id);
}
}  // namespace rhodo::core::signals
