export module Rhodo.Signals:ScopedConnection;

import :Signal;

export namespace rhodo {
template <typename... Args>
class ScopedConnection {
 public:
  ScopedConnection() noexcept;
  ScopedConnection(Signal<Args...>& signal, SlotId slot_id) noexcept;
  ~ScopedConnection() noexcept;

  // Move-only semantics
  ScopedConnection(ScopedConnection&& other) noexcept;
  auto operator=(ScopedConnection&& other) noexcept -> ScopedConnection&;
  ScopedConnection(const ScopedConnection&)                    = delete;
  auto operator=(const ScopedConnection&) -> ScopedConnection& = delete;

  auto                   Disconnect() noexcept -> void;
  [[nodiscard]] auto     Connected() const noexcept -> bool;
  [[nodiscard]] explicit operator bool() const noexcept;

 private:
  Signal<Args...>* signal_ = nullptr;
  SlotId           id_     = 0;
};
}   // namespace rhodo
