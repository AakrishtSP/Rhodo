module;
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

export module rhodo.signals:signal_hub;

import :signal;

export namespace rhodo {
class SignalHub {
 public:
  template <typename... Args>
  auto get(const std::string& name) -> Signal<Args...>&;

  template <typename... Args>
  void remove(const std::string& name);

  void clear() noexcept;
  void cleanupEmptySignals();

  template <typename... Args>
  [[nodiscard]] bool has(const std::string& name) const noexcept;

  [[nodiscard]] auto size() const noexcept -> size_t;

 private:
  struct SignalKey {
    std::string name;
    std::type_index type;

    auto operator==(const SignalKey& other) const noexcept -> bool;
  };

  struct SignalKeyHash {
    auto operator()(const SignalKey& key) const noexcept -> size_t;
  };

  struct ISignalHolder {
    virtual ~ISignalHolder();
    [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;
  };

  template <typename... Args>
  struct SignalHolder final : ISignalHolder {
    Signal<Args...> signal{};

    [[nodiscard]] auto is_empty() const noexcept -> bool override;
  };

  std::unordered_map<SignalKey, std::unique_ptr<ISignalHolder>, SignalKeyHash>
      signals_;
  mutable std::shared_mutex mutex_;
};

template <typename... Args>
auto SignalHub::get(const std::string& name) -> Signal<Args...>& {
  const SignalKey key{name, std::type_index(typeid(Signal<Args...>))};

  // Fast path: READ lock for existing signals
  {
    std::shared_lock readLock(mutex_);
    if (const auto it = signals_.find(key); it != signals_.end()) {
      return static_cast<SignalHolder<Args...>&>(*it->second).signal;
    }
  }

  // Slow path: WRITE lock to create a signal
  std::unique_lock writeLock(mutex_);

  // Double-check: another thread might have created it
  if (const auto it = signals_.find(key); it != signals_.end()) {
    return static_cast<SignalHolder<Args...>&>(*it->second).signal;
  }

  auto holder = std::make_unique<SignalHolder<Args...>>();
  auto& signalRef = holder->signal;
  signals_[key] = std::move(holder);
  return signalRef;
}

template <typename... Args>
void SignalHub::remove(const std::string& name) {
  std::unique_lock lock(mutex_);
  const SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
  signals_.erase(key);
}

void SignalHub::clear() noexcept {
  std::unique_lock lock(mutex_);
  signals_.clear();
}

void SignalHub::cleanupEmptySignals() {
  std::unique_lock lock(mutex_);
  for (auto it = signals_.begin(); it != signals_.end();) {
    if (it->second->is_empty()) {
      it = signals_.erase(it);
    } else {
      ++it;
    }
  }
}

template <typename... Args>
bool SignalHub::has(const std::string& name) const noexcept {
  std::shared_lock lock(mutex_);
  const SignalKey key{name, std::type_index(typeid(Signal<Args...>))};
  return signals_.contains(key);
}

auto SignalHub::size() const noexcept -> size_t {
  std::shared_lock lock(mutex_);
  return signals_.size();
}

bool SignalHub::SignalKey::operator==(const SignalKey& other) const noexcept {
  return name == other.name && type == other.type;
}

auto SignalHub::SignalKeyHash::operator()(const SignalKey& key) const noexcept
    -> size_t {
  // Improved hash combination using the golden ratio
  const size_t h1 = std::hash<std::string>{}(key.name);
  const size_t h2 = std::hash<std::type_index>{}(key.type);
  return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

SignalHub::ISignalHolder::~ISignalHolder() = default;

template <typename... Args>
bool SignalHub::SignalHolder<Args...>::is_empty() const noexcept {
  return signal.empty();
}
}  // namespace rhodo
