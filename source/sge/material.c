/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: material.c is a part of VulkanRenderer

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

#include <sge/material.h>
#include <sge/sge.h>

SGE_API material_t* material_new(memory_allocator_t* allocator)
{
	return vulkan_material_new(allocator);
}

SGE_API material_t* material_create(renderer_t* renderer, shader_t* shader)
{
	return vulkan_material_create(renderer->vulkan_handle, shader);
}

SGE_API void material_create_no_alloc(renderer_t* renderer, shader_t* shader, material_t* material)
{
	vulkan_material_create_no_alloc(renderer->vulkan_handle, shader, material);
}

SGE_API void material_destroy(material_t* material)
{
	vulkan_material_destroy(material);
}

SGE_API void material_release_resources(material_t* material)
{
	vulkan_material_release_resources(material);
}

SGE_API material_field_handle_t material_get_field_handle(material_t* material, const char* name)
{
	return vulkan_material_get_field_handle(material, name);	
}


/* ------------------------------------------ BEGIN: PUSH CONSTANTS -------------------------------------------*/

SGE_API void material_set_push_floatH(material_t* material, material_field_handle_t handle, float value)
{
	vulkan_material_set_push_floatH(material, handle, value);
}

SGE_API void material_set_push_intH(material_t* material, material_field_handle_t handle, int value)
{
	vulkan_material_set_push_intH(material, handle, value);
}

SGE_API void material_set_push_uintH(material_t* material, material_field_handle_t handle, uint value)
{
	vulkan_material_set_push_uintH(material, handle, value);
}

SGE_API void material_set_push_vec2H(material_t* material, material_field_handle_t handle, vec2_t value)
{
	vulkan_material_set_push_vec2H(material, handle, value);
}

SGE_API void material_set_push_vec3H(material_t* material, material_field_handle_t handle, vec3_t value)
{
	vulkan_material_set_push_vec3H(material, handle, value);
}

SGE_API void material_set_push_vec4H(material_t* material, material_field_handle_t handle, vec4_t value)
{
	vulkan_material_set_push_vec4H(material, handle, value);
}

SGE_API void material_set_push_mat2H(material_t* material, material_field_handle_t handle, mat2_t value)
{
	vulkan_material_set_push_mat2H(material, handle, value);
}

SGE_API void material_set_push_mat4H(material_t* material, material_field_handle_t handle, mat4_t value)
{
	vulkan_material_set_push_mat4H(material, handle, value);
}

// getters
SGE_API float material_get_push_floatH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_floatH(material, handle);
}

SGE_API int material_get_push_intH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_intH(material, handle);
}

SGE_API uint material_get_push_uintH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_uintH(material, handle);
}

SGE_API vec2_t material_get_push_vec2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec2H(material, handle);
}

SGE_API vec3_t material_get_push_vec3H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec3H(material, handle);
}

SGE_API vec4_t material_get_push_vec4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec4H(material, handle);
}

SGE_API mat2_t material_get_push_mat2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_mat2H(material, handle);
}

SGE_API mat4_t material_get_push_mat4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_mat4H(material, handle);
}

/* functions accepting strings */
SGE_API void material_set_push_float(material_t* material, const char* name, float value)
{
	vulkan_material_set_push_float(material, name, value);
}

SGE_API void material_set_push_int(material_t* material, const char* name, int value)
{
	vulkan_material_set_push_int(material, name, value);
}

SGE_API void material_set_push_uint(material_t* material, const char* name, uint value)
{
	vulkan_material_set_push_uint(material, name, value);
}

SGE_API void material_set_push_vec2(material_t* material, const char* name, vec2_t value)
{
	vulkan_material_set_push_vec2(material, name, value);
}

SGE_API void material_set_push_vec3(material_t* material, const char* name, vec3_t value)
{
	vulkan_material_set_push_vec3(material, name, value);
}

SGE_API void material_set_push_vec4(material_t* material, const char* name, vec4_t value)
{
	vulkan_material_set_push_vec4(material, name, value);
}

SGE_API void material_set_push_mat2(material_t* material, const char* name, mat2_t value)
{
	vulkan_material_set_push_mat2(material, name, value);
}

SGE_API void material_set_push_mat4(material_t* material, const char* name, mat4_t value)
{
	vulkan_material_set_push_mat4(material, name, value);
}

SGE_API float material_get_push_float(material_t* material, const char* name)
{
	return vulkan_material_get_push_float(material, name);
}

SGE_API int material_get_push_int(material_t* material, const char* name)
{
	return vulkan_material_get_push_int(material, name);
}

SGE_API uint material_get_push_uint(material_t* material, const char* name)
{
	return vulkan_material_get_push_uint(material, name);
}

SGE_API vec2_t material_get_push_vec2(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec2(material, name);
}

SGE_API vec3_t material_get_push_vec3(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec3(material, name);
}

SGE_API vec4_t material_get_push_vec4(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec4(material, name);
}

SGE_API mat2_t material_get_push_mat2(material_t* material, const char* name)
{
	return vulkan_material_get_push_mat2(material, name);
}

SGE_API mat4_t material_get_push_mat4(material_t* material, const char* name)
{
	return vulkan_material_get_push_mat4(material, name);
}

/* ------------------------------------------ END: PUSH CONSTANTS -------------------------------------------*/

SGE_API void material_set_floatH(material_t* material, material_field_handle_t handle, float value)
{
	vulkan_material_set_floatH(material, handle, value);
}

SGE_API void material_set_intH(material_t* material, material_field_handle_t handle, int value)
{
	vulkan_material_set_intH(material, handle, value);
}

SGE_API void material_set_uintH(material_t* material, material_field_handle_t handle, uint value)
{
	vulkan_material_set_uintH(material, handle, value);
}

SGE_API void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t value)
{
	vulkan_material_set_vec2H(material, handle, value);
}

SGE_API void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t value)
{
	vulkan_material_set_vec3H(material, handle, value);
}

SGE_API void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t value)
{
	vulkan_material_set_vec4H(material, handle, value);
}

SGE_API void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t value)
{
	vulkan_material_set_mat2H(material, handle, value);
}

SGE_API void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t value)
{
	vulkan_material_set_mat4H(material, handle, value);
}

SGE_API void material_set_textureH(material_t* material, material_field_handle_t handle, texture_t* texture)
{
	vulkan_material_set_textureH(material, handle, texture);
}

SGE_API float material_get_floatH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_floatH(material, handle);
}

SGE_API int material_get_intH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_intH(material, handle);
}

SGE_API uint material_get_uintH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_intH(material, handle);
}

SGE_API vec2_t material_get_vec2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec2H(material, handle);
}

SGE_API vec3_t material_get_vec3H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec3H(material, handle);
}

SGE_API vec4_t material_get_vec4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec4H(material, handle);
}

SGE_API mat2_t material_get_mat2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_mat2H(material, handle);
}

SGE_API mat4_t material_get_mat4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_mat4H(material, handle);
}

SGE_API texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_texture2dH(material, handle);
}

/* functions accepting strings */
SGE_API void material_set_float(material_t* material, const char* name, float value)
{
	vulkan_material_set_float(material, name, value);
}

SGE_API void material_set_int(material_t* material, const char* name, int value)
{
	vulkan_material_set_int(material, name, value);
}

SGE_API void material_set_uint(material_t* material, const char* name, uint value)
{
	vulkan_material_set_uint(material, name, value);
}

SGE_API void material_set_vec2(material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_vec2(material, name, v);
}

SGE_API void material_set_vec3(material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_vec3(material, name, v);
}

SGE_API void material_set_vec4(material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_vec4(material, name, v);
}

SGE_API void material_set_mat2(material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_mat2(material, name, m);
}

SGE_API void material_set_mat4(material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_mat4(material, name, m);
}

SGE_API void material_set_texture(material_t* material, const char* name, texture_t* texture)
{
	vulkan_material_set_texture(material, name, texture);
}

SGE_API float material_get_float(material_t* material, const char* name)
{
	return vulkan_material_get_float(material, name);
}

SGE_API int material_get_int(material_t* material, const char* name)
{
	return vulkan_material_get_int(material, name);
}

SGE_API uint material_get_uint(material_t* material, const char* name)
{
	return vulkan_material_get_uint(material, name);
}

SGE_API vec2_t material_get_vec2(material_t* material, const char* name)
{
	return vulkan_material_get_vec2(material, name);
}

SGE_API vec3_t material_get_vec3(material_t* material, const char* name)
{
	return vulkan_material_get_vec3(material, name);
}

SGE_API vec4_t material_get_vec4(material_t* material, const char* name)
{
	return vulkan_material_get_vec4(material, name);
}

SGE_API mat2_t material_get_mat2(material_t* material, const char* name)
{
	return vulkan_material_get_mat2(material, name);
}

SGE_API mat4_t material_get_mat4(material_t* material, const char* name)
{
	return vulkan_material_get_mat4(material, name);
}

SGE_API texture_t* material_get_texture2d(material_t* material, const char* name)
{
	return vulkan_material_get_texture2d(material, name);
}
