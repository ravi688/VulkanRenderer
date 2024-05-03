/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.h is a part of VulkanRenderer

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

#include <renderer/defines.h>


typedef struct string_t
{
	memory_allocator_t* allocator;
	char* data;			// ptr to the string data
	u32 length;			// length of the string (exclusive of the null character)
} string_t;

/*
	description: creates a string (memory allocation)
	params:
		str: constant string (ptr to the characters)
	returns:
		string_t, string_t object
 */
RENDERER_API string_t string_create(memory_allocator_t* allocator, const char* str);

static FORCE_INLINE string_t string_null() { return (string_t) { NULL, 0 }; }

/*
	description: destroys a string (deallocation)
	params:
		str: ptr to the string_t object to be destroyed
	returns:
		nothing
 */
RENDERER_API void string_destroy(string_t* str);

RENDERER_API u32 strlen_safe(const char* str);
