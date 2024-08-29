
#pragma once

#include <common/defines.h>

#ifdef CURIEAN_STATIC_LIBRARY
#	define CURIEAN_API
#elif CURIEAN_DYNAMIC_LIBRARY
#	define CURIEAN_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define CURIEAN_API __declspec(dllexport)
#else
#	define CURIEAN_API
#endif
