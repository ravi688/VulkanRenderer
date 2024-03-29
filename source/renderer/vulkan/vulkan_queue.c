/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_queue.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_queue.h>

RENDERER_API void vulkan_queue_present(VkQueue queue, VkSwapchainKHR swapchain, u32 image_index, VkSemaphore wait)
{
	vulkan_queue_presentv(queue, 1, &swapchain, &image_index, wait);
}

RENDERER_API void vulkan_queue_presentv(VkQueue queue, u32 swapchain_count, VkSwapchainKHR* swapchains, u32* image_indices, VkSemaphore wait)
{
	VkPresentInfoKHR present_info =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = swapchain_count,
		.pSwapchains = swapchains,
		.waitSemaphoreCount = (wait == VK_NULL_HANDLE) ? 0 : 1,
		.pWaitSemaphores = &wait,
		.pImageIndices = image_indices
	};

	vkCall(vkQueuePresentKHR(queue, &present_info));
}

RENDERER_API void vulkan_queue_submit(VkQueue queue, VkCommandBuffer buffer, VkSemaphore wait, VkPipelineStageFlags wait_dst_stage, VkSemaphore signal, VkFence fence)
{
	vulkan_queue_submitv(queue, 1, &buffer, wait, wait_dst_stage, signal, fence);
}

RENDERER_API void vulkan_queue_submitv(VkQueue queue, u32 count, VkCommandBuffer* buffers, VkSemaphore wait, VkPipelineStageFlags wait_dst, VkSemaphore signal, VkFence fence)
{
	VkSubmitInfo submit_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pWaitSemaphores = &wait,
		.pWaitDstStageMask = &wait_dst,
		.pSignalSemaphores = &signal,
		.waitSemaphoreCount = (wait == VK_NULL_HANDLE) ? 0 : 1,
		.signalSemaphoreCount = (signal == VK_NULL_HANDLE) ? 0 : 1,
		.commandBufferCount = count,
		.pCommandBuffers = buffers
	};
	vkCall(vkQueueSubmit(queue, 1, &submit_info, fence));
}
