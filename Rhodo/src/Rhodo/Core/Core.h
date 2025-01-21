#pragma once

// For use by Rhodo applications

//-----------Platform Detection-----------
#if defined (_WIN64) || defined( __MINGW64__)
#define RH_PLATFORM_WINDOWS
#elif defined (__linux__)
#define RH_PLATFORM_LINUX
#else
#error "Rhodo only supports Windows and Linux!"
#endif
//---------------------------------------


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