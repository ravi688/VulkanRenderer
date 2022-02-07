
#pragma once

#include <vulkan/vulkan.h>

#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>

typedef struct render_window_t render_window_t;

typedef enum vulkan_renderer_gpu_type_t
{
	VULKAN_RENDERER_GPU_TYPE_AUTO = 0,				// automatically selects the best GPU
	VULKAN_RENDERER_GPU_TYPE_INTEGRATED,			// selects integrated gpu if present, otherwise fallbacks to discrete
	VULKAN_RENDERER_GPU_TYPE_DISCRETE				// selects discrete gpu if present, otherwise fallbacks to integrated
} vulkan_renderer_gpu_type_t;


typedef struct vulkan_renderer_t
{
	vulkan_instance_t* instance;
	vulkan_logical_device_t* logical_device;
	vulkan_physical_device_t* physical_device;
	vulkan_render_pass_t* render_pass;
	VkSurfaceKHR surface;
	VkQueue vk_graphics_queue;
	VkCommandPool vk_command_pool;
	VkDescriptorPool vk_descriptor_pool;

	render_window_t* window;
	vulkan_swapchain_create_info_t swapchain_create_info;		// for recreating the swapchain
	vulkan_swapchain_t* swapchain;

	u32 vk_graphics_queue_index;
	VkCommandBuffer* vk_command_buffers;
} vulkan_renderer_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_renderer_t* vulkan_renderer_init(vulkan_renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen);
RENDERER_API void vulkan_renderer_terminate(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_renderer_update(vulkan_renderer_t* renderer);
RENDERER_API bool vulkan_renderer_is_running(vulkan_renderer_t* renderer);

RENDERER_API void vulkan_renderer_begin_frame(vulkan_renderer_t* renderer, float r, float g, float b, float a);
RENDERER_API void vulkan_renderer_end_frame(vulkan_renderer_t* renderer);

RENDERER_API render_window_t* vulkan_renderer_get_window(vulkan_renderer_t* renderer);

END_CPP_COMPATIBLE
