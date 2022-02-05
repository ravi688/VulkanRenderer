
#pragma once

#include <template_system.h>
#include <renderer/defines.h>

#define memory_allocator_array(T) template(memory_allocator_array, T)
#define instantiate_declaration_memory_allocator_array(T) RENDERER_API T* memory_allocator_array(T)(void* buffer, u64 count, ...)
#define instantiate_implementation_memory_allocator_array(T) RENDERER_API T* memory_allocator_array(T)(void* buffer, u64 count, ...)\
{\
	va_list args;\
	va_start(args, count);\
	T* _buffer = (T*)buffer;\
	u64 i = 0;\
	while(count > 0)\
	{\
		_buffer[i] = va_arg(args, T);\
		i++;\
		count--;\
	}\
	va_end(args);\
	return buffer;\
}

