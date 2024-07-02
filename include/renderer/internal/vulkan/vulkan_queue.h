/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_queue.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_defines.h>

BEGIN_CPP_COMPATIBLE

SGE_API bool vulkan_queue_present(VkQueue queue, VkSwapchainKHR swapchain, u32 image_index, VkSemaphore wait);
SGE_API bool vulkan_queue_presentv(VkQueue queue, u32 swapchain_count, VkSwapchainKHR* swapchains, u32* image_indices, VkSemaphore wait);
SGE_API void vulkan_queue_submit(VkQueue queue, VkCommandBuffer buffer, VkSemaphore wait, VkPipelineStageFlags wait_dst_stage, VkSemaphore signal, VkFence fence);
SGE_API void vulkan_queue_submitv(VkQueue queue, u32 count, VkCommandBuffer* buffers, VkSemaphore wait, VkPipelineStageFlags wait_dst, VkSemaphore signal, VkFence fence);
static inline void vulkan_queue_wait_idle(VkQueue queue) { vkCall(vkQueueWaitIdle(queue)); } 

END_CPP_COMPATIBLE
