/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glsl_memory_layout.c is a part of VulkanRenderer

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

#include <renderer/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>

u32 glsl_std140_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_STD140);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD140);
}

u32 glsl_std140_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD140);
}

typedef struct glsl_type_traits_callback_user_data_t
{ 
	struct_field_type_getter_t type_getter;
	void* user_data;
} glsl_type_traits_callback_user_data_t;

static glsl_type_layout_traits_t type_layout_traits_callback_handler(void* user_data, u32 type_index)
{
	AUTO _user_data = CAST_TO(glsl_type_traits_callback_user_data_t*, user_data);
	field_type_align_t type_align = _user_data->type_getter(_user_data->user_data, type_index);
	return (glsl_type_layout_traits_t)
	{
		.type = (type_align.type == 0) ? GLSL_TYPE_UNDEFINED : type_align.type,
		.align = type_align.align,
		.size = type_align.size
	};
}

u32 glsl_std140_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD140);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD140);
}

u32 glsl_std430_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_STD430);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD430);
}
u32 glsl_std430_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD430);
}
u32 glsl_std430_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD430);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD430);
}

u32 glsl_scalar_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
}
u32 glsl_scalar_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
}
u32 glsl_scalar_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_SCALAR);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_SCALAR);
}