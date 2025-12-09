module;
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

export module Rhodo.Signals:SignalHub;

import :Signal;

export namespace rhodo {
class SignalHub {
 public:
  template <typename... Args>
  auto Get(const std::string& name) -> Signal<Args...>&;

  template <typename... Args>
  auto Remove(const std::string& name) -> void;

  auto Clear() noexcept -> void;
  auto CleanupEmptySignals() -> void;

  template <typename... Args>
  [[nodiscard]] auto Has(const std::string& name) const noexcept -> bool;

  [[nodiscard]] auto Size() noexcept -> size_t;

 private:
  struct SignalKey {
    std::string     name;
    std::type_index type;

    auto operator==(const SignalKey& other) const noexcept -> bool;
  };

  struct SignalKeyHash {
    auto operator()(const SignalKey& key) const noexcept -> size_t;
  };

  struct ISignalHolder {
    virtual ~ISignalHolder();
    [[nodiscard]] virtual auto IsEmpty() const noexcept -> bool = 0;
  };

  template <typename... Args>
  struct SignalHolder final : ISignalHolder {
    Signal<Args...> signal{};

    [[nodiscard]] auto IsEmpty() const noexcept -> bool override;
  };

  std::unordered_map<SignalKey, std::unique_ptr<ISignalHolder>, SignalKeyHash> signals_;
  mutable std::shared_mutex                                                    mutex_;
};

template <typename... Args>
auto SignalHub::Get(const std::string& name) -> Signal<Args...>& {
  const SignalKey key{.name = name, .type = std::type_index(typeid(Signal<Args...>))};

  // Fast path: READ lock for existing signals
  {
    std::shared_lock read_lock(mutex_);
    if (const auto kIt = signals_.find(key); kIt != signals_.end()) {
      return static_cast<SignalHolder<Args...>&>(*kIt->second).signal;
    }
  }

  // Slow path: WRITE lock to create a signal
  std::unique_lock write_lock(mutex_);

  // Double-check: another thread might have created it
  if (const auto kIt = signals_.find(key); kIt != signals_.end()) {
    return static_cast<SignalHolder<Args...>&>(*kIt->second).signal;
  }

  auto  holder     = std::make_unique<SignalHolder<Args...>>();
  auto& signal_ref = holder->signal;
  signals_[key]    = std::move(holder);
  return signal_ref;
}

template <typename... Args>
auto SignalHub::Remove(const std::string& name) -> void {
  std::unique_lock lock(mutex_);
  const SignalKey  key{.name = name, .type = std::type_index(typeid(Signal<Args...>))};
  signals_.erase(key);
}

auto SignalHub::Clear() noexcept -> void {
  std::unique_lock lock(mutex_);
  signals_.clear();
}

auto SignalHub::CleanupEmptySignals() -> void {
  std::unique_lock lock(mutex_);
  for (auto it = signals_.begin(); it != signals_.end();) {
    if (it->second->IsEmpty()) {
      it = signals_.erase(it);
    } else {
      ++it;
    }
  }
}

template <typename... Args>
auto SignalHub::Has(const std::string& name) const noexcept -> bool {
  std::shared_lock lock(mutex_);
  const SignalKey  key{.name = name, .type = std::type_index(typeid(Signal<Args...>))};
  return signals_.contains(key);
}

auto SignalHub::Size() noexcept -> size_t {
  std::shared_lock lock(mutex_);
  return signals_.size();
}

auto SignalHub::SignalKey::operator==(const SignalKey& other) const noexcept -> bool {
  return name == other.name && type == other.type;
}

auto SignalHub::SignalKeyHash::operator()(const SignalKey& key) const noexcept -> size_t {
  // Improved hash combination using the golden ratio
  const size_t kH1 = std::hash<std::string>{}(key.name);
  const size_t kH2 = std::hash<std::type_index>{}(key.type);
  return kH1 ^ (kH2 + 0x9e3779b9 + (kH1 << 6) + (kH1 >> 2));
}

SignalHub::ISignalHolder::~ISignalHolder() = default;

template <typename... Args>
auto SignalHub::SignalHolder<Args...>::IsEmpty() const noexcept -> bool {
  return signal.empty();
}
}   // namespace rhodo
