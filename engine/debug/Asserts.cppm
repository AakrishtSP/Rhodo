module;
#include <functional>
#include <source_location>

export module Rhodo.Debug:Asserts;

import Rhodo.Logger;
import :Config;
namespace rhodo::debug {

// Debug assertions - stripped in release builds
auto DebugCheck(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

// Verify - reports but doesn't abort (always enabled)
auto VerifyCheck(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

// Ensure - like to verify but for critical runtime checks
auto EnsureCheck(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

// Contract programming support
auto Precondition(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

auto Postcondition(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

auto Invariant(
    bool condition, const char* expr,
    const std::source_location& loc = std::source_location::current(),
    const char* message = nullptr) noexcept -> void;

// Unconditional panic - always aborts
[[noreturn]] auto Panic(const char* message,
                        const std::source_location& loc =
                            std::source_location::current()) noexcept -> void;

// ============ Handler Management ============
auto SetPanicHandler(HandlerT h) noexcept -> void;
auto SetReportHandler(const HandlerT& h) noexcept -> void;

auto GetPanicHandler() noexcept -> HandlerT;
auto GetReportHandler() noexcept -> HandlerT;

}  // namespace rhodo::debug
