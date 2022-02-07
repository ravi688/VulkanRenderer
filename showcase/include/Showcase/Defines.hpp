
#pragma once

#ifdef SHOWCASE_STATIC_LIBRARY
#	define SHOWCASE_API
#elif SHOWCASE_DYNAMIC_LIBRARY
#	define SHOWCASE_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SHOWCASE_API __declspec(dllexport)
#else
#	define SHOWCASE_API
#endif
