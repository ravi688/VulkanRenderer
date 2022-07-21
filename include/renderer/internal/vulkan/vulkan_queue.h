
#pragma once


#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_defines.h>

BEGIN_CPP_COMPATIBLE

RENDERER_API void vulkan_queue_present(VkQueue queue, VkSwapchainKHR swapchain, u32 image_index, VkSemaphore wait);
RENDERER_API void vulkan_queue_presentv(VkQueue queue, u32 swapchain_count, VkSwapchainKHR* swapchains, u32* image_indices, VkSemaphore wait);
RENDERER_API void vulkan_queue_submit(VkQueue queue, VkCommandBuffer buffer, VkSemaphore wait, VkPipelineStageFlags wait_dst_stage, VkSemaphore signal, VkFence fence);
RENDERER_API void vulkan_queue_submitv(VkQueue queue, u32 count, VkCommandBuffer* buffers, VkSemaphore wait, VkPipelineStageFlags wait_dst, VkSemaphore signal, VkFence fence);
static inline void vulkan_queue_wait_idle(VkQueue queue) { vkCall(vkQueueWaitIdle(queue)); } 

END_CPP_COMPATIBLE
