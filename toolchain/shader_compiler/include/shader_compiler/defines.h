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

#include <stdlib.h>
#include <string.h>
#define new(type) (type*)_new(sizeof(type))

static void* _new(u32 size)
{
	void* ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}
