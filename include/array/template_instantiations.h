
#pragma once

#include <array/template_definitions.h>

instantiate_array_struct(uint32_t);
instantiate_array_struct(int);
instantiate_array_struct(float);
instantiate_array_struct(char);

BEGIN_CPP_COMPATIBLE

RENDERER_API instantiate_declaration_array(uint32_t);
RENDERER_API instantiate_declaration_array(int);
RENDERER_API instantiate_declaration_array(float);
RENDERER_API instantiate_declaration_array(char);


#ifdef IMPLEMENTATION
RENDERER_API instantiate_implementation_array(uint32_t);
RENDERER_API instantiate_implementation_array(int);

RENDERER_API array_t(char) array(char)(uint32_t count, ...)
{
	char* buffer = heap_newv(char, count);
	va_list args;
	va_start(args, count);
	uint32_t i = 0;
	while(i < count)
	{
		buffer[i] = (char)va_arg(args, int);
		++i;
	}
	return (array_t(char)) { count, buffer };
}

RENDERER_API array_t(float) array(float)(uint32_t count, ...)
{
	float* buffer = heap_newv(float, count);
	va_list args;
	va_start(args, count);
	uint32_t i = 0;
	while(i < count)
	{
		buffer[i] = (float)va_arg(args, double);
		++i;
	}
	return (array_t(float)) { count, buffer };
}
#endif

END_CPP_COMPATIBLE
