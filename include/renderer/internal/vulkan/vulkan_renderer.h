
#pragma once

#include <vulkan/vulkan.h>

#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>

typedef struct render_window_t render_window_t;
typedef struct vulkan_shader_library_t vulkan_shader_library_t;
typedef struct vulkan_material_library_t vulkan_material_library_t;
typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
typedef struct vulkan_camera_system_t vulkan_camera_system_t;

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
	// vulkan_render_pass_t* render_pass; 							// render pass for rendering the final image
	// vulkan_render_pass_t* shadow_map_render_pass; 				// render pass for rendering the shadow map
	VkSurfaceKHR vo_surface;
	VkQueue vo_graphics_queue;
	VkCommandPool vo_command_pool;
	VkDescriptorPool vo_descriptor_pool;

	VkSemaphore vo_image_available_semaphore;
	VkSemaphore vo_render_finished_semaphore;
	VkFence vo_fence;

	render_window_t* window;
	vulkan_swapchain_create_info_t swapchain_create_info;		// for recreating the swapchain
	vulkan_swapchain_t* swapchain;

	u32 graphics_queue_index;
	VkCommandBuffer* vo_command_buffers;
	VkCommandBuffer vo_aux_command_buffer;

	VkSharingMode vo_sharing_mode;

	vulkan_descriptor_set_layout_t global_set_layout;
	vulkan_descriptor_set_layout_t object_set_layout;
	vulkan_descriptor_set_layout_t camera_set_layout;
	
	vulkan_descriptor_set_t global_set;

/*
	TODO:
	vulkan_pipeline_layout_pool_t* pipeline_layout_pool;
 */

	vulkan_shader_library_t* shader_library;
	vulkan_material_library_t* material_library;
	vulkan_render_pass_pool_t* render_pass_pool;
	vulkan_camera_system_t* camera_system;
} vulkan_renderer_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_renderer_t* vulkan_renderer_init(vulkan_renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
RENDERER_API void vulkan_renderer_terminate(vulkan_renderer_t* renderer);

/* submits the recorded command buffer to the graphics queue and performs presentation */
RENDERER_API void vulkan_renderer_update(vulkan_renderer_t* renderer);

RENDERER_API bool vulkan_renderer_is_running(vulkan_renderer_t* renderer);

/* begins the command buffer recording */
RENDERER_API void vulkan_renderer_begin_frame(vulkan_renderer_t* renderer);
/* ends the command buffer recording */
RENDERER_API void vulkan_renderer_end_frame(vulkan_renderer_t* renderer);

RENDERER_API render_window_t* vulkan_renderer_get_window(vulkan_renderer_t* renderer);

END_CPP_COMPATIBLE
