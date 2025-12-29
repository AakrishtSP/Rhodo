export module Rhodo.Memory:Hooks;

import :Categories;

import <cstddef>;
import <source_location>;

export namespace rhodo::memory {

// Hook Function Signatures
using AllocHookFn = void (*)(void* ptr, size_t size, MemoryCategory category,
                             const std::source_location& loc) noexcept;

using FreeHookFn = void (*)(void* ptr, size_t size,
                            MemoryCategory category) noexcept;

// Global Hook Storage (Function Pointers Only)
// These are intentionally nullable - hooks are OPTIONAL
inline AllocHookFn g_alloc_hook = nullptr;
inline FreeHookFn g_free_hook = nullptr;

// Hook Registration API
inline auto SetAllocationHook(AllocHookFn hook) noexcept -> void {
  g_alloc_hook = hook;
}

inline auto SetDeallocationHook(FreeHookFn hook) noexcept -> void {
  g_free_hook = hook;
}

inline auto ClearHooks() noexcept -> void {
  g_alloc_hook = nullptr;
  g_free_hook = nullptr;
}

// Hook Invocation Helpers (Used by Allocators)
inline auto NotifyAllocation(
    void* ptr, const size_t size, const MemoryCategory category,
    const std::source_location& loc = std::source_location::current()) noexcept
    -> void {
  if (g_alloc_hook != nullptr) [[likely]] {
    g_alloc_hook(ptr, size, category, loc);
  }
}

inline auto NotifyDeallocation(void* ptr, const size_t size,
                               const MemoryCategory category) noexcept -> void {
  if (g_free_hook != nullptr) [[likely]] {
    g_free_hook(ptr, size, category);
  }
}

}  // namespace rhodo::memory
