module;
#include <cstdlib>
#include <print>
#include <source_location>
#include <string_view>

export module Rhodo.Core.Asserts;

export namespace rhodo::core::asserts {

// Hook for custom assertion handling
using AssertHandlerFn = void (*)(std::string_view condition,
                                 std::string_view message,
                                 const std::source_location& loc);

inline AssertHandlerFn g_assert_handler = nullptr;

inline void set_assert_handler(AssertHandlerFn handler) {
    g_assert_handler = handler;
}

// Internal default handler
inline void default_assert_handler(std::string_view condition,
                                   std::string_view message,
                                   const std::source_location& loc) {
    std::println(stderr,
                 "Assertion Failed: {}\nMessage: {}\nFile: {}:{}\nFunction: {}",
                 condition.data(), message.data(), loc.file_name(), loc.line(),
                 loc.function_name());
    std::abort();
}

inline void trigger_assert(std::string_view condition, std::string_view message,
                           const std::source_location& loc) {
    if (g_assert_handler != nullptr) {
        g_assert_handler(condition, message, loc);
    } else {
        default_assert_handler(condition, message, loc);
    }
}

// Assert: Only checks in Debug builds (or when RH_ENABLE_ASSERTS is defined)
inline void assert(
        bool condition, std::string_view message = "",
        const std::source_location& loc = std::source_location::current()) {
#ifdef RH_ENABLE_ASSERTS
    if (!condition) [[unlikely]] {
        TriggerAssert("Assertion Failed", message, loc);
    }
#endif
}

// Verify: Always checks, but only asserts/crashes on failure
// Useful for things that have side effects or must be checked in Release too
inline void verify(
        bool condition, std::string_view message = "",
        const std::source_location& loc = std::source_location::current()) {
    if (!condition) [[unlikely]] {
        trigger_assert("Verification Failed", message, loc);
    }
}

// Unreachable: Marks code path as unreachable
[[noreturn]] inline void unreachable(
        std::string_view message = "",
        const std::source_location& loc = std::source_location::current()) {
    trigger_assert("Unreachable Code Reached", message, loc);
    std::abort();  // Compiler hint
}

}  // namespace rhodo::core::asserts

export namespace asserts = ::rhodo::core::asserts;
