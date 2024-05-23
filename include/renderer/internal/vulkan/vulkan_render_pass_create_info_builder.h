/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_create_info_builder.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_render_pass.h> // vulkan_render_pass_create_info_t

#include <bufferlib/buffer.h> // buffer_t

typedef struct vulkan_subpass_create_info_t vulkan_subpass_create_info_t;
typedef struct vulkan_subpass_create_info_builder_t vulkan_subpass_create_info_builder_t;
typedef struct vulkan_shader_resource_description_builder_t vulkan_shader_resource_description_builder_t;

typedef struct vulkan_render_pass_create_info_build_info_t
{
	/* list of VkSubpassDependency objects */
	buffer_t subpass_dependencies;
	/* list of VkAttachmentDescription objects */
	buffer_t attachment_descriptions;
	/* list of vulkan_attachment_next_pass_usage_t objects */
	buffer_t attachment_usages;
	/* non-null if set by vulkan_render_pass_create_info_builder_add_render_set_bindings_builder() */
	vulkan_shader_resource_description_builder_t* render_set_bindings_builder;
	bool is_destroy_render_set_bindings_builder;
	/* non-null if allocated by vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket() */
	VkImageView* vo_supplementary_attachments;
	/* non-null if set by vulkan_render_pass_create_info_builder_set_subpasses_builder*/
	vulkan_subpass_create_info_builder_t* subpasses_builder;
	bool is_destroy_subpasses_builder;

	bool is_use_render_set_bindings_builder;
	bool is_use_subpasses_builder;
	bool is_supplementary_attachments_internal;
	bool is_attachment_descriptions_internal;
	bool is_attachment_usages_internal;
	bool is_supbass_dependencies_internal;
} vulkan_render_pass_create_info_build_info_t;

typedef struct vulkan_render_pass_create_info_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_render_pass_create_info_t objects */
	buffer_t create_info_array;
	/* array of vulkan_render_pass_create_build_info_t objects */
	buffer_t build_info_array;
	/* currently bound element in the above array, can be set by vulkan_render_pass_create_info_builder_bind() */
	u32 bind_index;
} vulkan_render_pass_create_info_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_render_pass_create_info_builder_t* vulkan_render_pass_create_info_builder_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_render_pass_create_info_builder_destroy(vulkan_render_pass_create_info_builder_t* builder);

RENDERER_API void vulkan_render_pass_create_info_builder_add(vulkan_render_pass_create_info_builder_t* builder, u32 count);
RENDERER_API void vulkan_render_pass_create_info_builder_bind(vulkan_render_pass_create_info_builder_t* builder, u32 index);
RENDERER_API void vulkan_render_pass_create_info_builder_add_next(vulkan_render_pass_create_info_builder_t* builder);

RENDERER_API vulkan_render_pass_create_info_t* vulkan_render_pass_create_info_builder_get(vulkan_render_pass_create_info_builder_t* builder);
RENDERER_API u32 vulkan_render_pass_create_info_builder_get_count(vulkan_render_pass_create_info_builder_t* builder);


RENDERER_API void vulkan_render_pass_create_info_builder_set_framebuffer_count(vulkan_render_pass_create_info_builder_t* builder, u32 framebuffer_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(vulkan_render_pass_create_info_builder_t* builder, u32 bucket_count, u32 bucket_depth);
RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachments(vulkan_render_pass_create_info_builder_t* builder, u32 start_index, VkImageView* views, u32 view_count);
RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_descriptions(vulkan_render_pass_create_info_builder_t* builder, VkAttachmentDescription* descriptions, u32 description_count);
RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_usages(vulkan_render_pass_create_info_builder_t* builder, vulkan_attachment_next_pass_usage_t* usages, u32 usage_count);

RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_t* bindings, u32 binding_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder, bool is_destroy);
RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_t* infos, u32 info_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_builder_t* sci_builder, bool is_destroy);
RENDERER_API void vulkan_render_pass_create_info_builder_add_dependencies(vulkan_render_pass_create_info_builder_t* builder, VkSubpassDependency* dependencies, u32 dependency_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_dependencies(vulkan_render_pass_create_info_builder_t* builder, VkSubpassDependency* dependencies, u32 dependency_count);
RENDERER_API buffer_t* vulkan_render_pass_create_info_builder_get_dependencies_buffer(vulkan_render_pass_create_info_builder_t* builder);



END_CPP_COMPATIBLE