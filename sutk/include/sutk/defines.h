
#pragma once

#include <common/defines.h>

#ifdef SUTK_STATIC_LIBRARY
#	define SUTK_API
#elif SUTK_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SUTK_API __declspec(dllexport)
#else
#	define SUTK_API
#endif
