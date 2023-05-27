/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_command.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/assert.h>

#include <stdarg.h>
#include <renderer/alloc.h>	 // memcopyv

RENDERER_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image,
		VkImageSubresourceRange* subresource,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask,
		VkAccessFlags dst_access_mask,
		VkPipelineStageFlags src_pipeline_stage,
		VkPipelineStageFlags dst_pipeline_stage)
{
	VkImageMemoryBarrier barrier = 
	{ 
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcAccessMask = src_access_mask,
		.dstAccessMask = dst_access_mask
	};
	memcopy(&barrier.subresourceRange, subresource, VkImageSubresourceRange);
	vkCmdPipelineBarrier(cb, src_pipeline_stage, dst_pipeline_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
}
