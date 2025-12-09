module;
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <shared_mutex>

export module rhodo.signals:signal;

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
  void forceCleanup();

 private:
  void cleanupInternal() noexcept;

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

template <typename... Args>
auto Signal<Args...>::NextId() const -> uint64_t {
  return next_id_.load();
}

template <typename... Args>
template <typename T>
auto Signal<Args...>::Connect(T& obj, void (T::*method)(Args...)) -> SlotId {
  return Connect([&obj, method](Args&&... args) {
    (obj.*method)(std::forward<Args>(args)...);
  });
}

template <typename... Args>
auto Signal<Args...>::Connect(std::function<void(Args...)> callback) -> SlotId {
  std::unique_lock lock(mutex_);

  SlotId id = next_id_.fetch_add(1, std::memory_order_relaxed);

  // Guard against overflow (wrap to 1, skip 0 as reserved)
  if (id == 0) {
    id = 1;
    next_id_.store(2, std::memory_order_relaxed);
  }

  slots_.push_back({std::move(callback), id, true});
  return id;
}

template <typename... Args>
void Signal<Args...>::Disconnect(SlotId slot_id) {
  std::unique_lock lock(mutex_);

  for (auto& slot : slots_) {
    if (slot.id == slot_id && slot.active) {
      slot.active = false;

      // Trigger cleanup if the threshold reached
      if (const uint32_t count =
              disconnect_count_.fetch_add(1, std::memory_order_relaxed) + 1;
          count >= kCleanupThreshold) {
        needs_cleanup_.store(true, std::memory_order_release);
      } else {
      }
      return;
    }
  }
}

template <typename... Args>
void Signal<Args...>::DisconnectAll() noexcept {
  std::unique_lock lock(mutex_);
  for (auto& slot : slots_) {
    slot.active = false;
  }
  disconnect_count_.store(static_cast<uint32_t>(slots_.size()),
                          std::memory_order_relaxed);
  needs_cleanup_.store(true, std::memory_order_release);
}

template <typename... Args>
template <typename... A>
void Signal<Args...>::Emit(A&&... args) {
  {
    std::shared_lock lock(mutex_);

    // Call all active slots - use const& to avoid moving args
    for (const auto& slot : slots_) {
      if (slot.active) {
        try {
          slot.callback(args...);
        } catch (...) {
          // Swallow exceptions to prevent one slot from breaking others
        }
      }
    }
  }

  // After releasing the shared lock, check if cleanup is needed
  // Only one thread will successfully perform cleanup due to the atomic
  // exchange
  if (needs_cleanup_.load(std::memory_order_acquire)) {
    std::unique_lock lock(mutex_);
    // Double-check after acquiring a lock
    if (needs_cleanup_.exchange(false, std::memory_order_acq_rel)) {
      cleanupInternal();
    }
  }
}

template <typename... Args>
template <typename... A>
void Signal<Args...>::BlockingEmit(A&&... args) {
  std::unique_lock lock(mutex_);

  for (const auto& slot : slots_) {
    if (slot.active) {
      try {
        slot.callback(std::forward<A>(args)...);
      } catch (...) {
      }
    }
  }

  // Perform cleanup if needed
  if (needs_cleanup_.exchange(false, std::memory_order_acq_rel)) {
    cleanupInternal();
  }
}

template <typename... Args>
template <typename... A>
void Signal<Args...>::operator()(A&&... args) {
  emit(std::forward<A>(args)...);
}

template <typename... Args>
auto Signal<Args...>::Size() const noexcept -> size_t {
  std::shared_lock lock(mutex_);
  size_t count = 0;
  for (const auto& slot : slots_) {
    if (slot.active)
      ++count;
  }
  return count;
}

template <typename... Args>
auto Signal<Args...>::ContainerSize() const noexcept -> size_t {
  std::shared_lock lock(mutex_);
  const size_t sz = slots_.size();
  return sz;
}

template <typename... Args>
bool Signal<Args...>::Empty() const noexcept {
  return Size() == 0;
}

template <typename... Args>
void Signal<Args...>::Clear() noexcept {
  std::unique_lock lock(mutex_);
  slots_.clear();
  needs_cleanup_.store(false, std::memory_order_relaxed);
  disconnect_count_.store(0, std::memory_order_relaxed);
}

template <typename... Args>
void Signal<Args...>::forceCleanup() {
  std::unique_lock lock(mutex_);
  cleanupInternal();
}

template <typename... Args>
void Signal<Args...>::cleanupInternal() noexcept {
  slots_.erase(std::remove_if(slots_.begin(), slots_.end(),
                              [](const Slot& s) noexcept { return !s.active; }),
               slots_.end());
  needs_cleanup_.store(false, std::memory_order_release);
  disconnect_count_.store(0, std::memory_order_relaxed);
}
}  // namespace rhodo
