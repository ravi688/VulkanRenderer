/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_create_info_builder.h is a part of VulkanRenderer

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
#include <bufferlib/buffer.h>  // buffer_t

typedef struct vulkan_shader_create_info_t vulkan_shader_create_info_t;
typedef struct struct_descriptor_t struct_descriptor_t;

typedef struct vulkan_shader_resource_description_builder_t vulkan_shader_resource_description_builder_t;
typedef struct vulkan_vertex_buffer_layout_description_builder_t vulkan_vertex_buffer_layout_description_builder_t;
typedef struct vulkan_render_pass_description_builder_t vulkan_render_pass_description_builder_t;
typedef struct vulkan_graphics_pipeline_description_builder_t vulkan_graphics_pipeline_description_builder_t;

typedef struct vulkan_shader_create_info_build_info_t
{
	/* material set bindings builder */
	vulkan_shader_resource_description_builder_t* msb_builder;
	bool is_destroy_msb_builder;
	vulkan_vertex_buffer_layout_description_builder_t* vbld_builder;
	bool is_destroy_vbld_builder;
	vulkan_render_pass_description_builder_t* rpd_builder;
	bool is_destroy_rpd_builder;
	vulkan_graphics_pipeline_description_builder_t* gfx_pipes_builder;
	bool is_destroy_gfx_pipes_builder;

	bool is_use_msb_builder;
	bool is_use_vbld_builder;
	bool is_use_rpd_builder;
	bool is_use_gfx_pipes_builder;
} vulkan_shader_create_info_build_info_t;

typedef struct vulkan_shader_create_info_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_shader_create_info_t objects */
	buffer_t create_info_array;
	/* array of vulkan_shader_create_info_build_info_t objects */
	buffer_t build_info_array;
	u32 bind_index;
} vulkan_shader_create_info_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_shader_create_info_builder_t* vulkan_shader_create_info_builder_create(memory_allocator_t* allocator);
RENDERER_API vulkan_shader_create_info_builder_t* vulkan_shader_create_info_builder_create_inverse(memory_allocator_t* allocator, vulkan_shader_create_info_t* descriptions, u32 description_count);
RENDERER_API void vulkan_shader_create_info_builder_destroy(vulkan_shader_create_info_builder_t* builder);

RENDERER_API void vulkan_shader_create_info_builder_add(vulkan_shader_create_info_builder_t* builder, u32 count);
RENDERER_API void vulkan_shader_create_info_builder_bind(vulkan_shader_create_info_builder_t* builder, u32 index);

RENDERER_API vulkan_shader_create_info_t* vulkan_shader_create_info_builder_get(vulkan_shader_create_info_builder_t* builder);
RENDERER_API u32 vulkan_shader_create_info_builder_get_count(vulkan_shader_create_info_builder_t* builder);

RENDERER_API void vulkan_shader_create_info_builder_set_material_set_bindings_builder(vulkan_shader_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder, bool is_destroy);
RENDERER_API void vulkan_shader_create_info_builder_set_vertex_buffer_layout_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_vertex_buffer_layout_description_builder_t* vbld_builder, bool is_destroy);
RENDERER_API void vulkan_shader_create_info_builder_set_render_pass_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_render_pass_description_builder_t* rpd_builder, bool is_destroy);
RENDERER_API void vulkan_shader_create_info_builder_set_graphics_pipeline_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_graphics_pipeline_description_builder_t* gfx_pipes_builder, bool is_destroy);
