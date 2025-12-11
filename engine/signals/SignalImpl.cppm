module;
#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <shared_mutex>

export module Rhodo.Signals:SignalImpl;

import :Signal;

namespace rhodo {
template <typename... Args>
auto Signal<Args...>::NextId() const -> uint64_t {
  return next_id_.load();
}

template <typename... Args>
template <typename T>
auto Signal<Args...>::Connect(T& obj, void (T::*method)(Args...)) -> SlotId {
  return Connect([&obj, method](Args&&... args) -> void {
    (obj.*method)(std::forward<Args>(args)...);
    ;
  });
}

template <typename... Args>
auto Signal<Args...>::Connect(std::function<void(Args...)> callback) -> SlotId {
  std::unique_lock lock(mutex_);

  SlotId slot_id = next_id_.fetch_add(1, std::memory_order_relaxed);

  // Guard against overflow (wrap to 1, skip 0 as reserved)
  if (slot_id == 0) {
    slot_id = 1;
    next_id_.store(2, std::memory_order_relaxed);
  }

  slots_.push_back({std::move(callback), slot_id, true});
  return slot_id;
}

template <typename... Args>
auto Signal<Args...>::Disconnect(SlotId slot_id) -> void {
  std::unique_lock lock(mutex_);

  for (auto& slot : slots_) {
    if (slot.id == slot_id && slot.active) {
      slot.active = false;

      // Trigger cleanup if the threshold reached
      if (const uint32_t kCount =
              disconnect_count_.fetch_add(1, std::memory_order_relaxed) + 1;
          kCount >= kCleanupThreshold) {
        needs_cleanup_.store(true, std::memory_order_release);
          } else {
          }
      return;
    }
  }
}

template <typename... Args>
auto Signal<Args...>::DisconnectAll() noexcept -> void {
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
auto Signal<Args...>::Emit(A&&... args) -> void {
  {
    std::shared_lock lock(mutex_);

    // Call all active slots - use const& to avoid moving args
    for (const auto& slot : slots_) {
      if (slot.active) {
        try {
          slot.callback(std::forward<A>(args)...);
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
      CleanupInternal();
    }
  }
}

template <typename... Args>
template <typename... A>
auto Signal<Args...>::BlockingEmit(A&&... args) -> void {
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
    CleanupInternal();
  }
}

template <typename... Args>
template <typename... A>
auto Signal<Args...>::operator()(A&&... args) -> void {
  Emit(std::forward<A>(args)...);
}

template <typename... Args>
auto Signal<Args...>::Size() const noexcept -> size_t {
  std::shared_lock lock(mutex_);
  size_t count = 0;
  for (const auto& slot : slots_) {
    if (slot.active) {
      ++count;
    }
  }
  return count;
}

template <typename... Args>
auto Signal<Args...>::ContainerSize() const noexcept -> size_t {
  std::shared_lock lock(mutex_);
  const size_t kSz = slots_.size();
  return kSz;
}

template <typename... Args>
auto Signal<Args...>::Empty() const noexcept -> bool {
  return Size() == 0;
}

template <typename... Args>
auto Signal<Args...>::Clear() noexcept -> void {
  std::unique_lock lock(mutex_);
  slots_.clear();
  needs_cleanup_.store(false, std::memory_order_relaxed);
  disconnect_count_.store(0, std::memory_order_relaxed);
}

template <typename... Args>
auto Signal<Args...>::ForceCleanup() -> void {
  std::unique_lock lock(mutex_);
  CleanupInternal();
}

template <typename... Args>
auto Signal<Args...>::CleanupInternal() noexcept -> void {
  slots_.erase(std::remove_if(slots_.begin(), slots_.end(),
                              [](const Slot& slot) noexcept -> bool {
                                return !slot.active;
                              }),
               slots_.end());
  needs_cleanup_.store(false, std::memory_order_release);
  disconnect_count_.store(0, std::memory_order_relaxed);
}
} // namespace rhodo