/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glsl_memory_layout.h is a part of VulkanRenderer

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


/* Implementation of memory_layout_provider_callbacks_t for GLSL memory layout */

#pragma once

#include <sge/struct_descriptor.h> /* memory_layout_provider_callbacks_t */

u32 glsl_std140_get_type_align(u8 type, bool is_array);
u32 glsl_std140_get_type_size(u8 type);
u32 glsl_std140_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

u32 glsl_std430_get_type_align(u8 type, bool is_array);
u32 glsl_std430_get_type_size(u8 type);
u32 glsl_std430_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

u32 glsl_scalar_get_type_align(u8 type, bool is_array);
u32 glsl_scalar_get_type_size(u8 type);
u32 glsl_scalar_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_std140_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_std140_get_type_align,
		.get_type_size = glsl_std140_get_type_size,
		.get_struct_align = glsl_std140_get_struct_align
	};
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_std430_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_std430_get_type_align,
		.get_type_size = glsl_std430_get_type_size,
		.get_struct_align = glsl_std430_get_struct_align
	};
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_scalar_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_scalar_get_type_align,
		.get_type_size = glsl_scalar_get_type_size,
		.get_struct_align = glsl_scalar_get_struct_align
	};
}


#define GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT glsl_std140_memory_layout_callbacks()
#define GLSL_STORAGE_BUFFER_MEMORY_LAYOUT glsl_std430_memory_layout_callbacks()
#define GLSL_PUSH_CONSTANT_BUFFER_MEMORY_LAYOUT GLSL_STORAGE_BUFFER_MEMORY_LAYOUT
#define GLSL_MEMORY_LAYOUT_CALLBACKS GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT
