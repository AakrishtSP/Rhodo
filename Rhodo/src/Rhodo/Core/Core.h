#pragma once

#include "Rhodo/Core/PlatformDetection.h"

//-----------Export/Import Macro-----------
#ifdef RH_PLATFORM_WINDOWS
#ifdef RH_BUILD_SHARED
#define RHODO_API __declspec(dllexport)
#else
#define RHODO_API __declspec(dllimport)
#endif
#else
#define RHODO_API __attribute__((visibility("default")))
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

//-----------Bitwise Operations-----------
#define RH_BIT_LEFT_SHIFT(x) (1 << x)
#define RH_BIT_RIGHT_SHIFT(x) (1 >> x)
#define RH_BIT_CHECK(x, y) (x & RH_EXPAND_MACRO(RH_BIT_LEFT_SHIFT(y)))
#define RH_BIT_SET(x, y) (x |= RH_EXPAND_MACRO(RH_BIT_LEFT_SHIFT(y)))
#define RH_BIT_CLEAR(x, y) (x &= ~RH_EXPAND_MACRO(RH_BIT_LEFT_SHIFT(y)))
#define RH_BIT_TOGGLE(x, y) (x ^= RH_EXPAND_MACRO(RH_BIT_LEFT_SHIFT(y)))
//-----------------------------------------
