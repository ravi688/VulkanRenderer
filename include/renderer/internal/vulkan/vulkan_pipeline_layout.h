/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_pipeline_layout.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

typedef struct vulkan_pipeline_layout_create_info_t
{
	/* list of vulkan descriptor set layouts  */
	VkDescriptorSetLayout* vo_set_layouts;
	u32 set_layout_count;

	/* list of vulkan push constant ranges */
	VkPushConstantRange* vo_push_constant_ranges;
	u32 push_constant_range_count;
} vulkan_pipeline_layout_create_info_t;

typedef struct vulkan_pipeline_layout_t
{
	vulkan_renderer_t* renderer;
	VkPipelineLayout vo_handle;
} vulkan_pipeline_layout_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_new(memory_allocator_t* allocator);
RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info);
RENDERER_API void vulkan_pipeline_layout_create_no_alloc(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t OUT layout);
RENDERER_API void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout);
RENDERER_API void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* layout);

RENDERER_API void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* layout, VkShaderStageFlagBits stage_flags, u32 offset, u32 size, void* bytes);

END_CPP_COMPATIBLE
