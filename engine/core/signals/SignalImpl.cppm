module;
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
export module Rhodo.Core.Signals:SignalImpl;

import :Signal;
import :Hooks;

namespace rhodo {
    template<typename... Args>
    auto Signal<Args...>::next_id() const -> uint64_t {
        return next_id_.load();
    }

    template<typename... Args>
    template<typename T>
    auto Signal<Args...>::connect(T &obj, void (T::*method)(Args...)) -> SlotId {
        return connect([&obj, method](Args &&... args) -> void {
            (obj.*method)(std::forward<Args>(args)...);;
        });
    }

    template<typename... Args>
    auto Signal<Args...>::connect(std::function<void(Args...)> callback) -> SlotId {
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

    template<typename... Args>
    auto Signal<Args...>::disconnect(SlotId slot_id) -> void {
        std::unique_lock lock(mutex_);

        for (auto &slot: slots_) {
            if (slot.id == slot_id && slot.active) {
                slot.active = false;

                // Trigger cleanup if the threshold reached
                if (const uint32_t count =
                            disconnect_count_.fetch_add(1, std::memory_order_relaxed) + 1;
                    count >= k_cleanup_threshold) {
                    needs_cleanup_.store(true, std::memory_order_release);
                } else {
                }
                return;
            }
        }
    }

    template<typename... Args>
    auto Signal<Args...>::disconnect_all() noexcept -> void {
        std::unique_lock lock(mutex_);
        for (auto &slot: slots_) {
            slot.active = false;
        }
        disconnect_count_.store(static_cast<uint32_t>(slots_.size()),
                                std::memory_order_relaxed);
        needs_cleanup_.store(true, std::memory_order_release);
    }

    template<typename... Args>
    template<typename... A>
    auto Signal<Args...>::emit(A &&... args) -> void {
        core::signals::notify_emit_begin("Signal::Emit");
        {
            std::shared_lock lock(mutex_);

            // Call all active slots - use const& to avoid moving args
            for (const auto &slot: slots_) {
                if (slot.active) {
                    try {
                        slot.callback(std::forward<A>(args)...);
                    } catch (...) {
                        // Swallow exceptions to prevent one slot from breaking others
                    }
                }
            }
        }
        core::signals::notify_emit_end();

        // After releasing the shared lock, check if cleanup is needed
        // Only one thread will successfully perform cleanup due to the atomic
        // exchange
        if (needs_cleanup_.load(std::memory_order_acquire)) {
            std::unique_lock lock(mutex_);
            // Double-check after acquiring a lock
            if (needs_cleanup_.exchange(false, std::memory_order_acq_rel)) {
                cleanup_internal();
            }
        }
    }

    template<typename... Args>
    template<typename... A>
    auto Signal<Args...>::blocking_emit(A &&... args) -> void {
        core::signals::notify_emit_begin("Signal::blocking_emit");
        std::unique_lock lock(mutex_);

        for (const auto &slot: slots_) {
            if (slot.active) {
                try {
                    slot.callback(std::forward<A>(args)...);
                } catch (...) {
                }
            }
        }
        core::signals::notify_emit_end();

        // Perform cleanup if needed
        if (needs_cleanup_.exchange(false, std::memory_order_acq_rel)) {
            cleanup_internal();
        }
    }

    template<typename... Args>
    template<typename... A>
    auto Signal<Args...>::operator()(A &&... args) -> void {
        Emit(std::forward<A>(args)...);
    }

    template<typename... Args>
    auto Signal<Args...>::size() const noexcept -> size_t {
        std::shared_lock lock(mutex_);
        size_t count = 0;
        for (const auto &slot: slots_) {
            if (slot.active) {
                ++count;
            }
        }
        return count;
    }

    template<typename... Args>
    auto Signal<Args...>::container_size() const noexcept -> size_t {
        std::shared_lock lock(mutex_);
        const size_t size = slots_.size();
        return size;
    }

    template<typename... Args>
    auto Signal<Args...>::empty() const noexcept -> bool {
        return size() == 0;
    }

    template<typename... Args>
    auto Signal<Args...>::clear() noexcept -> void {
        std::unique_lock lock(mutex_);
        slots_.clear();
        needs_cleanup_.store(false, std::memory_order_relaxed);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }

    template<typename... Args>
    auto Signal<Args...>::force_cleanup() -> void {
        std::unique_lock lock(mutex_);
        cleanup_internal();
    }

    template<typename... Args>
    auto Signal<Args...>::cleanup_internal() noexcept -> void {
        slots_.erase(std::remove_if(slots_.begin(), slots_.end(),
                                    [](const Slot &slot) noexcept -> bool {
                                        return !slot.active;
                                    }),
                     slots_.end());
        needs_cleanup_.store(false, std::memory_order_release);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }
} // namespace rhodo
