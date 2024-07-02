/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string_builder.h is a part of VulkanRenderer

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

#pragma once


#include <sge/defines.h>
#include <bufferlib/buffer.h>

/* string builder is used to build string efficiently with convenience */
typedef struct string_builder_t
{
	memory_allocator_t* allocator;
	/* stores the main string buffer */
	BUFFER string_buffer;
	/* stores the list of tab character ('\t') */
	BUFFER indentation_buffer;
} string_builder_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API string_builder_t* string_builder_create(memory_allocator_t* allocator, u32 capacity);
SGE_API void string_builder_destroy(string_builder_t* builder);

/* returns the internal string buffer 
 * NOTE: if you want null terminated string, then explicitly call string_builder_append_null(), 
 * or use string_builder_get_str_null() which guarantees null termination regardless of calling string_builder_append_null() */
SGE_API char* string_builder_get_str(string_builder_t* builder);
/* returns the internal string buffer, but guaranteed null terminataion */
SGE_API char* string_builder_get_str_null(string_builder_t* builder);

/* appends the formatted string to the internal character buffer considering the current level of indentation */
SGE_API void string_builder_append(string_builder_t* builder, const char* const format, ...);
/* stiches the formatted string to the internal character buffer without considering the current level of indentation */
SGE_API void string_builder_stitch(string_builder_t* builder, const char* const format, ...);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void string_builder_append_builder(string_builder_t* builder, string_builder_t* _builder)
{
	string_builder_append(builder, string_builder_get_str(_builder));
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void string_builder_append_builder_destroy(string_builder_t* builder, string_builder_t* _builder)
{
	string_builder_append_builder(builder, _builder);
	string_builder_destroy(_builder);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void string_builder_stitch_builder(string_builder_t* builder, string_builder_t* _builder)
{
	string_builder_stitch(builder, string_builder_get_str(_builder));
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void string_builder_stitch_builder_destroy(string_builder_t* builder, string_builder_t* _builder)
{
	string_builder_stitch_builder(builder, _builder);
	string_builder_destroy(_builder);
}

/* appends null termination character, sometimes we don't need to append that so lets keep it explicit  not default */
SGE_API void string_builder_append_null(string_builder_t* builder);
SGE_API void string_builder_append_newline(string_builder_t* builder);
SGE_API void string_builder_stitch_newline(string_builder_t* builder);

/* increments the indentation for the subsequent calls to string_builder_append() */
SGE_API void string_builder_increment_indentation(string_builder_t* builder);
/* decrements the indentiation for the subsequent calls to the string_builder_append() */
SGE_API void string_builder_decrement_indentation(string_builder_t* builder);


/* clears the string buffer and indentation buffer */
SGE_API void string_builder_clear(string_builder_t* builder);

END_CPP_COMPATIBLE
