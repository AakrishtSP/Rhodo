module;
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <source_location>
#include <type_traits>
#include <utility>

export module Rhodo.Debug;

import Rhodo.Logger;

export namespace rhodo::asserts {

// ============ Build Configuration ============
#ifdef RHODO_DEBUG
inline constexpr bool isDebug = true;
#else
inline constexpr bool kIsDebug = false;
#endif

// ============ Handler Types ============
using handlerT = void (*)(const char* expr, const std::source_location& loc,
                          const char* message) noexcept;

// ============ Core Assertion Functions ============

// Debug assertions - stripped in release builds
auto DebugCheck(bool condition, const char* expr,
                const std::source_location& loc     = std::source_location::current(),
                const char*                 message = nullptr) noexcept -> void;

// Verify - reports but doesn't abort (always enabled)
auto VerifyCheck(bool condition, const char* expr,
                 const std::source_location& loc     = std::source_location::current(),
                 const char*                 message = nullptr) noexcept -> void;

// Ensure - like to verify but for critical runtime checks
auto EnsureCheck(bool condition, const char* expr,
                 const std::source_location& loc     = std::source_location::current(),
                 const char*                 message = nullptr) noexcept -> void;

// Contract programming support
auto Precondition(bool condition, const char* expr,
                  const std::source_location& loc     = std::source_location::current(),
                  const char*                 message = nullptr) noexcept -> void;

auto Postcondition(bool condition, const char* expr,
                   const std::source_location& loc     = std::source_location::current(),
                   const char*                 message = nullptr) noexcept -> void;

auto Invariant(bool condition, const char* expr,
               const std::source_location& loc     = std::source_location::current(),
               const char*                 message = nullptr) noexcept -> void;

// Unconditional panic - always aborts
[[noreturn]] auto Panic(const char*                 message,
                        const std::source_location& loc = std::source_location::current()) noexcept
    -> void;

// ============ Handler Management ============
auto SetPanicHandler(handlerT h) noexcept -> void;
auto SetReportHandler(handlerT h) noexcept -> void;

auto GetPanicHandler() noexcept -> handlerT;
auto GetReportHandler() noexcept -> handlerT;

// ============ Performance Profiling ============
class ScopedTimer {
 public:
  explicit ScopedTimer(const char*                 name,
                       const std::source_location& loc = std::source_location::current()) noexcept
      : name_(name), loc_(loc), start_(std::chrono::high_resolution_clock::now()) {}

  ~ScopedTimer() noexcept {
    if constexpr (kIsDebug) {
      auto end      = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);

      logger::CoreLogger::Debug("[TIMER] {}: {} μs (at {}:{})", name_, duration.count(),
                                loc_.file_name(), loc_.line());
    }
  }

  ScopedTimer(const ScopedTimer&)                    = delete;
  auto operator=(const ScopedTimer&) -> ScopedTimer& = delete;

 private:
  const char*                                    name_;
  std::source_location                           loc_;
  std::chrono::high_resolution_clock::time_point start_;
};

// ============ Scope Guards ============
template <typename F>
struct ScopeExit {
  F    fn;
  bool active = true;

  explicit ScopeExit(F f) noexcept(std::is_nothrow_move_constructible_v<F>) : fn(std::move(f)) {}

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

  ScopeExit(const ScopeExit&)                    = delete;
  auto operator=(const ScopeExit&) -> ScopeExit& = delete;
  auto operator=(ScopeExit&&) -> ScopeExit&      = delete;
};

template <typename F>
ScopeExit(F) -> ScopeExit<F>;

template <typename F>
struct ScopeFail {
  F   fn;
  int exceptions_on_entry;

  explicit ScopeFail(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(f)), exceptions_on_entry(std::uncaught_exceptions()) {}

  ~ScopeFail() noexcept {
    if (std::uncaught_exceptions() > exceptions_on_entry) {
      fn();
    }
  }

  ScopeFail(const ScopeFail&)                    = delete;
  auto operator=(const ScopeFail&) -> ScopeFail& = delete;
};

template <typename F>
ScopeFail(F) -> ScopeFail<F>;

template <typename F>
struct ScopeSuccess {
  F   fn;
  int exceptions_on_entry;

  explicit ScopeSuccess(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
      : fn(std::move(f)), exceptions_on_entry(std::uncaught_exceptions()) {}

  ~ScopeSuccess() noexcept(noexcept(fn())) {
    if (std::uncaught_exceptions() <= exceptions_on_entry) {
      fn();
    }
  }

  ScopeSuccess(const ScopeSuccess&)                    = delete;
  auto operator=(const ScopeSuccess&) -> ScopeSuccess& = delete;
};

template <typename F>
ScopeSuccess(F) -> ScopeSuccess<F>;

// ============ Memory Debugging ============
class MemoryTracker {
 public:
  static auto RecordAllocation(
      void* /*ptr*/, size_t size,
      const std::source_location& /*loc*/ = std::source_location::current()) noexcept -> void {
    if constexpr (kIsDebug) {
      g_total_allocated.fetch_add(size, std::memory_order_relaxed);
      g_allocation_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  static auto RecordDeallocation(void* /*ptr*/, size_t size) noexcept -> void {
    if constexpr (kIsDebug) {
      g_total_freed.fetch_add(size, std::memory_order_relaxed);
      g_deallocation_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  static auto TotalAllocated() noexcept -> size_t {
    return g_total_allocated.load(std::memory_order_relaxed);
  }

  static auto TotalFreed() noexcept -> size_t {
    return g_total_freed.load(std::memory_order_relaxed);
  }

  static auto NetAllocated() noexcept -> size_t { return TotalAllocated() - TotalFreed(); }

  static auto AllocationCount() noexcept -> size_t {
    return g_allocation_count.load(std::memory_order_relaxed);
  }

  static auto DeallocationCount() noexcept -> size_t {
    return g_deallocation_count.load(std::memory_order_relaxed);
  }

  static auto Reset() noexcept -> void {
    g_total_allocated.store(0, std::memory_order_relaxed);
    g_total_freed.store(0, std::memory_order_relaxed);
    g_allocation_count.store(0, std::memory_order_relaxed);
    g_deallocation_count.store(0, std::memory_order_relaxed);
  }

  static auto PrintStats() noexcept -> void {
    logger::CoreLogger::Info(
        "[MEMORY] Allocated: {} bytes ({} allocs), "
        "Freed: {} bytes ({} frees), Net: {} bytes",
        TotalAllocated(), AllocationCount(), TotalFreed(), DeallocationCount(), NetAllocated());
  }

 private:
  static inline std::atomic<size_t> g_total_allocated{0};
  static inline std::atomic<size_t> g_total_freed{0};
  static inline std::atomic<size_t> g_allocation_count{0};
  static inline std::atomic<size_t> g_deallocation_count{0};
};

}   // namespace rhodo::asserts

// ==================== IMPLEMENTATION ====================

namespace rhodo::asserts::detail {
static inline handlerT g_panic_handler  = nullptr;
static inline handlerT g_report_handler = nullptr;
}   // namespace rhodo::asserts::detail
namespace rhodo::asserts {

// ===== Default Handlers =====

auto DefaultPanicHandler(const char* expr, const std::source_location& loc,
                         const char* message) noexcept -> void {
  logger::CoreLogger::Critical(
      "\n╔═══════════════════════════════════════════════════════════════╗\n"
      "║                        RHODO PANIC                            ║\n"
      "╚═══════════════════════════════════════════════════════════════╝\n"
      "Expression: {}\n"
      "Location:   {}:{}\n"
      "Function:   {}\n"
      "{}{}",
      expr ? expr : "(null)", loc.file_name(), loc.line(), loc.function_name(),
      message ? "Message:    " : "", message ? message : "");

  logger::CoreLogger::Flush();
  std::abort();
}

auto DefaultReportHandler(const char* expr, const std::source_location& loc,
                          const char* message) noexcept -> void {
  logger::CoreLogger::Error("[VERIFY FAILED] {} at {}:{}{}{}", expr ? expr : "(null)",
                            loc.file_name(), loc.line(), message ? " - " : "",
                            message ? message : "");
}

inline auto EnsureDefaultsInstalled() noexcept -> void {
  if (!detail::g_panic_handler) {
    detail::g_panic_handler = &DefaultPanicHandler;
  }
  if (!detail::g_report_handler) {
    detail::g_report_handler = &DefaultReportHandler;
  }
}

// ===== Handler Management =====

auto SetPanicHandler(handlerT h) noexcept -> void {
  EnsureDefaultsInstalled();
  detail::g_panic_handler = h ? h : &DefaultPanicHandler;
}

auto SetReportHandler(const handlerT h) noexcept -> void {
  EnsureDefaultsInstalled();
  detail::g_report_handler = h ? h : &DefaultReportHandler;
}

auto GetPanicHandler() noexcept -> handlerT {
  EnsureDefaultsInstalled();
  return detail::g_panic_handler;
}

auto GetReportHandler() noexcept -> handlerT {
  EnsureDefaultsInstalled();
  return detail::g_report_handler;
}

// ===== Core Assertions =====

void DebugCheck(const bool condition, const char* expr, const std::source_location& loc,
                const char* message) noexcept {
  if constexpr (!kIsDebug) {
    (void)condition;
    (void)expr;
    (void)loc;
    (void)message;
  } else {
    if (!condition) {
      EnsureDefaultsInstalled();
      if (detail::g_panic_handler) {
        detail::g_panic_handler(expr, loc, message);
      }
      std::abort();
    }
  }
}

auto VerifyCheck(const bool condition, const char* expr, const std::source_location& loc,
                 const char* message) noexcept -> void {
  if (!condition) {
    EnsureDefaultsInstalled();
    if (detail::g_report_handler) {
      detail::g_report_handler(expr, loc, message);
    }
  }
}

void EnsureCheck(const bool condition, const char* expr, const std::source_location& loc,
                 const char* message) noexcept {
  if (!condition) {
    EnsureDefaultsInstalled();
    if (detail::g_panic_handler) {
      detail::g_panic_handler(expr, loc, message);
    }
    std::abort();
  }
}

auto Precondition(const bool condition, const char* expr, const std::source_location& loc,
                  const char* message) noexcept -> void {
  DebugCheck(condition, expr, loc, message);
}

auto Postcondition(const bool condition, const char* expr, const std::source_location& loc,
                   const char* message) noexcept -> void {
  DebugCheck(condition, expr, loc, message);
}

auto Invariant(const bool condition, const char* expr, const std::source_location& loc,
               const char* message) noexcept -> void {
  DebugCheck(condition, expr, loc, message);
}

[[noreturn]] auto Panic(const char* message, const std::source_location& loc) noexcept -> void {
  EnsureDefaultsInstalled();
  if (detail::g_panic_handler) {
    detail::g_panic_handler("panic()", loc, message);
  }
  std::abort();
}

}   // namespace rhodo::asserts
