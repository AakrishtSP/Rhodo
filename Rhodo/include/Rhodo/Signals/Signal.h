// Rhodo/src/Rhodo/Events/Signal.h
#pragma once
#include <functional>
#include <memory>
#include <string>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <typeindex>


namespace Rhodo {

// ============================================================================
// Signal - Thread-safe signal/slot system with read-write lock optimization
// ============================================================================
template<typename... Args>
class Signal {
private:
    struct Slot {
        std::function<void(Args...)> callback;
        uint64_t id = 0;
        bool active = true;
    };

    // Use deque to avoid iterator invalidation during emission
    std::deque<Slot> slots_;
    std::atomic<uint64_t> next_id_{1};
    std::atomic<uint32_t> disconnect_count_{0};
    std::atomic<bool> needs_cleanup_{false};
    mutable std::shared_mutex mutex_;


public:
    // Batch cleanup threshold (tune based on your use case)
    static constexpr uint32_t CLEANUP_THRESHOLD = 16;

    uint64_t next_id() const {return next_id_.load();}

    // For testing only
    std::atomic<uint64_t>& _test_next_id_() { return next_id_; }

    using SlotID = uint64_t;

    // Connect a callback - WRITE operation
    SlotID connect(std::function<void(Args...)> callback) {
        std::unique_lock lock(mutex_);

        SlotID id = next_id_.fetch_add(1, std::memory_order_relaxed);

        // Guard against overflow (wrap to 1, skip 0 as reserved)
        if (id == 0) {
            id = 1;
            next_id_.store(2, std::memory_order_relaxed);
        }

        slots_.push_back({std::move(callback), id, true});
        return id;
    }

    // Connect member function - WRITE operation
    // WARNING: Object must outlive the signal or be disconnected before destruction
    template<typename T>
    SlotID connect(T& obj, void (T::*method)(Args...)) {
        return connect([&obj, method](Args&&... args) {
            (obj.*method)(std::forward<Args>(args)...);
        });
    }


    // Disconnect by ID - WRITE operation with batched cleanup
    void disconnect(SlotID id) {
        std::unique_lock lock(mutex_);

        for (auto& slot : slots_) {
            if (slot.id == id && slot.active) {
                slot.active = false;

                uint32_t count = disconnect_count_.fetch_add(1, std::memory_order_relaxed) + 1;

                // Trigger cleanup if threshold reached
                if (count >= CLEANUP_THRESHOLD) {
                    needs_cleanup_.store(true, std::memory_order_release);
                }
                return;
            }
        }
    }

    // Disconnect all slots - WRITE operation
    void disconnect_all() noexcept {
        std::unique_lock lock(mutex_);
        for (auto& slot : slots_) {
            slot.active = false;
        }
        disconnect_count_.store(static_cast<uint32_t>(slots_.size()), std::memory_order_relaxed);
        needs_cleanup_.store(true, std::memory_order_release);
    }

    // Emit signal - READ operation (multiple threads can emit concurrently!)
    template<typename... A>
    void emit(A&&... args) {
        // Exception-safe RAII guard for cleanup
        struct EmitGuard {
            Signal& sig;
            bool should_cleanup;

            EmitGuard(Signal& s) noexcept
                : sig(s)
                , should_cleanup(sig.needs_cleanup_.load(std::memory_order_acquire)) {}

            ~EmitGuard() noexcept {
                if (should_cleanup) {
                    sig.cleanup_if_needed();
                }
            }
        };

        EmitGuard guard(*this);

        // Shared lock allows multiple concurrent emits
        std::shared_lock lock(mutex_);

        // Call all active slots with perfect forwarding
        for (const auto& slot : slots_) {
            if (slot.active) {
                slot.callback(std::forward<A>(args)...);
            }
        }
    }

    // Blocking emit - WRITE operation (waits for all callbacks to complete)
    // Useful for shutdown sequences or when you need guaranteed delivery
    template<typename... A>
    void blocking_emit(A&&... args) {
        std::unique_lock lock(mutex_);

        for (const auto& slot : slots_) {
            if (slot.active) {
                try {
                    slot.callback(std::forward<A>(args)...);
                } catch (...) {
                    // Continue calling other slots even if one throws
                    // Consider logging here if you have a logger
                }
            }
        }

        // Perform cleanup if needed
        if (needs_cleanup_.exchange(false, std::memory_order_acq_rel)) {
            cleanup_internal();
        }
    }

    // Operator() as alias for emit
    template<typename... A>
    void operator()(A&&... args) {
        emit(std::forward<A>(args)...);
    }

    // Query - READ operation
    [[nodiscard]] size_t size() const noexcept {
        std::shared_lock lock(mutex_);
        size_t count = 0;
        for (const auto& slot : slots_) {
            if (slot.active) ++count;
        }
        return count;
    }

    [[nodiscard]] bool empty() const noexcept {
        return size() == 0;
    }

    // Clear all slots - WRITE operation
    void clear() noexcept {
        std::unique_lock lock(mutex_);
        slots_.clear();
        needs_cleanup_.store(false, std::memory_order_relaxed);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }

    // Force immediate cleanup - WRITE operation
    void force_cleanup() {
        std::unique_lock lock(mutex_);
        cleanup_internal();
    }

private:
    void cleanup_if_needed() {
        // Only acquire write lock if cleanup is actually needed
        if (!needs_cleanup_.load(std::memory_order_acquire)) {
            return;
        }

        std::unique_lock lock(mutex_);

        // Double-check pattern: another thread might have cleaned up
        if (!needs_cleanup_.load(std::memory_order_relaxed)) {
            return;
        }

        cleanup_internal();
    }

    void cleanup_internal() noexcept {
        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                [](const Slot& s) noexcept { return !s.active; }),
            slots_.end()
        );
        needs_cleanup_.store(false, std::memory_order_release);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }
};



} // namespace Rhodo
