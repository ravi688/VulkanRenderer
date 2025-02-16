#pragma once

#if (defined _WIN32 || defined __CYGWIN__) && defined(__GNUC__)
#	define SC_IMPORT_API __declspec(dllimport)
#	define SC_EXPORT_API __declspec(dllexport)
#else
#	define SC_IMPORT_API __attribute__((visibility("default")))
#	define SC_EXPORT_API __attribute__((visibility("default")))
#endif

#ifdef SC_BUILD_STATIC_LIBRARY
#	define SC_API
#elif defined(SC_BUILD_DYNAMIC_LIBRARY)
#	define SC_API SC_EXPORT_API
#elif defined(SC_USE_DYNAMIC_LIBRARY)
#	define SC_API SC_IMPORT_API
#elif defined(SC_USE_STATIC_LIBRARY)
#	define SC_API
#else
#	define SC_API
#endif
