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

RENDERER_API void vulkan_queue_submit(VkQueue queue, VkCommandBuffer buffer, VkSemaphore wait, VkPipelineStageFlags wait_dst_stage, VkSemaphore signal)
{
	vulkan_queue_submitv(queue, 1, &buffer, wait, wait_dst_stage, signal, VK_NULL_HANDLE);
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
