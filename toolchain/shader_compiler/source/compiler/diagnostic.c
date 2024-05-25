/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: diagnostic.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

#include <shader_compiler/compiler/diagnostic.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>
#include <stdlib.h>
#include <string.h> 		// for memcpy

typedef struct arg_t
{
	void* ptr;
	u32 size;
} arg_t;

CAN_BE_UNUSED_FUNCTION static arg_t arg_create(void* ptr, u32 size)
{
	arg_t arg;
	arg.ptr = malloc(size);
	// TODO: create a safe version of memcpy, for example safe_memcpy()
	memcpy(arg.ptr, ptr, size);
	arg.size = size;
	return arg;
}

static void arg_destroy(arg_t* arg)
{
	free(arg->ptr);
	arg->ptr = NULL;
	arg->size = 0;
}

SC_API diagnostic_t* diagnostic_new()
{
	diagnostic_t* diagnostic = (diagnostic_t*)malloc(sizeof(diagnostic_t));
	memset(diagnostic, 0, sizeof(diagnostic_t));
	return diagnostic;
}

SC_API diagnostic_t* diagnostic_create()
{
	diagnostic_t* diagnostic = diagnostic_new();
	diagnostic->args = buf_new(arg_t);
	buf_set_on_free(&diagnostic->args, (void (*)(void*))arg_destroy);
	return diagnostic;
}

SC_API void diagnostic_release_resources(diagnostic_t* diagnostic)
{
	// frees all the resources allocated by each arg_t object but not the memory allocated for themselves.
	buf_clear(&diagnostic->args, NULL);
}

SC_API void diagnostic_destroy(diagnostic_t* diagnostic)
{
	// frees all the resources allocated by each arg_t object and the memory allocated for themselves as well.
	buf_free(&diagnostic->args);
}

SC_API void diagnostic_push_string(diagnostic_t* diagnostic, const char* value)
{
	diagnostic_push(diagnostic, value, strlen(value) + 1);
}

SC_API void diagnostic_push(diagnostic_t* diagnostic, const void* const ptr, u32 size)
{
	assert(buf_get_element_size(&diagnostic->args) == sizeof(u8));
	// buf_push(&diagnostic->args, (void*)ptr, size);
	// TODO
}

SC_API void __diagnostic_log(diagnostic_t* diagnostic, diagnostic_type_t type, diagnostic_message_t message, u32 __line__, const char* __function__, const char* __file__)
{
	const char* description = ""; //get_description(type);
	switch(message)
	{
		case DIAGNOSTIC_MESSAGE_FAILED_TO_OPEN_FILE:
			debug_log(description, __line__, __function__, __file__, "Failed to open file %s", CAST_TO(arg_t*, buf_peek_ptr(&diagnostic->args))->ptr);
			buf_pop(&diagnostic->args, NULL);
			break;
		case DIAGNOSTIC_MESSAGE_UNEXPECTED_END_OF_FILE:
			debug_log(description, __line__, __function__, __file__, "Unexpected end of file %s", CAST_TO(arg_t*, buf_peek_ptr(&diagnostic->args))->ptr);
			buf_pop(&diagnostic->args, NULL);
			break;
		case DIAGNOSTIC_MESSAGE_UNEXPECTED_SYMBOL:
			arg_t* unexpected = buf_pop_get_ptr(&diagnostic->args);
			arg_t* line_no = 	buf_pop_get_ptr(&diagnostic->args);
			arg_t* file = 		buf_pop_get_ptr(&diagnostic->args);
			arg_t* expected = 	buf_pop_get_ptr(&diagnostic->args);
			debug_log(description, __line__, __function__, __file__, "Unexpected symbol \"%s\" at line %ul in %s, expected %s",
				(const char*)unexpected->ptr, DEREF_TO(u32, line_no->ptr), (const char*)file->ptr, (const char*)expected->ptr);
			arg_destroy(unexpected);
			arg_destroy(line_no);
			arg_destroy(file);
			arg_destroy(expected);
			break;
		default:
			debug_log("[Error] ", __line__, __function__, __file__, "Unrecognized diagnostic_type_t \"%u", (u32)type);
	}
}
