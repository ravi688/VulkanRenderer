
#pragma once

#include <memory_allocator/template_definitions.h>

#include <defines.h>


instantiate_declaration_memory_allocator_array(u8);
instantiate_declaration_memory_allocator_array(u16);
instantiate_declaration_memory_allocator_array(u32);
instantiate_declaration_memory_allocator_array(u64);
instantiate_declaration_memory_allocator_array(s8);
instantiate_declaration_memory_allocator_array(s16);
instantiate_declaration_memory_allocator_array(s32);
instantiate_declaration_memory_allocator_array(s64);
instantiate_declaration_memory_allocator_array(float);
instantiate_declaration_memory_allocator_array(double);
instantiate_declaration_memory_allocator_array(char);

#ifdef MEMORY_ALLOCATOR_IMPLEMENTATION
instantiate_implementation_memory_allocator_array(double);
instantiate_implementation_memory_allocator_array(u64);
instantiate_implementation_memory_allocator_array(s64);

float* memory_allocator_array(float)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	float* _buffer = (float*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, double);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

char* memory_allocator_array(char)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	char* _buffer = (char*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

u8* memory_allocator_array(u8)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u8* _buffer = (u8*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}
u16* memory_allocator_array(u16)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u16* _buffer = (u16*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

u32* memory_allocator_array(u32)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u32* _buffer = (u32*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s8* memory_allocator_array(s8)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s8* _buffer = (s8*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s16* memory_allocator_array(s16)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s16* _buffer = (s16*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s32* memory_allocator_array(s32)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s32* _buffer = (s32*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}
#endif


