module;
#include <chrono>
#include <source_location>

export module Rhodo.Debug:Profiling;

import :Config;
import Rhodo.Logger;

export namespace rhodo::debug {
class ScopedTimer {
 public:
  explicit ScopedTimer(const char* name,
                       const std::source_location& loc =
                           std::source_location::current()) noexcept
      : name_(name),
        loc_(loc),
        start_(std::chrono::high_resolution_clock::now()) {}

  ~ScopedTimer() noexcept {
    if constexpr (kIsDebug) {
      auto end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start_);

      logger::CoreLogger::Debug("[TIMER] {}: {} μs (at {}:{})", name_,
                                duration.count(), loc_.file_name(),
                                loc_.line());
    }
  }

  ScopedTimer(const ScopedTimer&) = delete;
  auto operator=(const ScopedTimer&) -> ScopedTimer& = delete;
  ScopedTimer(ScopedTimer&&) noexcept = default;
  auto operator=(ScopedTimer&&) noexcept -> ScopedTimer& = default;

 private:
  const char* name_;
  std::source_location loc_;
  std::chrono::high_resolution_clock::time_point start_;
};

}  // namespace rhodo::debug
