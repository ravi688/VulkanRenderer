/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_description_builder.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_render_pass_description.h> // vulkan_attachment_reference_type_t

typedef struct vulkan_vertex_buffer_layout_description_builder_t vulkan_vertex_buffer_layout_description_builder_t;

typedef struct vulkan_vertex_buffer_layout_description_builder_traits_t
{
	vulkan_vertex_buffer_layout_description_builder_t* builder;
	/* should the above builder be destroyed when vulkan_render_pass_description_builder_destroy() */
	bool is_destroy;
} vulkan_vertex_buffer_layout_description_builder_traits_t;

typedef buffer_t /* vulkan_vertex_buffer_layout_description_builder_traits_t* */ vulkan_vertex_buffer_layout_description_builder_list_t;

typedef struct vulkan_render_pass_description_build_info_t
{
	vulkan_vertex_buffer_layout_description_builder_list_t vbld_builders;
} vulkan_render_pass_description_build_info_t;

typedef struct vulkan_render_pass_description_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_render_pass_description_t objects */
	buffer_t description_array;
	/* array of vulkan_render_pass_description_build_info_t objects */
	buffer_t build_info_array;
	u32 bind_index;
} vulkan_render_pass_description_builder_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create(memory_allocator_t* allocator);
RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_render_pass_description_t* descriptions, u32 description_count);
RENDERER_API void vulkan_render_pass_description_builder_destroy(vulkan_render_pass_description_builder_t* builder);

RENDERER_API void vulkan_render_pass_description_builder_add(vulkan_render_pass_description_builder_t* builder, u32 count);
RENDERER_API void vulkan_render_pass_description_builder_bind(vulkan_render_pass_description_builder_t* builder, u32 index);

RENDERER_API vulkan_render_pass_description_t* vulkan_render_pass_description_builder_get(vulkan_render_pass_description_builder_t* builder);
RENDERER_API u32 vulkan_render_pass_description_builder_get_count(vulkan_render_pass_description_builder_t* builder);


RENDERER_API void vulkan_render_pass_description_builder_begin_pass(vulkan_render_pass_description_builder_t* builder, vulkan_render_pass_type_t type);
RENDERER_API void vulkan_render_pass_description_builder_add_input(vulkan_render_pass_description_builder_t* builder, glsl_type_t type, u32 index, u32 binding);
RENDERER_API void vulkan_render_pass_description_builder_add_attachment(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_type_t type);

RENDERER_API void vulkan_render_pass_description_builder_begin_subpass(vulkan_render_pass_description_builder_t* builder, u32 pipeline_index);
RENDERER_API void vulkan_render_pass_description_builder_set_vertex_infos_builder(vulkan_render_pass_description_builder_t* builder, vulkan_vertex_buffer_layout_description_builder_t* vbld_builder, bool is_destroy);
RENDERER_API void vulkan_render_pass_description_builder_add_attachment_reference(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_reference_type_t type, u32 reference, u32 binding);
RENDERER_API void vulkan_render_pass_description_builder_end_subpass(vulkan_render_pass_description_builder_t* builder);

RENDERER_API void vulkan_render_pass_description_builder_add_subpass_dependency(vulkan_render_pass_description_builder_t* builder, VkSubpassDependency* dependency);
RENDERER_API void vulkan_render_pass_description_builder_end_pass(vulkan_render_pass_description_builder_t* builder);

END_CPP_COMPATIBLE
