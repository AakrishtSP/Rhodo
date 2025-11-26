#pragma once

//-----------Windows-----------

#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
/* Windows x64  */
#define RH_PLATFORM_WINDOWS
#else
/* Windows x86 */
#error "x86 Builds are not supported!"
#endif

//-----------Apple-----------
#elif defined(__APPLE__) || defined(__MACH__)
#define RH_PLATFORM_APPLE
#error "Unknown Apple platform!"

//-----------Andriod-----------

/* We have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define RH_PLATFORM_ANDROID
#error "Android is not supported!"

//-----------Linux-----------

#elif defined(__linux__)
#define RH_PLATFORM_LINUX
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif

//-----------------------------------------
