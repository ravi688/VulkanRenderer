/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_command.c is a part of VulkanRenderer

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

RENDERER_API void vulkan_command_bind_vertex_buffers(VkCommandBuffer cb, u32* bindings, VkBuffer* buffers, u32 buffer_count)
{
#if GLOBAL_DEBUG
	_debug_assert__(buffer_count >= 1);
	for(u32 i = 0; i < (buffer_count - 1); i++)
		if(bindings[i] >= bindings[i + 1])
			debug_log_fetal_error("Either the bindings are not sorted in increasing order correctly or duplicate bindings exists");
#endif /* GLOBAL_DEBUG */

	u32 group_size = 0;
	u32 i = 0;
	while(i < buffer_count)
	{
		/* group consecutive bindings */
		group_size = 1;
		u32 group = i++;
		for(; i < buffer_count; i++, group_size++)
			if(bindings[i - 1] == bindings[i])
				continue;
			else
				break;

		/* create offsets */
		VkDeviceSize offsets[group_size];
		for(u32 j = 0; j < group_size; j++)
			offsets[j] = 0;

		/* binding vertex buffers with consecutive bind points */
		vkCmdBindVertexBuffers(cb, bindings[group], group_size, &buffers[group], offsets);
	}
}
