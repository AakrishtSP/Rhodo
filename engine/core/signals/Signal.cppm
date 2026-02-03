module;
#include <atomic>
#include <cstdint>
#include <deque>
#include <functional>
#include <shared_mutex>

export module Rhodo.Core.Signals:Signal;

export namespace rhodo {
using SlotId = uint64_t;

template <typename... Args>
class Signal {
 public:
  static constexpr uint32_t k_cleanup_threshold = 16;
  auto next_id() const -> uint64_t;

  // Connect - WRITE operation
  template <typename T>
  auto connect(T& obj, void (T::*method)(Args...)) -> SlotId;
  auto connect(std::function<void(Args...)> callback) -> SlotId;

  // Disconnect - WRITE
  auto disconnect(SlotId slot_id) -> void;
  auto disconnect_all() noexcept -> void;

  // Emit signal - READ operation
  template <typename... A>
  auto emit(A&&... args) -> void;
  template <typename... A>
  auto blocking_emit(A&&... args) -> void;

  // Operator() as alias for emit
  template <typename... A>
  auto operator()(A&&... args) -> void;

  // Query
  [[nodiscard]] auto size() const noexcept -> size_t;  // ACTIVE slots
  [[nodiscard]] auto container_size() const noexcept
      -> size_t;  // slots incl inactive waiting for cleanup
  [[nodiscard]] auto empty() const noexcept -> bool;

  // Clear all slots - WRITE operation
  auto clear() noexcept -> void;
  auto force_cleanup() -> void;

 private:
  auto cleanup_internal() noexcept -> void;

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
