// Improved Signal.h with read-write mutex optimization
#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <shared_mutex>
#include <atomic>
#include <mutex>
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
        uint64_t id;
        bool active = true;
    };

    std::vector<Slot> slots_;
    std::atomic<uint64_t> next_id_{1};
    std::atomic<bool> needs_cleanup_{false};
    mutable std::shared_mutex mutex_;

public:
    using SlotID = uint64_t;

    // Connect a callback - WRITE operation
    SlotID connect(std::function<void(Args...)> callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        SlotID id = next_id_.fetch_add(1, std::memory_order_relaxed);
        slots_.push_back({std::move(callback), id, true});
        return id;
    }

    // Connect member function - WRITE operation
    template<typename T>
    SlotID connect(T* obj, void (T::*method)(Args...)) {
        return connect([obj, method](Args... args) {
            (obj->*method)(args...);
        });
    }

    // Disconnect by ID - WRITE operation
    void disconnect(SlotID id) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        for (auto& slot : slots_) {
            if (slot.id == id && slot.active) {
                slot.active = false;
                needs_cleanup_.store(true, std::memory_order_release);
                return;
            }
        }
    }

    // Emit signal - READ operation (multiple threads can emit concurrently!)
    void operator()(Args... args) {
        // Shared lock allows multiple concurrent emits
        std::shared_lock<std::shared_mutex> lock(mutex_);

        // Call all active slots while holding shared lock
        // This is safe because:
        // 1. Vector won't be modified (write lock prevents it)
        // 2. Callbacks are copyable functors (stable references)
        // 3. Multiple threads can read simultaneously
        for (const auto& slot : slots_) {
            if (slot.active) {
                slot.callback(args...);
            }
        }

        // Release lock before cleanup check
        lock.unlock();

        // Cleanup if needed (will acquire write lock)
        if (needs_cleanup_.load(std::memory_order_acquire)) {
            cleanup();
        }
    }

    // Alternative emit syntax
    void emit(Args... args) {
        (*this)(args...);
    }

    // Query - READ operation
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& slot : slots_) {
            if (slot.active) ++count;
        }
        return count;
    }

    bool empty() const {
        return size() == 0;
    }

    // Clear - WRITE operation
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        slots_.clear();
        needs_cleanup_.store(false, std::memory_order_relaxed);
    }

    // Check if cleanup is needed - READ operation
    bool needs_cleanup() const {
        return needs_cleanup_.load(std::memory_order_acquire);
    }

private:
    // Cleanup - WRITE operation
    void cleanup() {
        std::unique_lock<std::shared_mutex> lock(mutex_);

        // Double-check pattern: another thread might have cleaned up
        if (!needs_cleanup_.load(std::memory_order_relaxed)) {
            return;
        }

        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                [](const Slot& s) { return !s.active; }),
            slots_.end()
        );
        needs_cleanup_.store(false, std::memory_order_release);
    }
};

// ============================================================================
// SignalCopyEmit - Alternative with snapshot emission (safer for callbacks
// that might disconnect during emit)
// ============================================================================
template<typename... Args>
class SignalCopyEmit {
private:
    struct Slot {
        std::function<void(Args...)> callback;
        uint64_t id;
        bool active = true;
    };

    std::vector<Slot> slots_;
    std::atomic<uint64_t> next_id_{1};
    std::atomic<bool> needs_cleanup_{false};
    mutable std::shared_mutex mutex_;

public:
    using SlotID = uint64_t;

    SlotID connect(std::function<void(Args...)> callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        SlotID id = next_id_.fetch_add(1, std::memory_order_relaxed);
        slots_.push_back({std::move(callback), id, true});
        return id;
    }

    template<typename T>
    SlotID connect(T* obj, void (T::*method)(Args...)) {
        return connect([obj, method](Args... args) {
            (obj->*method)(args...);
        });
    }

    void disconnect(SlotID id) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        for (auto& slot : slots_) {
            if (slot.id == id && slot.active) {
                slot.active = false;
                needs_cleanup_.store(true, std::memory_order_release);
                return;
            }
        }
    }

    // Makes a snapshot copy before emitting
    void operator()(Args... args) {
        std::vector<std::function<void(Args...)>> callbacks;

        {
            std::shared_lock<std::shared_mutex> lock(mutex_);
            callbacks.reserve(slots_.size());
            for (const auto& slot : slots_) {
                if (slot.active) {
                    callbacks.push_back(slot.callback);
                }
            }
        }

        // Call without holding any lock - safe for self-disconnection
        for (auto& callback : callbacks) {
            callback(args...);
        }

        if (needs_cleanup_.load(std::memory_order_acquire)) {
            cleanup();
        }
    }

    void emit(Args... args) { (*this)(args...); }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& slot : slots_) {
            if (slot.active) ++count;
        }
        return count;
    }

    bool empty() const { return size() == 0; }

    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        slots_.clear();
        needs_cleanup_.store(false, std::memory_order_relaxed);
    }

private:
    void cleanup() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (!needs_cleanup_.load(std::memory_order_relaxed)) {
            return;
        }

        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                [](const Slot& s) { return !s.active; }),
            slots_.end()
        );
        needs_cleanup_.store(false, std::memory_order_release);
    }
};

// ============================================================================
// SignalLockFree - For single-threaded high-performance scenarios
// ============================================================================
template<typename... Args>
class SignalLockFree {
private:
    struct Slot {
        std::function<void(Args...)> callback;
        uint64_t id;
        bool active = true;
    };

    std::vector<Slot> slots_;
    uint64_t next_id_ = 1;
    int emit_depth_ = 0;
    bool needs_cleanup_ = false;

public:
    using SlotID = uint64_t;

    SlotID connect(std::function<void(Args...)> callback) {
        SlotID id = next_id_++;
        slots_.push_back({std::move(callback), id, true});
        return id;
    }

    template<typename T>
    SlotID connect(T* obj, void (T::*method)(Args...)) {
        return connect([obj, method](Args... args) {
            (obj->*method)(args...);
        });
    }

    void disconnect(SlotID id) {
        for (auto& slot : slots_) {
            if (slot.id == id && slot.active) {
                slot.active = false;
                if (emit_depth_ > 0) {
                    needs_cleanup_ = true;
                }
                return;
            }
        }
    }

    void operator()(Args... args) {
        ++emit_depth_;

        // Use index-based loop to handle potential vector growth
        size_t size = slots_.size();
        for (size_t i = 0; i < size; ++i) {
            if (slots_[i].active) {
                slots_[i].callback(args...);
            }
        }

        --emit_depth_;

        if (emit_depth_ == 0 && needs_cleanup_) {
            cleanup();
        }
    }

    void emit(Args... args) { (*this)(args...); }

    size_t size() const {
        size_t count = 0;
        for (const auto& slot : slots_) {
            if (slot.active) ++count;
        }
        return count;
    }

    bool empty() const { return size() == 0; }
    void clear() {
        slots_.clear();
        needs_cleanup_ = false;
    }

private:
    void cleanup() {
        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                [](const Slot& s) { return !s.active; }),
            slots_.end()
        );
        needs_cleanup_ = false;
    }
};

// ============================================================================
// ScopedConnection - RAII wrapper for automatic disconnection
// ============================================================================
template<typename SignalType>
class ScopedConnection {
    SignalType* signal_ = nullptr;
    typename SignalType::SlotID id_ = 0;

public:
    ScopedConnection() = default;

    ScopedConnection(SignalType* signal, typename SignalType::SlotID id)
        : signal_(signal), id_(id) {}

    ~ScopedConnection() { disconnect(); }

    ScopedConnection(ScopedConnection&& other) noexcept
        : signal_(other.signal_), id_(other.id_) {
        other.signal_ = nullptr;
        other.id_ = 0;
    }

    ScopedConnection& operator=(ScopedConnection&& other) noexcept {
        if (this != &other) {
            disconnect();
            signal_ = other.signal_;
            id_ = other.id_;
            other.signal_ = nullptr;
            other.id_ = 0;
        }
        return *this;
    }

    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;

    void disconnect() {
        if (signal_) {
            signal_->disconnect(id_);
            signal_ = nullptr;
            id_ = 0;
        }
    }

    bool connected() const { return signal_ != nullptr; }
};

template<typename SignalType>
ScopedConnection<SignalType> connect_scoped(SignalType& signal,
                                            std::function<void(typename SignalType::SlotID)> callback) {
    auto id = signal.connect(std::move(callback));
    return ScopedConnection<SignalType>(&signal, id);
}

template<typename SignalType, typename T, typename... Args>
ScopedConnection<SignalType> connect_scoped(SignalType& signal,
                                            T* obj, void (T::*method)(Args...)) {
    auto id = signal.connect(obj, method);
    return ScopedConnection<SignalType>(&signal, id);
}

// ============================================================================
// Type-safe SignalHub with read-write locks
// ============================================================================
class SignalHub {
private:
    struct SignalKey {
        std::string name;
        std::type_index type;

        bool operator==(const SignalKey& other) const {
            return name == other.name && type == other.type;
        }
    };

    struct SignalKeyHash {
        size_t operator()(const SignalKey& key) const {
            return std::hash<std::string>{}(key.name) ^
                   std::hash<std::type_index>{}(key.type);
        }
    };

    struct ISignalHolder {
        virtual ~ISignalHolder() = default;
    };

    template<typename... Args>
    struct SignalHolder : ISignalHolder {
        Signal<Args...> signal;
    };

    std::unordered_map<SignalKey, std::unique_ptr<ISignalHolder>, SignalKeyHash> signals_;
    mutable std::shared_mutex mutex_;

public:
    // Get or create a signal - may require WRITE lock
    template<typename... Args>
    Signal<Args...>& get(const std::string& name) {
        SignalKey key{name, std::type_index(typeid(Signal<Args...>))};

        // Try READ lock first (fast path for existing signals)
        {
            std::shared_lock<std::shared_mutex> read_lock(mutex_);
            auto it = signals_.find(key);
            if (it != signals_.end()) {
                auto* holder = static_cast<SignalHolder<Args...>*>(it->second.get());
                return holder->signal;
            }
        }

        // Need to create signal - acquire WRITE lock
        std::unique_lock<std::shared_mutex> write_lock(mutex_);

        // Double-check: another thread might have created it
        auto it = signals_.find(key);
        if (it != signals_.end()) {
            auto* holder = static_cast<SignalHolder<Args...>*>(it->second.get());
            return holder->signal;
        }

        // Create new signal
        auto holder = std::make_unique<SignalHolder<Args...>>();
        auto& signal = holder->signal;
        signals_[key] = std::move(holder);
        return signal;
    }

    // Check if signal exists - READ operation
    template<typename... Args>
    bool has(const std::string& name) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
        return signals_.find(key) != signals_.end();
    }

    // Remove a signal - WRITE operation
    template<typename... Args>
    void remove(const std::string& name) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
        signals_.erase(key);
    }

    // Clear all signals - WRITE operation
    void clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        signals_.clear();
    }

    // Get count - READ operation
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return signals_.size();
    }
};

// ============================================================================
// Global signal access
// ============================================================================
namespace Signals {
    inline SignalHub& global() {
        static SignalHub hub;
        return hub;
    }

    template<typename... Args>
    Signal<Args...>& get(const std::string& name) {
        return global().get<Args...>(name);
    }
}

} // namespace Rhodo

// ============================================================================
// PERFORMANCE CHARACTERISTICS
// ============================================================================

/*
 * Signal (with shared_mutex):
 * - Multiple threads can emit() concurrently (shared lock)
 * - connect()/disconnect() blocks all operations (unique lock)
 * - Best for: General purpose, high emit/low modify ratio
 * - Tradeoff: Callbacks see consistent state but hold lock during execution
 *
 * SignalCopyEmit (with shared_mutex + snapshot):
 * - Multiple threads can prepare emission concurrently (shared lock)
 * - Callbacks execute without holding any locks
 * - Safe for callbacks that disconnect themselves/others
 * - Best for: When callbacks might modify signal connections
 * - Tradeoff: Small overhead copying callbacks (~8-24 bytes per slot)
 *
 * SignalLockFree:
 * - Zero synchronization overhead
 * - Best for: Single-threaded game loops, controlled threading
 * - Tradeoff: NOT thread-safe, must ensure single-threaded access
 *
 * RECOMMENDATION:
 * - Use Signal for most cases (optimal read-write performance)
 * - Use SignalCopyEmit if callbacks disconnect during emission
 * - Use SignalLockFree for tight game loops with threading control
 */