module;
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>

export module Rhodo.Debug:ScopeGuards;

import Rhodo.Logger;

export namespace rhodo::debug {
template <typename F>
struct ScopeExit {
  F fn;
  bool active = true;

  explicit ScopeExit(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(f)) {}

  ~ScopeExit() noexcept {
    if (active) {
      fn();
    }
  }

  auto Dismiss() noexcept -> void { active = false; }

  ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(other.fn)), active(other.active) {
    other.active = false;
  }

  ScopeExit(const ScopeExit&) = delete;
  auto operator=(const ScopeExit&) -> ScopeExit& = delete;
  auto operator=(ScopeExit&&) -> ScopeExit& = delete;
};

template <typename F>
ScopeExit(F) -> ScopeExit<F>;

template <typename F>
struct ScopeFail {
  F fn;
  int exceptions_on_entry;

  explicit ScopeFail(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(f)), exceptions_on_entry(std::uncaught_exceptions()) {}

  ~ScopeFail() noexcept {
    if (std::uncaught_exceptions() > exceptions_on_entry) {
      fn();
    }
  }

  ScopeFail(const ScopeFail&) = delete;
  auto operator=(const ScopeFail&) -> ScopeFail& = delete;
  ScopeFail(ScopeFail&&) = delete;
  auto operator=(ScopeFail&&) -> ScopeFail& = delete;
};

template <typename F>
ScopeFail(F) -> ScopeFail<F>;

template <typename F>
struct ScopeSuccess {
  F fn;
  int exceptions_on_entry;

  explicit ScopeSuccess(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(f)), exceptions_on_entry(std::uncaught_exceptions()) {}

  ~ScopeSuccess() noexcept(noexcept(fn())) {
    if (std::uncaught_exceptions() <= exceptions_on_entry) {
      fn();
    }
  }

  ScopeSuccess(const ScopeSuccess&) = delete;
  auto operator=(const ScopeSuccess&) -> ScopeSuccess& = delete;
  ScopeSuccess(ScopeSuccess&&) = delete;
  auto operator=(ScopeSuccess&&) -> ScopeSuccess& = delete;
};

template <typename F>
ScopeSuccess(F) -> ScopeSuccess<F>;
}  // namespace rhodo::debug
