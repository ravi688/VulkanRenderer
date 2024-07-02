/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.c is a part of VulkanRenderer

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


#include <renderer/string.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

#include <string.h>

SGE_API string_t string_create(memory_allocator_t* allocator, const char* str)
{
	u32 len = strlen(str);
	string_t string =
	{
		.data = CAST_TO(char*, memory_allocator_alloc(allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_CHAR_BUFFER, sizeof(char) * (len + 1))),
		.length = len,
		.allocator = allocator
	};
	strcpy(string.data, str);
	return string;
}

SGE_API void string_destroy(string_t* str)
{
	if(str->data == NULL) return;

	memory_allocator_dealloc(str->allocator, str->data);
	str->data = NULL;
	str->length = 0;
}

SGE_API u32 strlen_safe(const char* str)
{
	if(str == NULL)
		return 0;
	return strlen(str);
}
