
#pragma once

#include <template_system.h>
#include <no_compile_header.h>
#include <garbage_collector/garbage_collector.h>
#include <stdint.h>
#include <stdarg.h>

#define array_t(T) template(array_t, T)

#define array(T) template(array, T)

#define instantiate_array_struct(T)\
typedef struct array_t(T)\
{\
	uint32_t count;\
	T* data;\
} array_t(T)

#define instantiate_declaration_array(T) array_t(T) array(T)(uint32_t, ...)
#define instantiate_implementation_array(T)\
array_t(T) array(T)(uint32_t count, ...)\
{\
	T* buffer = GC_NEWV(T, count);\
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

