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
void DebugCheck(bool condition, const char* expr,
                const std::source_location& loc     = std::source_location::current(),
                const char*                 message = nullptr) noexcept;

// Verify - reports but doesn't abort (always enabled)
void VerifyCheck(bool condition, const char* expr,
                 const std::source_location& loc     = std::source_location::current(),
                 const char*                 message = nullptr) noexcept;

// Ensure - like to verify but for critical runtime checks
void EnsureCheck(bool condition, const char* expr,
                 const std::source_location& loc     = std::source_location::current(),
                 const char*                 message = nullptr) noexcept;

// Contract programming support
void Precondition(bool condition, const char* expr,
                  const std::source_location& loc     = std::source_location::current(),
                  const char*                 message = nullptr) noexcept;

void Postcondition(bool condition, const char* expr,
                   const std::source_location& loc     = std::source_location::current(),
                   const char*                 message = nullptr) noexcept;

void Invariant(bool condition, const char* expr,
               const std::source_location& loc     = std::source_location::current(),
               const char*                 message = nullptr) noexcept;

// Unconditional panic - always aborts
[[noreturn]] void Panic(const char*                 message,
                        const std::source_location& loc = std::source_location::current()) noexcept;

// ============ Handler Management ============
void SetPanicHandler(handlerT h) noexcept;
void SetReportHandler(handlerT h) noexcept;

handlerT GetPanicHandler() noexcept;
handlerT GetReportHandler() noexcept;

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

      Logger::coreLogger::debug("[TIMER] {}: {} μs (at {}:{})", name_, duration.count(),
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

  void Dismiss() noexcept { active = false; }

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
  static void RecordAllocation(
      void* /*ptr*/, size_t size,
      const std::source_location& /*loc*/ = std::source_location::current()) noexcept {
    if constexpr (kIsDebug) {
      g_total_allocated.fetch_add(size, std::memory_order_relaxed);
      g_allocation_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  static void RecordDeallocation(void* /*ptr*/, size_t size) noexcept {
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

  static void Reset() noexcept {
    g_total_allocated.store(0, std::memory_order_relaxed);
    g_total_freed.store(0, std::memory_order_relaxed);
    g_allocation_count.store(0, std::memory_order_relaxed);
    g_deallocation_count.store(0, std::memory_order_relaxed);
  }

  static void PrintStats() noexcept {
    Logger::coreLogger::info(
        "[MEMORY] Allocated: {} bytes ({} allocs), "
        "Freed: {} bytes ({} frees), Net: {} bytes",
        totalAllocated(), allocationCount(), totalFreed(), deallocationCount(), netAllocated());
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
static inline handlerT g_g_panic_handler  = nullptr;
static inline handlerT g_g_report_handler = nullptr;
}   // namespace rhodo::asserts::detail
namespace rhodo::asserts {

// ===== Default Handlers =====

static void DefaultPanicHandler(const char* expr, const std::source_location& loc,
                                const char* message) noexcept {
  Logger::coreLogger::critical(
      "\n╔═══════════════════════════════════════════════════════════════╗\n"
      "║                        RHODO PANIC                            ║\n"
      "╚═══════════════════════════════════════════════════════════════╝\n"
      "Expression: {}\n"
      "Location:   {}:{}\n"
      "Function:   {}\n"
      "{}{}",
      expr ? expr : "(null)", loc.file_name(), loc.line(), loc.function_name(),
      message ? "Message:    " : "", message ? message : "");

  Logger::coreLogger::flush();
  std::abort();
}

static void DefaultReportHandler(const char* expr, const std::source_location& loc,
                                 const char* message) noexcept {
  Logger::coreLogger::error("[VERIFY FAILED] {} at {}:{}{}{}", expr ? expr : "(null)",
                            loc.file_name(), loc.line(), message ? " - " : "",
                            message ? message : "");
}

static inline void EnsureDefaultsInstalled() noexcept {
  if (!Detail::gPanicHandler) {
    detail::g_g_panic_handler = &DefaultPanicHandler;
  }
  if (!Detail::gReportHandler) {
    detail::g_g_report_handler = &DefaultReportHandler;
  }
}

// ===== Handler Management =====

static void SetPanicHandler(handlerT h) noexcept {
  EnsureDefaultsInstalled();
  detail::g_g_panic_handler = h ? h : &DefaultPanicHandler;
}

static void SetReportHandler(handlerT h) noexcept {
  EnsureDefaultsInstalled();
  detail::g_g_report_handler = h ? h : &DefaultReportHandler;
}

static handlerT GetPanicHandler() noexcept {
  EnsureDefaultsInstalled();
  return Detail::gPanicHandler;
}

static handlerT GetReportHandler() noexcept {
  EnsureDefaultsInstalled();
  return Detail::gReportHandler;
}

// ===== Core Assertions =====

static void DebugCheck(const bool kCondition, const char* expr, const std::source_location& loc,
                       const char* message) noexcept {
  if constexpr (!isDebug) {
    (void)kCondition;
    (void)expr;
    (void)loc;
    (void)message;
  } else {
    if (!kCondition) {
      EnsureDefaultsInstalled();
      if (detail::g_g_panic_handler) {
        detail::g_g_panic_handler(expr, loc, message);
      }
      std::abort();
    }
  }
}

static void VerifyCheck(const bool kCondition, const char* expr, const std::source_location& loc,
                        const char* message) noexcept {
  if (!kCondition) {
    EnsureDefaultsInstalled();
    if (detail::g_g_report_handler) {
      detail::g_g_report_handler(expr, loc, message);
    }
  }
}

static void EnsureCheck(const bool kCondition, const char* expr, const std::source_location& loc,
                        const char* message) noexcept {
  if (!kCondition) {
    EnsureDefaultsInstalled();
    if (detail::g_g_panic_handler) {
      detail::g_g_panic_handler(expr, loc, message);
    }
    std::abort();
  }
}

static void Precondition(const bool kCondition, const char* expr, const std::source_location& loc,
                         const char* message) noexcept {
  DebugCheck(kCondition, expr, loc, message);
}

static void Postcondition(const bool kCondition, const char* expr, const std::source_location& loc,
                          const char* message) noexcept {
  DebugCheck(kCondition, expr, loc, message);
}

static void Invariant(const bool kCondition, const char* expr, const std::source_location& loc,
                      const char* message) noexcept {
  DebugCheck(kCondition, expr, loc, message);
}

[[noreturn]] static void Panic(const char* message, const std::source_location& loc) noexcept {
  EnsureDefaultsInstalled();
  if (detail::g_g_panic_handler) {
    detail::g_g_panic_handler("panic()", loc, message);
  }
  std::abort();
}

}   // namespace rhodo::asserts
