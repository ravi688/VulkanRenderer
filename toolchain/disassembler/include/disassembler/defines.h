
#pragma once

#include <common/defines.h>

#ifdef DISASM_STATIC_LIBRARY
#	define DISASM_API
#elif DISASM_DYNAMIC_LIBRARY
#	define DISASM_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define DISASM_API __declspec(dllexport)
#else
#	define DISASM_API
#endif
