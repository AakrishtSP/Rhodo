module;
#include <atomic>
#include <cstdint>
#include <deque>
#include <functional>
#include <shared_mutex>

export module Rhodo.Signals:Signal;

export namespace rhodo {
using SlotId = uint64_t;

template <typename... Args>
class Signal {
 public:
  static constexpr uint32_t kCleanupThreshold = 16;
  auto NextId() const -> uint64_t;

  // Connect - WRITE operation
  template <typename T>
  auto Connect(T& obj, void (T::*method)(Args...)) -> SlotId;
  auto Connect(std::function<void(Args...)> callback) -> SlotId;

  // Disconnect - WRITE
  auto Disconnect(SlotId slot_id) -> void;
  auto DisconnectAll() noexcept -> void;

  // Emit signal - READ operation
  template <typename... A>
  auto Emit(A&&... args) -> void;
  template <typename... A>
  auto BlockingEmit(A&&... args) -> void;

  // Operator() as alias for emit
  template <typename... A>
  auto operator()(A&&... args) -> void;

  // Query
  [[nodiscard]] auto Size() const noexcept -> size_t;  // ACTIVE slots
  [[nodiscard]] auto ContainerSize() const noexcept
      -> size_t;  // slots incl inactive waiting for cleanup
  [[nodiscard]] auto Empty() const noexcept -> bool;

  // Clear all slots - WRITE operation
  auto Clear() noexcept -> void;
  auto ForceCleanup() -> void;

 private:
  auto CleanupInternal() noexcept -> void;

  struct Slot {
    std::function<void(Args...)> callback;
    uint64_t id = 0;
    bool active = true;
  };

  // Deque to avoid iterator invalidation during emission
  std::deque<Slot> slots_;
  std::atomic<uint64_t> next_id_{1};
  std::atomic<uint32_t> disconnect_count_{0};
  std::atomic<bool> needs_cleanup_{false};
  mutable std::shared_mutex mutex_;
};

}  // namespace rhodo
