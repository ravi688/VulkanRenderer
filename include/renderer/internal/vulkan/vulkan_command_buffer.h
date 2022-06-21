
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>


BEGIN_CPP_COMPATIBLE

RENDERER_API VkCommandPool vulkan_command_pool_create(vulkan_renderer_t* renderer, u32 queueFamilyIndex, VkCommandPoolCreateFlags flags);
RENDERER_API VkCommandBuffer vulkan_command_buffer_allocate(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool);
RENDERER_API void vulkan_command_buffer_allocatev(vulkan_renderer_t* renderer, VkCommandBufferLevel level, VkCommandPool pool, u32 count, VkCommandBuffer* out_buffers);
RENDERER_API void vulkan_command_buffer_begin(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags);
RENDERER_API void vulkan_command_buffer_reset(VkCommandBuffer buffer, VkCommandBufferResetFlags flags);
RENDERER_API void vulkan_command_buffer_end(VkCommandBuffer buffer);

END_CPP_COMPATIBLE
