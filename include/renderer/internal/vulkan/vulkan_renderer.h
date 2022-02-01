
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>

typedef struct render_window_t render_window_t;

#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>

typedef struct renderer_t
{
	vulkan_instance_t* instance;
	vulkan_logical_device_t* logical_device;
	vulkan_physical_device_t* physical_device;
	VkSurfaceKHR surface;
	VkQueue vk_graphics_queue;
	VkRenderPass vk_render_pass;
	VkCommandPool vk_command_pool;
	VkDescriptorPool vk_descriptor_pool;

	render_window_t* window;
	vulkan_swapchain_create_info_t swapchain_create_info;		// for recreating the swapchain
	vulkan_swapchain_t* swapchain;

	uint32_t vk_graphics_queue_index;
	tuple_t(uint32_t, pVkCommandBuffer_t) vk_command_buffers;
} renderer_t;

