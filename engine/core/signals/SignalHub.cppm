module;
#include <memory>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

export module Rhodo.Core.Signals:SignalHub;

import :Signal;

export namespace rhodo {
class SignalHub {
 public:
  template <typename... Args>
  auto get(const std::string& name) -> Signal<Args...>&;

  template <typename... Args>
  auto remove(const std::string& name) -> void;

  auto clear() noexcept -> void;
  auto cleanup_empty_signals() -> void;

  template <typename... Args>
  [[nodiscard]] auto has(const std::string& name) const noexcept -> bool;

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
    virtual ~ISignalHolder() = default;
    ISignalHolder(const ISignalHolder&) = delete;
    auto operator=(const ISignalHolder&) -> ISignalHolder& = delete;
    ISignalHolder(ISignalHolder&&) = delete;
    auto operator=(ISignalHolder&&) -> ISignalHolder& = delete;

    [[nodiscard]] virtual auto is_empty() const noexcept -> bool = 0;
  };

  template <typename... Args>
  struct SignalHolder final : ISignalHolder {
    SignalHolder() = default;
    Signal<Args...> signal{};
    [[nodiscard]] auto is_empty() const noexcept -> bool override;
  };

  std::unordered_map<SignalKey, std::unique_ptr<ISignalHolder>, SignalKeyHash>
      signals_;
  mutable std::shared_mutex mutex_;
};

}  // namespace rhodo
