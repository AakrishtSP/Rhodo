module;
#include <source_location>
#include <string_view>

export module Rhodo.Core.Signals:Hooks;

export namespace rhodo::core::signals {

// Hook Signatures
using EmitBeginHookFn = void (*)(std::string_view name, const std::source_location& loc) noexcept;
using EmitEndHookFn = void (*)() noexcept;

// Global Hook Storage
inline EmitBeginHookFn g_emit_begin_hook = nullptr;
inline EmitEndHookFn g_emit_end_hook = nullptr;

// Registration API
inline auto SetEmitBeginHook(EmitBeginHookFn hook) noexcept -> void {
    g_emit_begin_hook = hook;
}

inline auto SetEmitEndHook(EmitEndHookFn hook) noexcept -> void {
    g_emit_end_hook = hook;
}

inline auto ClearHooks() noexcept -> void {
    g_emit_begin_hook = nullptr;
    g_emit_end_hook = nullptr;
}

// Helper for internal use
inline auto NotifyEmitBegin(
    const std::string_view name,
                            const std::source_location& loc = std::source_location::current()) noexcept -> void {
    if (g_emit_begin_hook != nullptr) {
        g_emit_begin_hook(name, loc);
    }
}

inline auto NotifyEmitEnd() noexcept -> void {
    if (g_emit_end_hook != nullptr) {
        g_emit_end_hook();
    }
}

} // namespace rhodo::core::signals
