module;
#include <algorithm>
#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>

export module Rhodo.Signals:Signal;


export namespace Rhodo
{
    using slotId = uint64_t;

    template <typename... Args>
    class Signal
    {
    public:

        static constexpr uint32_t cleanup_threshold = 16;
        auto nextId() const -> uint64_t ;

        // Connect - WRITE operation
        template <typename T>
        auto connect(T &obj, void (T::*method)(Args...)) -> slotId ;
        auto connect(std::function<void(Args...)> callback) -> slotId ;

        // Disconnect - WRITE
        void disconnect(slotId id);
        void disconnectAll() noexcept;

        // Emit signal - READ operation
        template <typename... A>
        void emit(A &&... args);
        template <typename... A>
        void blockingEmit(A &&... args);

        // Operator() as alias for emit
        template <typename... A>
        void operator()(A &&... args);

        // Query
        [[nodiscard]] auto size() const noexcept -> size_t ; // ACTIVE slots
        [[nodiscard]] auto containerSize() const noexcept -> size_t ; // slots incl inactive waiting for cleanup
        [[nodiscard]] bool empty() const noexcept;

        // Clear all slots - WRITE operation
        void clear() noexcept;
        void forceCleanup();

    private:
        void cleanupInternal() noexcept;

        struct Slot
        {
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

    template<typename ... Args>
    auto Signal<Args...>::nextId() const -> uint64_t { return next_id_.load(); }

    template<typename ... Args>
    template<typename T>
    auto Signal<Args...>::connect(T &obj, void(T::*method)(Args...)) -> slotId {
        return connect([&obj, method](Args&&... args)
        {
            (obj.*method)(std::forward<Args>(args)...);
        });
    }

    template<typename ... Args>
    auto Signal<Args...>::connect(std::function<void(Args...)> callback) -> slotId {
        std::unique_lock lock(mutex_);

        slotId id = next_id_.fetch_add(1, std::memory_order_relaxed);

        // Guard against overflow (wrap to 1, skip 0 as reserved)
        if (id == 0)
        {
            id = 1;
            next_id_.store(2, std::memory_order_relaxed);
        }

        slots_.push_back({std::move(callback), id, true});
        return id;
    }

    template<typename ... Args>
    void Signal<Args...>::disconnect(slotId id) {
        std::unique_lock lock(mutex_);

        for (auto& slot : slots_)
        {
            if (slot.id == id && slot.active)
            {
                slot.active = false;

                // Trigger cleanup if the threshold reached
                if (const uint32_t count = disconnect_count_.fetch_add(1, std::memory_order_relaxed) + 1; count >= cleanup_threshold)
                {
                    needs_cleanup_.store(true, std::memory_order_release);
                }
                else
                {
                }
                return;
            }
        }

    }

    template<typename ... Args>
    void Signal<Args...>::disconnectAll() noexcept {
        std::unique_lock lock(mutex_);
        for (auto& slot : slots_)
        {
            slot.active = false;
        }
        disconnect_count_.store(static_cast<uint32_t>(slots_.size()), std::memory_order_relaxed);
        needs_cleanup_.store(true, std::memory_order_release);
    }

    template<typename ... Args>
    template<typename ... A>
    void Signal<Args...>::emit(A &&... args) {
        {
            std::shared_lock lock(mutex_);

            // Call all active slots - use const& to avoid moving args
            for (const auto& slot : slots_)
            {
                if (slot.active)
                {
                    try
                    {
                        slot.callback(args...);
                    }
                    catch (...)
                    {
                        // Swallow exceptions to prevent one slot from breaking others
                    }
                }
            }
        }

        // After releasing the shared lock, check if cleanup is needed
        // Only one thread will successfully perform cleanup due to the atomic exchange
        if (needs_cleanup_.load(std::memory_order_acquire))
        {
            std::unique_lock lock(mutex_);
            // Double-check after acquiring a lock
            if (needs_cleanup_.exchange(false, std::memory_order_acq_rel))
            {
                cleanupInternal();
            }
        }
    }

    template<typename ... Args>
    template<typename ... A>
    void Signal<Args...>::blockingEmit(A &&... args) {
        std::unique_lock lock(mutex_);

        for (const auto& slot : slots_)
        {
            if (slot.active)
            {
                try
                {
                    slot.callback(std::forward<A>(args)...);
                }
                catch (...)
                {
                }
            }
        }

        // Perform cleanup if needed
        if (needs_cleanup_.exchange(false, std::memory_order_acq_rel))
        {
            cleanupInternal();
        }
    }

    template<typename ... Args>
    template<typename ... A>
    void Signal<Args...>::operator()(A &&... args) {
        emit(std::forward<A>(args)...);
    }

    template<typename ... Args>
    auto Signal<Args...>::size() const noexcept -> size_t {
        std::shared_lock lock(mutex_);
        size_t count = 0;
        for (const auto& slot : slots_)
        {
            if (slot.active) ++count;
        }
        return count;
    }

    template<typename ... Args>
    auto Signal<Args...>::containerSize() const noexcept -> size_t {
        std::shared_lock lock(mutex_);
        const size_t sz = slots_.size();
        return sz;
    }

    template<typename ... Args>
    bool Signal<Args...>::empty() const noexcept {
        return size() == 0;
    }

    template<typename ... Args>
    void Signal<Args...>::clear() noexcept {
        std::unique_lock lock(mutex_);
        slots_.clear();
        needs_cleanup_.store(false, std::memory_order_relaxed);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }

    template<typename ... Args>
    void Signal<Args...>::forceCleanup() {
        std::unique_lock lock(mutex_);
        cleanupInternal();
    }

    template<typename ... Args>
    void Signal<Args...>::cleanupInternal() noexcept {
        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                           [](const Slot& s) noexcept { return !s.active; }),
            slots_.end()
        );
        needs_cleanup_.store(false, std::memory_order_release);
        disconnect_count_.store(0, std::memory_order_relaxed);
    }
}
