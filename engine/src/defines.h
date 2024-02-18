#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) 
#define PLATFORM_WINDOWS 1
#include "Platform\Windows\WindowsPlatform.h"
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#endif
