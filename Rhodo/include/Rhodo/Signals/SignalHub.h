// Rhodo/include/Rhodo/Signals/SignalHub.h
#pragma once
#include "ScopedConnection.h"
#include "Signal.h"

namespace Rhodo
{
    // ============================================================================
    // SignalHub - Thread-safe named signal registry with type safety
    // ============================================================================
    class SignalHub {
    private:
        struct SignalKey {
            std::string name;
            std::type_index type;

            bool operator==(const SignalKey& other) const noexcept {
                return name == other.name && type == other.type;
            }
        };

        struct SignalKeyHash {
            size_t operator()(const SignalKey& key) const noexcept {
                // Improved hash combination using golden ratio
                size_t h1 = std::hash<std::string>{}(key.name);
                size_t h2 = std::hash<std::type_index>{}(key.type);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        struct ISignalHolder {
            virtual ~ISignalHolder() = default;
            [[nodiscard]] virtual bool is_empty() const noexcept = 0;
        };

        template<typename... Args>
        struct SignalHolder : ISignalHolder {
            Signal<Args...> signal;

            [[nodiscard]] bool is_empty() const noexcept override {
                return signal.empty();
            }
        };

        std::unordered_map<SignalKey, std::unique_ptr<ISignalHolder>, SignalKeyHash> signals_;
        mutable std::shared_mutex mutex_;

    public:
        // Get or create a signal - uses double-checked locking pattern
        template<typename... Args>
        Signal<Args...>& get(const std::string& name) {
            // Use Signal type itself for reliable ODR-safe matching across TUs
            SignalKey key{name, std::type_index(typeid(Signal<Args...>))};

            // Fast path: READ lock for existing signals
            {
                std::shared_lock read_lock(mutex_);
                auto it = signals_.find(key);
                if (it != signals_.end()) {
                    return static_cast<SignalHolder<Args...>&>(*it->second).signal;
                }
            }

            // Slow path: WRITE lock to create signal
            std::unique_lock write_lock(mutex_);

            // Double-check: another thread might have created it
            auto it = signals_.find(key);
            if (it != signals_.end()) {
                return static_cast<SignalHolder<Args...>&>(*it->second).signal;
            }

            // Create new signal
            auto holder = std::make_unique<SignalHolder<Args...>>();
            auto& signal_ref = holder->signal;
            signals_[key] = std::move(holder);
            return signal_ref;
        }

        // Check if signal exists - READ operation
        template<typename... Args>
        [[nodiscard]] bool has(const std::string& name) const noexcept {
            std::shared_lock lock(mutex_);
            SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
            return signals_.contains(key);
        }

        // Remove a signal - WRITE operation
        template<typename... Args>
        void remove(const std::string& name) {
            std::unique_lock lock(mutex_);
            SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
            signals_.erase(key);
        }

        // Clear all signals - WRITE operation
        void clear() noexcept {
            std::unique_lock lock(mutex_);
            signals_.clear();
        }

        // Cleanup empty signals - WRITE operation
        void cleanup_empty_signals() {
            std::unique_lock lock(mutex_);
            for (auto it = signals_.begin(); it != signals_.end(); ) {
                if (it->second->is_empty()) {
                    it = signals_.erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Get count - READ operation
        [[nodiscard]] size_t size() const noexcept {
            std::shared_lock lock(mutex_);
            return signals_.size();
        }
    };
}