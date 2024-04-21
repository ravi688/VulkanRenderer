/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_command_buffer.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/debug.h>

RENDERER_API VkCommandPool vulkan_command_pool_create(vulkan_renderer_t* renderer, u32 queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo command_pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndex,
		.flags = flags
	};
	VkCommandPool pool;
	vkCall(vkCreateCommandPool(renderer->logical_device->vo_handle, &command_pool_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &pool));
	log_msg("CommandPool for the queueFamilyIndex: %u is created successfully\n", queueFamilyIndex);
	return pool;
}

RENDERER_API VkCommandBuffer vulkan_command_buffer_allocate(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool)
{
	VkCommandBuffer buffer;
	vulkan_command_buffer_allocatev(renderer, level, pool, 1, &buffer);
	return buffer;
}

RENDERER_API void vulkan_command_buffer_allocatev(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool, u32 count, VkCommandBuffer* out_buffers)
{
	VkCommandBufferAllocateInfo alloc_info = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = level,
		.commandPool = pool,
		.commandBufferCount = count
	};
	vkCall(vkAllocateCommandBuffers(renderer->logical_device->vo_handle, &alloc_info, out_buffers));
}

RENDERER_API void vulkan_command_buffer_begin(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = flags,
	};
	vkCall(vkBeginCommandBuffer(buffer, &begin_info));
}

RENDERER_API void vulkan_command_buffer_reset(VkCommandBuffer buffer, VkCommandBufferResetFlags flags) 
{ 
	vkCall(vkResetCommandBuffer(buffer, flags)); 
}

RENDERER_API void vulkan_command_buffer_end(VkCommandBuffer buffer) 
{ 
	vkCall(vkEndCommandBuffer(buffer)); 
}

