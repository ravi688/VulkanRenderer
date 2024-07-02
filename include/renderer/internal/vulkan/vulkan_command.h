/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_command.h is a part of VulkanRenderer

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
#include <vulkan/vulkan.h>

BEGIN_CPP_COMPATIBLE

SGE_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image,
		VkImageSubresourceRange* subresource,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask,
		VkAccessFlags dst_access_mask,
		VkPipelineStageFlags src_pipeline_stage,
		VkPipelineStageFlags dst_pipeline_stage);

SGE_API void vulkan_command_bind_vertex_buffers(VkCommandBuffer cb, u32* bindings, VkBuffer* buffers, u32 buffer_count);

END_CPP_COMPATIBLE
