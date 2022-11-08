#pragma once

#include <common/defines.h>

#ifdef SC_STATIC_LIBRARY
#	define SC_API
#elif SC_DYNAMIC_LIBRARY
#	define SC_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SC_API __declspec(dllexport)
#else
#	define SC_API
#endif
