/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_renderer.h is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>

#include <renderer/struct_descriptor.h>

typedef struct render_window_t render_window_t;
typedef struct vulkan_shader_library_t vulkan_shader_library_t;
typedef struct vulkan_material_library_t vulkan_material_library_t;
typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
typedef struct vulkan_camera_system_t vulkan_camera_system_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef enum vulkan_renderer_gpu_type_t
{
	VULKAN_RENDERER_GPU_TYPE_AUTO = 0,				// automatically selects the best GPU
	VULKAN_RENDERER_GPU_TYPE_INTEGRATED,			// selects integrated gpu if present, otherwise fallbacks to discrete
	VULKAN_RENDERER_GPU_TYPE_DISCRETE,				// selects discrete gpu if present, otherwise fallbacks to integrated
	VULKAN_RENDERER_GPU_TYPE_CPU					// selects cpu based software implementation of vulkan, otherwise fallbacks to integrated or discrete device
} vulkan_renderer_gpu_type_t;

typedef struct memory_allocator_t memory_allocator_t;
typedef struct vulkan_allocator_t vulkan_allocator_t;
IF_DEBUG( typedef struct string_builder_t string_builder_t; )

typedef struct vulkan_renderer_create_info_t
{
	renderer_t* renderer;
	vulkan_renderer_gpu_type_t prefered_gpu_type;
	/* width of the window's content area in screen pixels */
	u32 width;
	/* height of the window's content area in screen pixels */
	u32 height;
	/* string to be displayed in the title bar of the window */
	const char* title;
	/* set it to true to render onto the entire screen */
	bool full_screen;
	/* set it to true to make the window resizable; ignored if full_screen is true */
	bool resizable;

	/* number of swapchain images to create; however, the actual number of swapchain images might be different 
	 * it is automatically clamped to the max number of swapchain images possible if greater than that
	 * if zero, then it will use the minimum number of swapchain images */
	u32 swapchain_image_count;
	/* number of max allowed frames in flight, it is automatically clamped to the number of swapchain images if greater than that 
	 * if it is zero, then the renderer will be created with max frames in flight as 1 */
	u32 max_frames_in_flight;
	/* when enabled, call to vulkan_renderer_update() will block execution on the host until rendering commands finish on device for the current frame
	 * NOTE: if frame_pipelining is disabled, then it is of no use keeping max_frames_in_flight more than 1 */
	bool frame_pipelining;
	/* number of point lights which can be added in a render scene,
	 * this is to preset the descriptor set binding counts for point light shadow maps */
	u32 max_point_lights;
	/* number of spot lights which can be added in a render scene,
	 * this is to preset the descriptor set binding counts for spot light shadow maps */
	u32 max_spot_lights;
	/* number of far (directional) lights which can be added in a render scene,
	 * this is to preset the descriptor set binding counts for directional light shadow maps */
	u32 max_far_lights;
} vulkan_renderer_create_info_t;

typedef struct vulkan_renderer_t
{
	renderer_t* renderer;
	memory_allocator_t* allocator;
	vulkan_allocator_t* vk_allocator;

	IF_DEBUG( string_builder_t* debug_log_builder; )

	vulkan_instance_t* instance;
	vulkan_logical_device_t* logical_device;
	vulkan_physical_device_t* physical_device;
	VkSurfaceKHR vo_surface;
	VkQueue vo_graphics_queue;
	VkCommandPool vo_command_pool;
	VkDescriptorPool vo_descriptor_pool;

	/* index of the current frame, it ranges from 0 (inclusive) to swapchain->image_count (exclusive) */
	u32 current_frame_index;

	struct
	{
		/* number of VkSemaphore or VkFence objects in vo_image_available_semaphores and vo_render_finished_semaphores, or vo_fences */
		u32 primitive_count;
		/* semaphores for max number of frames in flight to indicate presentation has finished for them */
		VkSemaphore* vo_image_available_semaphores;
		/* semaphores for max number of frames in flight to indicate rendering has finished on them */
		VkSemaphore* vo_render_finished_semaphores;
		/* fences */
		VkFence* vo_fences;
	} render_present_sync_primitives;


	/* global_set::screen_info
	 * {
	 * 		uvec2 resolution;  	// resolution of the display (in pixels)
	 * 		uvec2 dpi; 			// dpi of the display (in pixels per inch)
	 * 		uvec2 size; 		// window size (in pixels)
	 * 		mat4 matrix; 		// matrix to project onto the window/screen
	 * } */
	struct
	{
		struct_descriptor_t 	struct_def;
		struct_field_handle_t 	resolution_field;
		struct_field_handle_t 	dpi_field;
		struct_field_handle_t	size_field;
		struct_field_handle_t 	matrix_field;
		vulkan_buffer_t* buffer;
		event_subscription_handle_t update_handle;
	} screen_info;

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

	event_subscription_handle_t swapchain_recreate_handle;

	u32 max_frames_in_flight;
	/* cache of create_info->frame_pipelining */
	bool is_pipeline_frame;

} vulkan_renderer_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_renderer_t* vulkan_renderer_create(vulkan_renderer_create_info_t* create_info);
RENDERER_API void vulkan_renderer_destroy(vulkan_renderer_t* renderer);

/* submits the recorded command buffer to the graphics queue and performs presentation */
RENDERER_API void vulkan_renderer_update(vulkan_renderer_t* renderer);

RENDERER_API bool vulkan_renderer_is_running(vulkan_renderer_t* renderer);

/* begins the command buffer recording */
RENDERER_API void vulkan_renderer_begin_frame(vulkan_renderer_t* renderer);
/* ends the command buffer recording */
RENDERER_API void vulkan_renderer_end_frame(vulkan_renderer_t* renderer);

RENDERER_API void vulkan_renderer_wait_idle(vulkan_renderer_t* renderer);

RENDERER_API render_window_t* vulkan_renderer_get_window(vulkan_renderer_t* renderer);

END_CPP_COMPATIBLE
