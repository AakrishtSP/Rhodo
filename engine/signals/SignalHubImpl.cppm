module;
#include <algorithm>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

export module Rhodo.Signals:SignalHubImpl;
import :SignalHub;

using namespace rhodo;

template <typename... Args>
auto SignalHub::Get(const std::string& name) -> Signal<Args...>& {
  const SignalKey kKey{.name = name,
                       .type = std::type_index(typeid(Signal<Args...>))};

  // Fast path: READ lock for existing signals
  {
    const std::shared_lock kReadLock(mutex_);
    if (const auto kIt = signals_.find(kKey); kIt != signals_.end()) {
      return static_cast<SignalHolder<Args...>&>(*kIt->second).signal;
    }
  }

  // Slow path: WRITE lock to create a signal
  const std::unique_lock kWriteLock(mutex_);

  // Double-check: another thread might have created it
  if (const auto kIt = signals_.find(kKey); kIt != signals_.end()) {
    return static_cast<SignalHolder<Args...>&>(*kIt->second).signal;
  }

  auto holder = std::make_unique<SignalHolder<Args...>>();
  auto& signal_ref = holder->signal;
  signals_[kKey] = std::move(holder);
  return signal_ref;
}

template <typename... Args>
auto SignalHub::Remove(const std::string& name) -> void {
  const std::unique_lock kLock(mutex_);
  const SignalKey kKey{.name = name,
                       .type = std::type_index(typeid(Signal<Args...>))};
  signals_.erase(kKey);
}

auto SignalHub::Clear() noexcept -> void {
  const std::unique_lock kLock(mutex_);
  signals_.clear();
}

auto SignalHub::CleanupEmptySignals() -> void {
  const std::unique_lock kLock(mutex_);
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
  const std::shared_lock kLock(mutex_);
  const SignalKey kKey{.name = name,
                       .type = std::type_index(typeid(Signal<Args...>))};
  return signals_.contains(kKey);
}

auto SignalHub::Size() const noexcept -> size_t {
  const std::shared_lock kLock(mutex_);
  return signals_.size();
}

auto SignalHub::SignalKey::operator==(const SignalKey& other) const noexcept
    -> bool {
  return name == other.name && type == other.type;
}
namespace {
auto WyMix(uint64_t a, const uint64_t b)  // NOLINT(*-identifier-length)
    -> uint64_t {
  a ^= b;
  a *= 0x165667919E3779F9ULL;
  a ^= (a >> 32);  // NOLINT(*-signed-bitwise)
  return a;
}

auto Combine(const uint64_t h1, const uint64_t h2)  // NOLINT(*-identifier-length)
    -> uint64_t {
  return WyMix(h1, h2);
}
}  // unnamed namespace
auto SignalHub::SignalKeyHash::operator()(const SignalKey& key) const noexcept
    -> size_t {
  const size_t kH1 = std::hash<std::string>{}(key.name);
  const size_t kH2 = std::hash<std::type_index>{}(key.type);
  return Combine(kH1, kH2);
}
template <typename... Args>
auto SignalHub::SignalHolder<Args...>::IsEmpty() const noexcept -> bool {
  return signal.empty();
}
