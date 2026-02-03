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
inline auto set_emit_begin_hook(EmitBeginHookFn hook) noexcept -> void {
  g_emit_begin_hook = hook;
}

inline auto set_emit_end_hook(EmitEndHookFn hook) noexcept -> void {
  g_emit_end_hook = hook;
}

inline auto clear_hooks() noexcept -> void {
  g_emit_begin_hook = nullptr;
  g_emit_end_hook = nullptr;
}

// Helper for internal use
inline auto notify_emit_begin(
    const std::string_view name,
    const std::source_location& loc = std::source_location::current()) noexcept
    -> void {
  if (g_emit_begin_hook != nullptr) {
    g_emit_begin_hook(name, loc);
  }
}

inline auto notify_emit_end() noexcept -> void {
  if (g_emit_end_hook != nullptr) {
    g_emit_end_hook();
  }
}

} // namespace rhodo::core::signals
