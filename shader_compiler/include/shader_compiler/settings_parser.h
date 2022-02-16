
#pragma once

#include <shader_compiler/defines.h>


typedef struct settings_parser_callbacks_t
{
	void* user_data;
	void (*category)(const char* str, u32 length, void* user_data);
	void (*field)(const char* str, u32 length, const char* value, u32 value_length, void* user_data);
} settings_parser_callbacks_t;


/*
	description:
	
	params:
	str: either null terminated string, otherwise length would be considered
	length: length of the string if the string is not null terminated
 */
SC_API void settings_parse(const char* str, u32 length, settings_parser_callbacks_t* callbacks);
