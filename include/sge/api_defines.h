#pragma once

#if (defined _WIN32 || defined __CYGWIN__) && defined(__GNUC__)
#	define SGE_IMPORT_API __declspec(dllimport)
#	define SGE_EXPORT_API __declspec(dllexport)
#else
#	define SGE_IMPORT_API __attribute__((visibility("default")))
#	define SGE_EXPORT_API __attribute__((visibility("default")))
#endif

#ifdef SGE_BUILD_STATIC_LIBRARY
#	define SGE_API
#elif defined(SGE_BUILD_DYNAMIC_LIBRARY)
#	define SGE_API SGE_EXPORT_API
#elif defined(SGE_USE_DYNAMIC_LIBRARY)
#	define SGE_API SGE_IMPORT_API
#elif defined(SGE_USE_STATIC_LIBRARY)
#	define SGE_API
#else
#	define SGE_API
#endif
