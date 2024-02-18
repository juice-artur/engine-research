#pragma once
#include <Windows.h>

#ifdef JE_EXPORT
#ifdef _MSC_VER
#define JE_API __declspec(dllexport)
#else
#define JE_API __attribute__((visibility("default")))
#endif
#else

// Imports
#ifdef _MSC_VER
#define  JE_API __declspec(dllimport)
#else
#define  JE_API
#endif
#endif


#define FORCEINLINE __forceinline

typedef HANDLE WindowHandle_t;