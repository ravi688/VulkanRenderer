#pragma once

#if (defined _WIN32 || defined __CYGWIN__) && defined(__GNUC__)
#	define DISASM_IMPORT_API __declspec(dllimport)
#	define DISASM_EXPORT_API __declspec(dllexport)
#else
#	define DISASM_IMPORT_API __attribute__((visibility("default")))
#	define DISASM_EXPORT_API __attribute__((visibility("default")))
#endif

#ifdef DISASM_BUILD_STATIC_LIBRARY
#	define DISASM_API
#elif defined(DISASM_BUILD_DYNAMIC_LIBRARY)
#	define DISASM_API DISASM_EXPORT_API
#elif defined(DISASM_USE_DYNAMIC_LIBRARY)
#	define DISASM_API DISASM_IMPORT_API
#elif defined(DISASM_USE_STATIC_LIBRARY)
#	define DISASM_API
#else
#	define DISASM_API
#endif
