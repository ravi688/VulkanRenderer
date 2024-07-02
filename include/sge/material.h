/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: material.h is a part of VulkanRenderer

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

#ifdef SGE_VULKAN_DRIVER
	#include <sge/internal/vulkan/vulkan_material.h>
	typedef struct vulkan_texture_t vulkan_texture_t;
	typedef vulkan_texture_t texture_t;
	typedef vulkan_shader_t shader_t;
	typedef vulkan_material_t material_t;
	typedef vulkan_material_field_handle_t material_field_handle_t;
#elif SGE_OPENGL_DRIVER
	#include <sge/internal/opengl/opengl_material.h>
	typedef struct opengl_texture_t opengl_texture_t;
	typedef opengl_shader_t shader_t;
	typedef opengl_material_t material_t;
	typedef opengl_material_field_handle_t material_field_handle_t;
#elif SGE_DIRECTX_DRIVER
	#include <sge/internal/directx/directx_material.h>
	typedef struct directx_texture_t directx_texture_t;
	typedef directx_texture_t directx_texture_t;
	typedef directx_shader_t shader_t;
	typedef directx_material_t material_t;
	typedef directx_material_field_handle_t material_field_handle_t;
#elif SGE_METAL_DRIVER
	#include <sge/internal/metal/metal_material.h>
	typedef struct metal_texture_t metal_texture_t;
	typedef metal_texture_t metal_texture_t;
	typedef metal_shader_t shader_t;
	typedef metal_material_t material_t;
	typedef metal_material_field_handle_t material_field_handle_t;
#endif

#include <hpml/vec2.h>
#include <hpml/vec3.h>
#include <hpml/vec4.h>
#include <hpml/mat2.h>
#include <hpml/mat4.h>

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API material_t* material_new(memory_allocator_t* allocator);
SGE_API material_t* material_create(renderer_t* renderer, shader_t* shader);
SGE_API void material_create_no_alloc(renderer_t* renderer, shader_t* shader, material_t* material);
SGE_API void material_destroy(material_t* material);
SGE_API void material_release_resources(material_t* material);

SGE_API material_field_handle_t material_get_field_handle(material_t* material, const char* name);

SGE_API void material_set_push_floatH(material_t* material, material_field_handle_t handle, float value);
SGE_API void material_set_push_intH(material_t* material, material_field_handle_t handle, int value);
SGE_API void material_set_push_uintH(material_t* material, material_field_handle_t handle, uint value);
SGE_API void material_set_push_vec2H(material_t* material, material_field_handle_t handle, vec2_t value);
SGE_API void material_set_push_vec3H(material_t* material, material_field_handle_t handle, vec3_t value);
SGE_API void material_set_push_vec4H(material_t* material, material_field_handle_t handle, vec4_t value);
SGE_API void material_set_push_mat2H(material_t* material, material_field_handle_t handle, mat2_t value);
SGE_API void material_set_push_mat4H(material_t* material, material_field_handle_t handle, mat4_t value);

SGE_API void material_set_push_float(material_t* material, const char* name, float value);
SGE_API void material_set_push_int(material_t* material, const char* name, int value);
SGE_API void material_set_push_uint(material_t* material, const char* name, uint value);
SGE_API void material_set_push_vec2(material_t* material, const char* name, vec2_t v);
SGE_API void material_set_push_vec3(material_t* material, const char* name, vec3_t v);
SGE_API void material_set_push_vec4(material_t* material, const char* name, vec4_t v);
SGE_API void material_set_push_mat2(material_t* material, const char* name, mat2_t m);
SGE_API void material_set_push_mat4(material_t* material, const char* name, mat4_t m);

SGE_API float material_get_push_floatH(material_t* material, material_field_handle_t handle);
SGE_API int material_get_push_intH(material_t* material, material_field_handle_t handle);
SGE_API uint material_get_push_uintH(material_t* material, material_field_handle_t handle);
SGE_API vec2_t material_get_push_vec2H(material_t* material, material_field_handle_t handle);
SGE_API vec3_t material_get_push_vec3H(material_t* material, material_field_handle_t handle);
SGE_API vec4_t material_get_push_vec4H(material_t* material, material_field_handle_t handle);
SGE_API mat2_t material_get_push_mat2H(material_t* material, material_field_handle_t handle);
SGE_API mat4_t material_get_push_mat4H(material_t* material, material_field_handle_t handle);

SGE_API float material_get_push_float(material_t* material, const char* name);
SGE_API int material_get_push_int(material_t* material, const char* name);
SGE_API uint material_get_push_uint(material_t* material, const char* name);
SGE_API vec2_t material_get_push_vec2(material_t* material, const char* name);
SGE_API vec3_t material_get_push_vec3(material_t* material, const char* name);
SGE_API vec4_t material_get_push_vec4(material_t* material, const char* name);
SGE_API mat2_t material_get_push_mat2(material_t* material, const char* name);
SGE_API mat4_t material_get_push_mat4(material_t* material, const char* name);


SGE_API void material_set_floatH(material_t* material, material_field_handle_t handle, float value);
SGE_API void material_set_intH(material_t* material, material_field_handle_t handle, int value);
SGE_API void material_set_uintH(material_t* material, material_field_handle_t handle, uint value);
SGE_API void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t v);
SGE_API void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t v);
SGE_API void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t v);
SGE_API void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t m);
SGE_API void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t m);
SGE_API void material_set_textureH(material_t* material, material_field_handle_t handle, texture_t* texture);

SGE_API void material_set_float(material_t* material, const char* name, float value);
SGE_API void material_set_int(material_t* material, const char* name, int value);
SGE_API void material_set_uint(material_t* material, const char* name, uint value);
SGE_API void material_set_vec2(material_t* material, const char* name, vec2_t v);
SGE_API void material_set_vec3(material_t* material, const char* name, vec3_t v);
SGE_API void material_set_vec4(material_t* material, const char* name, vec4_t v);
SGE_API void material_set_mat2(material_t* material, const char* name, mat2_t m);
SGE_API void material_set_mat4(material_t* material, const char* name, mat4_t m);
SGE_API void material_set_texture(material_t* material, const char* name, texture_t* texture);


SGE_API float material_get_floatH(material_t* material, material_field_handle_t handle);
SGE_API int material_get_intH(material_t* material, material_field_handle_t handle);
SGE_API uint material_get_uintH(material_t* material, material_field_handle_t handle);
SGE_API vec2_t material_get_vec2H(material_t* material, material_field_handle_t handle);
SGE_API vec3_t material_get_vec3H(material_t* material, material_field_handle_t handle);
SGE_API vec4_t material_get_vec4H(material_t* material, material_field_handle_t handle);
SGE_API mat2_t material_get_mat2H(material_t* material, material_field_handle_t handle);
SGE_API mat4_t material_get_mat4H(material_t* material, material_field_handle_t handle);
SGE_API texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle);

SGE_API float material_get_float(material_t* material, const char* name);
SGE_API int material_get_int(material_t* material, const char* name);
SGE_API uint material_get_uint(material_t* material, const char* name);
SGE_API vec2_t material_get_vec2(material_t* material, const char* name);
SGE_API vec3_t material_get_vec3(material_t* material, const char* name);
SGE_API vec4_t material_get_vec4(material_t* material, const char* name);
SGE_API mat2_t material_get_mat2(material_t* material, const char* name);
SGE_API mat4_t material_get_mat4(material_t* material, const char* name);
SGE_API texture_t* material_get_texture2d(material_t* material, const char* name);

END_CPP_COMPATIBLE
