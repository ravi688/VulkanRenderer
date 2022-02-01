
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>

typedef struct vulkan_swapchain_t vulkan_swapchain_t;
typedef struct render_window_t render_window_t;

#include <renderer/internal/vulkan/vulkan_instance.h>

typedef struct renderer_t
{
	vulkan_instance_t* instance;
	VkPhysicalDevice vk_physical_device;
	VkSurfaceKHR surface;
	VkDevice vk_device;
	VkQueue vk_graphics_queue;
	VkRenderPass vk_render_pass;
	VkCommandPool vk_command_pool;
	VkDescriptorPool vk_descriptor_pool;

	render_window_t* window;
	vulkan_swapchain_t* swapchain;

	uint32_t vk_graphics_queue_index;
	tuple_t(uint32_t, pVkCommandBuffer_t) vk_command_buffers;
} renderer_t;

