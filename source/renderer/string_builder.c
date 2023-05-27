/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string_builder.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

#include <renderer/string_builder.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <stdarg.h>
#include <stdio.h>

RENDERER_API string_builder_t* string_builder_create(memory_allocator_t* allocator, u32 capacity)
{
	string_builder_t* builder = (allocator == NULL) ? heap_new(string_builder_t) : memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_STRING_BUILDER, string_builder_t);
	builder->allocator = allocator;
	builder->string_buffer = buf_create(sizeof(char), capacity, 0);
	builder->indentation_buffer = buf_create(sizeof(char), 1, 0);
	return builder;
}

RENDERER_API void string_builder_destroy(string_builder_t* builder)
{
	buf_free(&builder->string_buffer);
	buf_free(&builder->indentation_buffer);
	if(builder->allocator != NULL)
		memory_allocator_dealloc(builder->allocator, builder);
	else
		heap_free(builder);
}

RENDERER_API void string_builder_append(string_builder_t* builder, const char* const format, ...)
{
	MEM_CHECK(builder);

	va_list args;

	/* get the required size of the format string */
	va_start(args, format);
	u32 required_size = vsnprintf(NULL, 0, format, args);
	va_end(args);

	/* if nothing to write, return */
	if(required_size == 0)
		return;

	u32 indentation_level = buf_get_element_count(&builder->indentation_buffer);

	/* extend the string buffer to accomodate the formatted string and the indentation string */
	buf_ucount_t index = buf_get_element_count(&builder->string_buffer);
	buf_push_pseudo(&builder->string_buffer, required_size + 1 + indentation_level);
	void* ptr = buf_get_ptr_at(&builder->string_buffer, index);

	/* put the indentiation string into the buffer */
	if(indentation_level > 0)
		memcopyv(ptr, buf_get_ptr(&builder->indentation_buffer), u8, indentation_level);

	/* format the string and put that into the buffer */
	va_start(args, format);
	u32 written_size = vsnprintf(CAST_TO(char*, ptr + indentation_level), required_size + 1, format, args);
	_debug_assert__(written_size == required_size);
	va_end(args);

	/* pop out the null character */
	buf_pop_pseudo(&builder->string_buffer, 1);
}

RENDERER_API void string_builder_append_null(string_builder_t* builder)
{
	buf_push_null(&builder->string_buffer);
}

RENDERER_API void string_builder_increment_indentation(string_builder_t* builder)
{
	buf_push_char(&builder->indentation_buffer, '\t');
}

RENDERER_API void string_builder_decrement_indentation(string_builder_t* builder)
{
	buf_pop(&builder->indentation_buffer, NULL);
}

RENDERER_API char* string_builder_get_str(string_builder_t* builder)
{
	return CAST_TO(char*, buf_get_ptr(&builder->string_buffer));
}

RENDERER_API void string_builder_clear(string_builder_t* builder)
{
	buf_clear(&builder->string_buffer, NULL);
	buf_clear(&builder->indentation_buffer, NULL);
}