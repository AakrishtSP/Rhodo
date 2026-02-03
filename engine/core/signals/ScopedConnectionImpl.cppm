export module Rhodo.Core.Signals:ScopedConnectionImpl;

import :ScopedConnection;

namespace rhodo {
template <typename... Args>
ScopedConnection<Args...>::ScopedConnection() noexcept = default;

template <typename... Args>
ScopedConnection<Args...>::ScopedConnection(Signal<Args...>& signal,
                                            const SlotId slot_id) noexcept
    : signal_(&signal), id_(slot_id) {}

template <typename... Args>
ScopedConnection<Args...>::~ScopedConnection() noexcept {
  disconnect();
}

template <typename... Args>
ScopedConnection<Args...>::ScopedConnection(ScopedConnection&& other) noexcept
    : signal_(other.signal_), id_(other.id_) {
  other.signal_ = nullptr;
  other.id_ = 0;
}

template <typename... Args>
auto ScopedConnection<Args...>::operator=(ScopedConnection&& other) noexcept
    -> ScopedConnection& {
  if (this != &other) {
    disconnect();
    signal_ = other.signal_;
    id_ = other.id_;
    other.signal_ = nullptr;
    other.id_ = 0;
  }
  return *this;
}

template <typename... Args>
auto ScopedConnection<Args...>::disconnect() noexcept -> void {
  if (signal_) {
    signal_->disconnect(id_);
    signal_ = nullptr;
    id_ = 0;
  }
}

template <typename... Args>
auto ScopedConnection<Args...>::connected() const noexcept -> bool {
  return signal_ != nullptr;
}

template <typename... Args>
ScopedConnection<Args...>::operator bool() const noexcept {
  return connected();
}
}  // namespace rhodo
