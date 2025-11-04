#pragma once


// ReSharper disable once CppUnusedIncludeDirective
#include "Rhodo/Core/PlatformDetection.h"
#include <memory>

//-----------Export/Import Macro-----------
#ifdef RH_PLATFORM_WINDOWS
#ifdef RH_BUILD_SHARED
#define RHODO_API __declspec(dllexport)
#elif RH_BUILD_SHARED
#define RHODO_API __declspec(dllimport)
#else
#define RHODO_API
#endif
#else
#define RHODO_API
// #define RHODO_API __attribute__((visibility("default")))
#endif
//-----------------------------------------

//-----------------Debug Break---------------
#ifdef RH_DEBUG
#if defined(RH_PLATFORM_WINDOWS)
// MSVC and MinGW on Windows
#if defined(_MSC_VER) || defined(__MINGW32__)
#define RH_DEBUGBREAK() __debugbreak()
#else
#error "Unsupported Windows compiler"
#endif
#elif defined(RH_PLATFORM_LINUX)
// GCC and Clang on Linux
#if defined(__GNUC__) || defined(__clang__)
#include <signal.h>
#define RH_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Unsupported Linux compiler"
#endif
#else
#error "Platform doesn't support debugbreak yet!"
#endif

// Enable assertions in debug mode
#define RH_ENABLE_ASSERTS
#else
#define RH_DEBUGBREAK()
#endif
//-----------------------------------------


//-----------Assert Macro-----------
#ifdef RH_ENABLE_ASSERTS
#define RH_ASSERT(x, ...) { if(!(x)) { RH_ERROR("Assertion Failed: {0}", __VA_ARGS__); RH_DEBUGBREAK(); } }
#define RH_CORE_ASSERT(x, ...) { if(!(x)) { RH_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); RH_DEBUGBREAK(); } }
#else
#define RH_ASSERT(x, ...)
#define RH_CORE_ASSERT(x, ...)
#endif
//-----------------------------------------

//-----------Essential Macros-----------
#define RH_EXPAND_MACRO(x) x
#define RH_STRINGIFY_MACRO(x) #x
#define RH_CONCAT_MACRO(a, b) a##b
//-----------------------------------------


//-----------Function Bind Macro-----------
#define RH_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

//-----------------------------------------
namespace Rhodo {
    template<typename T>
    using scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr scope<T> createScope(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr ref<T> createRef(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
