module;
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <functional>
#include <source_location>
#include <type_traits>
#include <utility>

export module Rhodo.Debug;

import Rhodo.Logger;

export import :Config;

export namespace rhodo::debug {}  // namespace rhodo::debug

// ==================== IMPLEMENTATION ====================

namespace rhodo::debug::detail {
static inline HandlerT g_panic_handler = nullptr;
static inline HandlerT g_report_handler = nullptr;
}  // namespace rhodo::debug::detail
namespace rhodo::debug {

// ===== Default Handlers =====

auto static DefaultPanicHandler(const char* expr,
                                const std::source_location& loc,
                                const char* message) noexcept -> void {
  logger::CoreLogger::Critical(
      "\n╔═══════════════════════════════════════════════════════════════╗\n"
      "║                        RHODO PANIC                            ║\n"
      "╚═══════════════════════════════════════════════════════════════╝\n"
      "Expression: {}\n"
      "Location:   {}:{}\n"
      "Function:   {}\n"
      "{}{}",
      static_cast<bool>(expr) ? expr : "(null)", loc.file_name(), loc.line(),
      loc.function_name(), static_cast<bool>(message) ? "Message:    " : "",
      static_cast<bool>(message) ? message : "");

  logger::CoreLogger::Flush();
  std::abort();
}

auto static DefaultReportHandler(const char* expr,
                                 const std::source_location& loc,
                                 const char* message) noexcept -> void {
  logger::CoreLogger::Error("[VERIFY FAILED] {} at {}:{}{}{}",
                            static_cast<bool>(expr) ? expr : "(null)",
                            loc.file_name(), loc.line(),
                            static_cast<bool>(message) ? " - " : "",
                            static_cast<bool>(message) ? message : "");
}

inline static auto EnsureDefaultsInstalled() noexcept -> void {
  if (!detail::g_panic_handler) {
    detail::g_panic_handler = &DefaultPanicHandler;
  }
  if (!detail::g_report_handler) {
    detail::g_report_handler = &DefaultReportHandler;
  }
}

// ===== Handler Management =====

export auto SetPanicHandler(const HandlerT& h) noexcept -> void {
  EnsureDefaultsInstalled();
  detail::g_panic_handler = h ? h : &DefaultPanicHandler;
}

export auto SetReportHandler(const HandlerT& h) noexcept -> void {
  EnsureDefaultsInstalled();
  detail::g_report_handler = h ? h : &DefaultReportHandler;
}

auto GetPanicHandler() noexcept -> HandlerT {
  EnsureDefaultsInstalled();
  return detail::g_panic_handler;
}

auto GetReportHandler() noexcept -> HandlerT {
  EnsureDefaultsInstalled();
  return detail::g_report_handler;
}

// ===== Core Assertions =====

void DebugCheck(const bool condition, const char* expr,
                const std::source_location& loc, const char* message) noexcept {
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

auto VerifyCheck(const bool condition, const char* expr,
                 const std::source_location& loc, const char* message) noexcept
    -> void {
  if (!condition) {
    EnsureDefaultsInstalled();
    if (detail::g_report_handler) {
      detail::g_report_handler(expr, loc, message);
    }
  }
}

void EnsureCheck(const bool condition, const char* expr,
                 const std::source_location& loc,
                 const char* message) noexcept {
  if (!condition) {
    EnsureDefaultsInstalled();
    if (detail::g_panic_handler) {
      detail::g_panic_handler(expr, loc, message);
    }
    std::abort();
  }
}

auto Precondition(const bool condition, const char* expr,
                  const std::source_location& loc, const char* message) noexcept
    -> void {
  DebugCheck(condition, expr, loc, message);
}

auto Postcondition(const bool condition, const char* expr,
                   const std::source_location& loc,
                   const char* message) noexcept -> void {
  DebugCheck(condition, expr, loc, message);
}

auto Invariant(const bool condition, const char* expr,
               const std::source_location& loc, const char* message) noexcept
    -> void {
  DebugCheck(condition, expr, loc, message);
}

[[noreturn]] auto Panic(const char* message,
                        const std::source_location& loc) noexcept -> void {
  EnsureDefaultsInstalled();
  if (detail::g_panic_handler) {
    detail::g_panic_handler("panic()", loc, message);
  }
  std::abort();
}

}  // namespace rhodo::debug
