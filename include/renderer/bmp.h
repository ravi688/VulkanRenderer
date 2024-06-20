/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bmp.h is a part of VulkanRenderer

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
#include <calltrace/calltrace.h>

typedef struct bmp_t
{
	memory_allocator_t* allocator;
	void* data;
	u32 width;
	u32 height;
	u8 channel_count;
} bmp_t;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define bmp_load(...) define_alias_function_macro(bmp_load, __VA_ARGS__)
#define bmp_destroy(...) define_alias_function_macro(bmp_destroy, __VA_ARGS__)
#define bmp_write(...) define_alias_function_macro(bmp_write, __VA_ARGS__)

RENDERER_API function_signature(bmp_t, bmp_load, memory_allocator_t* allocator, const char* file_path);
RENDERER_API function_signature(void, bmp_destroy, bmp_t bmp);
RENDERER_API function_signature(void, bmp_write, void* data, u32 width, u32 height, u8 channel_count, const char* file_path);

#ifdef __cplusplus
}
#endif // __cplusplus
