/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_material.h is a part of VulkanRenderer

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

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_handles.h> 	// vulkan_material_handle_t
#include <renderer/internal/vulkan/vulkan_object.h>
#include <renderer/struct_descriptor.h>
#include <hpml/vec2.h>
#include <hpml/vec3.h>
#include <hpml/vec4.h>
#include <hpml/mat2.h>
#include <hpml/mat4.h>

typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;

typedef struct vulkan_material_field_handle_t
{
	u16 uniform_index; 			// index of this resource in the list of uniform resource descriptors of this material otherwise 0xFFFF if this is not a uniform
	u16 index; 					// index of this resource in the list of resource descriptors of this material
	struct_field_handle_t field_handle; // handle to the field in the uniform buffer, STRUCT_FIELD_INVALID_HANDLE if this isn't a uniform
} vulkan_material_field_handle_t;

typedef struct vulkan_uniform_resource_t
{
	bool has_buffer;
	vulkan_buffer_t buffer;
	u16 index;					// index of this resource in the list of resouce descriptors of this material
} vulkan_uniform_resource_t;

typedef vulkan_uniform_resource_t* vulkan_uniform_resource_ptr_t;

typedef struct vulkan_material_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;

	vulkan_material_handle_t handle;

	// ptr to the shader object from which this material has been derived
	vulkan_shader_t* shader;

	// list of vulkan buffers for each uniform binding in the material set
	vulkan_uniform_resource_t* uniform_resources;
	u16 uniform_resource_count;

	// material set for this material
	vulkan_descriptor_set_t material_set;

} vulkan_material_t;

#define VULKAN_MATERIAL(ptr) VULKAN_OBJECT_UP_CAST(vulkan_material_t*, VULKAN_OBJECT_TYPE_MATERIAL, ptr)
#define VULKAN_MATERIAL_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_material_t*, VULKAN_OBJECT_TYPE_MATERIAL, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_material_t* vulkan_material_new(memory_allocator_t* allocator);
RENDERER_API vulkan_material_t* vulkan_material_create(vulkan_renderer_t* renderer, vulkan_shader_t* shader);
RENDERER_API void vulkan_material_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_t* shader, vulkan_material_t* material);
RENDERER_API void vulkan_material_destroy(vulkan_material_t* material);
RENDERER_API void vulkan_material_release_resources(vulkan_material_t* material);

/* logic functions */
RENDERER_API void vulkan_material_push_constants(vulkan_material_t* material, vulkan_pipeline_layout_t* pipeline_layout);

/* getters */
RENDERER_API vulkan_material_field_handle_t vulkan_material_get_field_handle(vulkan_material_t* material, const char* name);
RENDERER_API float vulkan_material_get_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API int vulkan_material_get_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API uint vulkan_material_get_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec2_t vulkan_material_get_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec3_t vulkan_material_get_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec4_t vulkan_material_get_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API mat2_t vulkan_material_get_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API mat4_t vulkan_material_get_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle);

RENDERER_API float vulkan_material_get_push_float(vulkan_material_t* material, const char* name);
RENDERER_API int vulkan_material_get_push_int(vulkan_material_t* material, const char* name);
RENDERER_API uint vulkan_material_get_push_uint(vulkan_material_t* material, const char* name);
RENDERER_API vec2_t vulkan_material_get_push_vec2(vulkan_material_t* material, const char* name);
RENDERER_API vec3_t vulkan_material_get_push_vec3(vulkan_material_t* material, const char* name);
RENDERER_API vec4_t vulkan_material_get_push_vec4(vulkan_material_t* material, const char* name);
RENDERER_API mat2_t vulkan_material_get_push_mat2(vulkan_material_t* material, const char* name);
RENDERER_API mat4_t vulkan_material_get_push_mat4(vulkan_material_t* material, const char* name);

RENDERER_API float vulkan_material_get_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API int vulkan_material_get_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API uint vulkan_material_get_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec2_t vulkan_material_get_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec3_t vulkan_material_get_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vec4_t vulkan_material_get_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API mat2_t vulkan_material_get_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API mat4_t vulkan_material_get_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle);
RENDERER_API vulkan_texture_t* vulkan_material_get_texture2dH(vulkan_material_t* material, vulkan_material_field_handle_t handle);

RENDERER_API float vulkan_material_get_float(vulkan_material_t* material, const char* name);
RENDERER_API int vulkan_material_get_int(vulkan_material_t* material, const char* name);
RENDERER_API uint vulkan_material_get_uint(vulkan_material_t* material, const char* name);
RENDERER_API vec2_t vulkan_material_get_vec2(vulkan_material_t* material, const char* name);
RENDERER_API vec3_t vulkan_material_get_vec3(vulkan_material_t* material, const char* name);
RENDERER_API vec4_t vulkan_material_get_vec4(vulkan_material_t* material, const char* name);
RENDERER_API mat2_t vulkan_material_get_mat2(vulkan_material_t* material, const char* name);
RENDERER_API mat4_t vulkan_material_get_mat4(vulkan_material_t* material, const char* name);
RENDERER_API vulkan_texture_t* vulkan_material_get_texture2d(vulkan_material_t* material, const char* name);

/* setters */
RENDERER_API void vulkan_material_set_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value);
RENDERER_API void vulkan_material_set_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value);
RENDERER_API void vulkan_material_set_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value);
RENDERER_API void vulkan_material_set_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t value);
RENDERER_API void vulkan_material_set_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t value);
RENDERER_API void vulkan_material_set_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t value);
RENDERER_API void vulkan_material_set_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t value);
RENDERER_API void vulkan_material_set_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t value);

RENDERER_API void vulkan_material_set_push_float(vulkan_material_t* material, const char* name, float value);
RENDERER_API void vulkan_material_set_push_int(vulkan_material_t* material, const char* name, int value);
RENDERER_API void vulkan_material_set_push_uint(vulkan_material_t* material, const char* name, uint value);
RENDERER_API void vulkan_material_set_push_vec2(vulkan_material_t* material, const char* name, vec2_t v);
RENDERER_API void vulkan_material_set_push_vec3(vulkan_material_t* material, const char* name, vec3_t v);
RENDERER_API void vulkan_material_set_push_vec4(vulkan_material_t* material, const char* name, vec4_t v);
RENDERER_API void vulkan_material_set_push_mat2(vulkan_material_t* material, const char* name, mat2_t m);
RENDERER_API void vulkan_material_set_push_mat4(vulkan_material_t* material, const char* name, mat4_t m);

RENDERER_API void vulkan_material_set_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value);
RENDERER_API void vulkan_material_set_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value);
RENDERER_API void vulkan_material_set_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value);
RENDERER_API void vulkan_material_set_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t v);
RENDERER_API void vulkan_material_set_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t v);
RENDERER_API void vulkan_material_set_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t v);
typedef union uvec2_t
{
	struct { u32 x, y; };
	struct { u32 width, height; };
} uvec2_t;
RENDERER_API void vulkan_material_set_uvec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec2_t value);
typedef union uvec3_t
{
	struct { u32 x, y, z; };
	struct { u32 width, height, depth; };
} uvec3_t;
RENDERER_API void vulkan_material_set_uvec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec3_t value);
typedef union uvec4_t
{
	struct { u32 x, y, z, w; };
	struct { u32 width, height, depth, time; };
} uvec4_t;
RENDERER_API void vulkan_material_set_uvec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec4_t value);
RENDERER_API void vulkan_material_set_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t m);
RENDERER_API void vulkan_material_set_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t m);
RENDERER_API void vulkan_material_set_textureH(vulkan_material_t* material, vulkan_material_field_handle_t handle, vulkan_texture_t* texture);

RENDERER_API void vulkan_material_set_float(vulkan_material_t* material, const char* name, float value);
RENDERER_API void vulkan_material_set_int(vulkan_material_t* material, const char* name, int value);
RENDERER_API void vulkan_material_set_uint(vulkan_material_t* material, const char* name, uint value);
RENDERER_API void vulkan_material_set_vec2(vulkan_material_t* material, const char* name, vec2_t v);
RENDERER_API void vulkan_material_set_vec3(vulkan_material_t* material, const char* name, vec3_t v);
RENDERER_API void vulkan_material_set_vec4(vulkan_material_t* material, const char* name, vec4_t v);
RENDERER_API void vulkan_material_set_uvec2(vulkan_material_t* material, const char* name, uvec2_t v);
RENDERER_API void vulkan_material_set_uvec3(vulkan_material_t* material, const char* name, uvec3_t v);
RENDERER_API void vulkan_material_set_uvec4(vulkan_material_t* material, const char* name, uvec4_t v);
RENDERER_API void vulkan_material_set_mat2(vulkan_material_t* material, const char* name, mat2_t m);
RENDERER_API void vulkan_material_set_mat4(vulkan_material_t* material, const char* name, mat4_t m);
RENDERER_API void vulkan_material_set_texture(vulkan_material_t* material, const char* name, vulkan_texture_t* texture);

RENDERER_API void vulkan_material_set_array_size(vulkan_material_t* material, const char* name, u32 size);
RENDERER_API void vulkan_material_set_buffer(vulkan_material_t* material, const char* block_name, vulkan_buffer_t* buffer);

RENDERER_API const struct_descriptor_t* vulkan_material_get_interface(vulkan_material_t* material, const char* block_name);

END_CPP_COMPATIBLE
