module;
#include <cstdio>
#include <cstdlib>
#include <print>
#include <source_location>
#include <string_view>

export module Rhodo.Core:Asserts;

export namespace rhodo {

// Hook for custom assertion handling
using AssertHandlerFn = void(*)(std::string_view condition, std::string_view message, const std::source_location& loc);

inline AssertHandlerFn g_assert_handler = nullptr;

inline void SetAssertHandler(AssertHandlerFn handler) {
    g_assert_handler = handler;
}

// Internal default handler
inline void DefaultAssertHandler(std::string_view condition, std::string_view message, const std::source_location& loc) {
    std::println(stderr, "Assertion Failed: {}\nMessage: {}\nFile: {}:{}\nFunction: {}",
        condition.data(), message.data(), loc.file_name(), loc.line(), loc.function_name());
    std::abort();
}

inline void TriggerAssert(std::string_view condition, std::string_view message,
                          const std::source_location& loc) {
    if (g_assert_handler) {
        g_assert_handler(condition, message, loc);
    } else {
        DefaultAssertHandler(condition, message, loc);
    }
}

// Assert: Only checks in Debug builds (or when RH_ENABLE_ASSERTS is defined)
inline void Assert(bool condition, std::string_view message = "",
                   const std::source_location& loc = std::source_location::current()) {
#ifdef RH_ENABLE_ASSERTS
    if (!condition) [[unlikely]] {
        TriggerAssert("Assertion Failed", message, loc);
    }
#endif
}

// Verify: Always checks, but only asserts/crashes on failure
// Useful for things that have side effects or must be checked in Release too
inline void Verify(bool condition, std::string_view message = "",
                   const std::source_location& loc = std::source_location::current()) {
    if (!condition) [[unlikely]] {
        TriggerAssert("Verification Failed", message, loc);
    }
}

// Unreachable: Marks code path as unreachable
[[noreturn]] inline void Unreachable(std::string_view message = "",
                                     const std::source_location& loc = std::source_location::current()) {
    TriggerAssert("Unreachable Code Reached", message, loc);
    std::abort(); // Compiler hint
}

} // namespace rhodo
