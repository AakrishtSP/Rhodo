module;
#include <functional>
#include <string>
#include <utility>

export module Rhodo.Signals;

export import :Signal;
export import :SignalHub;
export import :ScopedConnection;
export namespace rhodo::signals {
constexpr auto global() noexcept -> SignalHub& {
  static SignalHub hub;
  return hub;
}

template <typename... Args>
auto get(const std::string& name) -> Signal<Args...>& {
  return global().Get<Args...>(name);
}

template <typename... Args>
[[nodiscard]] bool has(const std::string& name) noexcept {
  return global().Has<Args...>(name);
}

template <typename... Args>
void remove(const std::string& name) {
  global().Remove<Args...>(name);
}

inline void clear() noexcept {
  global().Clear();
}

inline void cleanupEmpty() {
  global().CleanupEmptySignals();
}

template <typename... Args>
[[nodiscard]] auto makeScopedConnection(Signal<Args...>&             signal,
                                        std::function<void(Args...)> callback)
    -> ScopedConnection<Args...> {
  auto id = signal.Connect(std::move(callback));
  return ScopedConnection<Args...>(signal, id);
}

template <typename... Args, typename T>
[[nodiscard]] auto makeScopedConnection(Signal<Args...>& signal, T& obj, void (T::*method)(Args...))
    -> ScopedConnection<Args...> {
  auto id = signal.Connect(obj, method);
  return ScopedConnection<Args...>(signal, id);
}
}   // namespace rhodo::signals
