
#pragma once

#include <template_system.h>
#include <no_compile_header.h>
#include <stdint.h>
#include <stdarg.h>
#include <renderer/defines.h>

#define array_t(T) c_template(array_t, T)

#define array(T) c_template(array, T)

#define instantiate_array_struct(T)\
typedef struct array_t(T)\
{\
	uint32_t count;\
	T* data;\
} array_t(T)

#define instantiate_declaration_array(T) RENDERER_API array_t(T) array(T)(uint32_t, ...)
#define instantiate_implementation_array(T)\
RENDERER_API array_t(T) array(T)(uint32_t count, ...)\
{\
	T* buffer = heap_newv(T, count);\
	va_list args;\
	va_start(args, count);\
	uint32_t i = 0;\
	while(i < count)\
	{\
		buffer[i] = va_arg(args, T);\
		++i;\
	}\
	return (array_t(T)) { count, buffer };\
}

