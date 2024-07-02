/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_command_buffer.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>


BEGIN_CPP_COMPATIBLE

SGE_API VkCommandPool vulkan_command_pool_create(vulkan_renderer_t* renderer, u32 queueFamilyIndex, VkCommandPoolCreateFlags flags);
SGE_API VkCommandBuffer vulkan_command_buffer_allocate(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool);
SGE_API void vulkan_command_buffer_allocatev(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool, u32 count, VkCommandBuffer* out_buffers);
SGE_API void vulkan_command_buffer_begin(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags);
SGE_API void vulkan_command_buffer_reset(VkCommandBuffer buffer, VkCommandBufferResetFlags flags);
SGE_API void vulkan_command_buffer_end(VkCommandBuffer buffer);

END_CPP_COMPATIBLE
