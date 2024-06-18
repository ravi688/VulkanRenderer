/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_renderer.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_queue.h>
#include <renderer/internal/vulkan/vulkan_to_string.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <hpml/affine_transformation.h>
#include <renderer/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>
#include <renderer/render_window.h>
#include <renderer/system/display.h>
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <renderer/defines.h>
#include <renderer/alloc.h>
#include <renderer/renderer.h>
#include <renderer/memory_allocator.h>

#include <stdio.h> 		// puts

UNUSED_FUNCTION static void memory_allocator_dump(memory_allocator_t* allocator, const char* const file_path)
{
	memory_allocation_tree_t* tree = memory_allocator_build_allocation_tree(allocator);
	memory_allocation_tree_serialize_to_file(tree, file_path);
	memory_allocation_tree_destroy(tree);
}

static void recreate_swapchain(void* _window, void* renderer);
static void update_screen_info(void* _window, void* renderer);

RENDERER_API render_window_t* vulkan_renderer_get_window(vulkan_renderer_t* renderer) { return renderer->window; }

UNUSED_FUNCTION static vulkan_physical_device_t* get_lowest_score_device(vulkan_physical_device_t** devices, u32 count) { /* TODO: */ return NULL; }

static vulkan_physical_device_t* find_physical_device(vulkan_physical_device_t* devices, u32 count, vulkan_renderer_gpu_type_t type)
{
	if(count == 0)
		LOG_FETAL_ERR("No vulkan physical device found\n");

	vulkan_physical_device_t* integrated_gpu = NULL;
	vulkan_physical_device_t* discrete_gpu = NULL;
	vulkan_physical_device_t* cpu_gpu = NULL;
	for(int i = 0; i < count; i++)
	{
		VkPhysicalDeviceProperties* properties = vulkan_physical_device_get_properties(&devices[i]);
		switch(properties->deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				integrated_gpu = &devices[i];
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				discrete_gpu = &devices[i];
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				cpu_gpu = &devices[i];
				break;
			default:
				debug_log_warning("Unhandled VkPhysicalDeviceType: %u", properties->deviceType);
				break;
		}
	}
	if((integrated_gpu == NULL) && (discrete_gpu == NULL) && (cpu_gpu == NULL))
		LOG_FETAL_ERR("No integrated, discrete or cpu based vulkan device found!\n");

	void* gpu = NULL;
	switch(type)
	{
		case VULKAN_RENDERER_GPU_TYPE_INTEGRATED:
			gpu = (integrated_gpu == NULL) ? discrete_gpu : integrated_gpu;
			break;
		case VULKAN_RENDERER_GPU_TYPE_DISCRETE:
			gpu = (discrete_gpu == NULL) ? integrated_gpu : discrete_gpu;
			break;
		case VULKAN_RENDERER_GPU_TYPE_CPU:
			gpu = (cpu_gpu == NULL) ? ((integrated_gpu != NULL) ? integrated_gpu : discrete_gpu) : cpu_gpu;
			break;
		default:
			gpu = (discrete_gpu == NULL) ? integrated_gpu : discrete_gpu;
			break;
	}
	LOG_MSG("Using the gpu: %s\n", vulkan_physical_device_get_properties(gpu)->deviceName);
	return gpu;
}

static VkSurfaceFormatKHR find_surface_format(VkSurfaceFormatKHR* surface_formats, u32 count, VkFormat preferred_format, VkColorSpaceKHR preferred_colorspace)
{
	for(u32 i = 0; i < count; i++)
	{
		VkSurfaceFormatKHR surface_format = surface_formats[i];
		if((surface_format.format == preferred_format) && (surface_format.colorSpace == preferred_colorspace))
		{
			log_msg("Preferred surface format and color space found\n");
			return surface_format;
		}
	}
	// TODO: Also print the preferred format and color space in string format.
	LOG_WRN("Preferred surface format and colorspace isn't found\n");
	return surface_formats[0];
}

static VkPresentModeKHR find_present_mode(VkPresentModeKHR* present_modes, u32 count, VkPresentModeKHR preferred_mode)
{
	for(u32 i = 0; i < count; i++)
	{
		if(present_modes[i] == preferred_mode)
		{
			log_msg("Preferred present mode found\n");
			return preferred_mode;
		}
	}
	// TODO: Also print the preferred present mode
	LOG_WRN("Preferred present mode isn't found\n");
	return VK_PRESENT_MODE_FIFO_KHR; 		// only the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available
}

static inline u32 clamp_u32(u32 value, u32 min, u32 max)
{
	return (value > max) ? max : ((value < min) ? min : value);
}

static VkExtent2D find_extent(VkSurfaceCapabilitiesKHR* surface_capabilities, render_window_t* window)
{
	if(surface_capabilities->currentExtent.width == U32_MAX)			// if it is already set
		return surface_capabilities->currentExtent;
	u32 width, height;
	render_window_get_framebuffer_extent(window, &width, &height);
	return (VkExtent2D)
	{
		clamp_u32(width, surface_capabilities->minImageExtent.width, surface_capabilities->maxImageExtent.width),
		clamp_u32(height, surface_capabilities->minImageExtent.height, surface_capabilities->maxImageExtent.height)
	};
}

static vulkan_descriptor_set_layout_t create_global_set_layout(vulkan_renderer_t* renderer);
static vulkan_descriptor_set_layout_t create_object_set_layout(vulkan_renderer_t* renderer);
static vulkan_descriptor_set_layout_t create_camera_set_layout(vulkan_renderer_t* renderer);
static void setup_global_set(vulkan_renderer_t* renderer);
static VkSemaphore create_semaphore(vulkan_renderer_t* renderer)
{
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	vkCall(vkCreateSemaphore(renderer->logical_device->vo_handle, &createInfo, VULKAN_ALLOCATION_CALLBACKS(renderer), &semaphore));
	return semaphore;
}
static VkSemaphore* create_semaphores(vulkan_renderer_t* renderer, u32 count)
{
	VkSemaphore* semaphores = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SEMAPHORE_ARRAY, VkSemaphore, count);
	for(u32 i = 0; i < count; i++)
		semaphores[i] = create_semaphore(renderer);
	return semaphores;
}
static INLINE_IF_RELEASE_MODE void destroy_semaphores(vulkan_renderer_t* renderer, VkSemaphore* semaphores, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vkDestroySemaphore(renderer->logical_device->vo_handle, semaphores[i], VULKAN_ALLOCATION_CALLBACKS(renderer));
	memory_allocator_dealloc(renderer->allocator, semaphores);
}
static VkFence create_fence(vulkan_renderer_t* renderer, VkFenceCreateFlagBits flags)
{
	VkFence fence;
	VkFenceCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags };
	vkCall(vkCreateFence(renderer->logical_device->vo_handle, &createInfo, VULKAN_ALLOCATION_CALLBACKS(renderer), &fence));
	return fence;
}
static INLINE_IF_RELEASE_MODE VkFence* create_fences(vulkan_renderer_t* renderer, u32 count, VkFenceCreateFlagBits flags)
{
	VkFence* fences = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_FENCE_ARRAY, VkFence, count);
	for(u32 i = 0; i < count; i++)
		fences[i] = create_fence(renderer, flags);
	return fences;
}
static void destroy_fences(vulkan_renderer_t* renderer, VkFence* fences, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vkDestroyFence(renderer->logical_device->vo_handle, fences[i], VULKAN_ALLOCATION_CALLBACKS(renderer));
	memory_allocator_dealloc(renderer->allocator, fences);
}

#ifdef PLATFORM_LINUX
#	define PLATFORM_SPECIFIC_VK_SURFACE_EXTENSION "VK_KHR_xcb_surface"
#elif defined(PLATFORM_WINDOWS)
#	define PLATFORM_SPECIFIC_VK_SURFACE_EXTENSION "VK_KHR_win32_surface"
#endif

static struct_descriptor_t create_screen_info_struct(memory_allocator_t* allocator)
{
	struct_descriptor_t screen_info_struct;
	OBJECT_INIT(&screen_info_struct, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	/* definition of the 'screen info' struct */
	struct_descriptor_begin(allocator, &screen_info_struct, "screen_info", 0, GLSL_MEMORY_LAYOUT_CALLBACKS);
		struct_descriptor_add_field(&screen_info_struct, "resolution", GLSL_TYPE_UVEC2);
		struct_descriptor_add_field(&screen_info_struct, "dpi", GLSL_TYPE_UVEC2);
		struct_descriptor_add_field(&screen_info_struct, "size", GLSL_TYPE_UVEC2);
		struct_descriptor_add_field(&screen_info_struct, "matrix", GLSL_TYPE_MAT4);
	struct_descriptor_end(allocator, &screen_info_struct);
	return screen_info_struct;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void destroy_screen_info_struct(memory_allocator_t* allocator, struct_descriptor_t* screen_info)
{
	struct_descriptor_destroy(allocator, screen_info);
}

RENDERER_API vulkan_renderer_t* vulkan_renderer_create(vulkan_renderer_create_info_t* create_info)
{
	renderer_t* _renderer = create_info->renderer; 
	vulkan_renderer_gpu_type_t prefered_gpu_type = create_info->prefered_gpu_type;
	u32 width = create_info->width;
	u32 height = create_info->height;
	const char* title = create_info->title; 	
	bool full_screen = create_info->full_screen;
	bool resizable = create_info->resizable;

	vulkan_renderer_t* renderer = memory_allocator_alloc_obj(_renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDERER, vulkan_renderer_t);
	memzero(renderer, vulkan_renderer_t);
	renderer->renderer = _renderer;
	renderer->allocator = _renderer->allocator;
	renderer->vk_allocator = vulkan_allocator_create(_renderer->allocator);
	IF_DEBUG( renderer->debug_log_builder = _renderer->debug_log_builder );
	renderer->is_pipeline_frame = create_info->frame_pipelining;

	// create a vulkan instance with extensions VK_KHR_surface, VK_KHR_win32_surface
	const char* extensions[3] = { "VK_KHR_surface", PLATFORM_SPECIFIC_VK_SURFACE_EXTENSION };
	#ifdef ENABLE_VULKAN_VALIDATION_LAYERS
	const char* layers[] = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_KHRONOS_synchronization2" };
	#endif /* ENABLE_VULKAN_VALIDATION_LAYERS */
	renderer->instance = vulkan_instance_create(renderer, extensions, 2
	#ifdef ENABLE_VULKAN_VALIDATION_LAYERS
		, layers, SIZEOF_ARRAY(layers)
	#else
		, NULL, 0
	#endif /* ENABLE_VULKAN_VALIDATION_LAYERS */
		);

	vulkan_physical_device_t* physical_devices = vulkan_instance_get_physical_devices(renderer->instance);
	u32 physical_device_count = vulkan_instance_get_physical_device_count(renderer->instance);

DEBUG_BLOCK
(
	BUFFER log_buffer = memory_allocator_buf_create(renderer->allocator, sizeof(char), 0, 0);
	vulkan_instance_to_string(renderer->instance, &log_buffer);
	for(u32 i = 0; i < physical_device_count; i++)
		vulkan_physical_device_to_string(&physical_devices[i], &log_buffer);
	buf_push_char(&log_buffer, 0);
	log_msg(buf_get_ptr(&log_buffer));
)

	vulkan_physical_device_t* physical_device = find_physical_device(physical_devices, physical_device_count, prefered_gpu_type);
	renderer->physical_device = physical_device;


	// create window
	renderer->window = render_window_init(renderer->allocator, width, height, title, full_screen, resizable);
	event_subscription_create_info_t subscription =
	{
		.handler = EVENT_HANDLER(recreate_swapchain),
		.handler_data = (void*)renderer,
		.wait_for = SIGNAL_NOTHING_BIT,
		/* swapchain creation involes image creation (internally though), and image view creation */
		.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT
	};
	renderer->swapchain_recreate_handle = event_subscribe(renderer->window->on_resize_event, &subscription);

	// create surface
	render_window_get_vulkan_surface(renderer->window, renderer, &(renderer->vo_surface));

	VkSurfaceCapabilitiesKHR surface_capabilities = vulkan_physical_device_get_surface_capabilities(physical_device, renderer->vo_surface);
DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	vk_surface_capabilities_to_string("Surface capabilities: \n", &surface_capabilities, &log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)

	// setup the surface format
	u32 surface_format_count;
	VkSurfaceFormatKHR* surface_formats = vulkan_physical_device_get_surface_formats(physical_device, renderer->vo_surface, &surface_format_count);
	VkSurfaceFormatKHR surface_format = find_surface_format(surface_formats, surface_format_count, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	buf_push_string(&log_buffer, "Selected surface format:");
	vk_surface_format_to_string("\t", surface_format, &log_buffer);
	buf_push_string(&log_buffer, "Supported surface formats: \n");
	for(u32 i = 0 ; i < surface_format_count; i++)
		vk_surface_format_to_string("\t\t", surface_formats[i], &log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)
	memory_allocator_dealloc(renderer->allocator, surface_formats);

	// setup the present mode
	u32 present_mode_count;
	VkPresentModeKHR* present_modes = vulkan_physical_device_get_present_modes(physical_device, renderer->vo_surface, &present_mode_count);
	VkPresentModeKHR present_mode = find_present_mode(present_modes, present_mode_count, VK_PRESENT_MODE_MAILBOX_KHR);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	buf_push_string(&log_buffer, "Selected present mode:");
	vk_present_mode_to_string("\t", present_mode, &log_buffer);
	buf_push_string(&log_buffer, "Supported present modes: \n");
	for(u32 i = 0; i < present_mode_count; i++)
		vk_present_mode_to_string("\t\t", present_modes[i], &log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)
	memory_allocator_dealloc(renderer->allocator, present_modes);

	// setup swap extent
	VkExtent2D image_extent = find_extent(&surface_capabilities, renderer->window);

	// setup image count
	// NOTE: if VkSurfaceCapabilitiesKHR::maxImageCount equals 0, then there is no maximum limit for image count
	u32 image_count = clamp_u32(create_info->swapchain_image_count, surface_capabilities.minImageCount, (surface_capabilities.maxImageCount == 0) ? U32_MAX : surface_capabilities.maxImageCount);
	debug_log_info("Image count: %" PRIu32, image_count);
	
	// create logical device
	VkPhysicalDeviceFeatures* minimum_required_features = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PHYSICAL_DEVICE_FEATURES, VkPhysicalDeviceFeatures);
	memzero(minimum_required_features, VkPhysicalDeviceFeatures);

	u32 queue_family_indices[2] =
	{
		vulkan_physical_device_find_queue_family_index(physical_device, VK_QUEUE_GRAPHICS_BIT),			// graphics queue family index
		vulkan_physical_device_find_queue_family_index_for_surface(physical_device, renderer->vo_surface)	// presentation queue family index
	};
	if(queue_family_indices[0] == U32_MAX)
		LOG_FETAL_ERR("No queue found supporting graphics capabilities\n");
	if(queue_family_indices[1] == U32_MAX)
		LOG_FETAL_ERR("No queue found supporting presentation capabilities to the created surface\n");

	renderer->vo_sharing_mode = (queue_family_indices[0] == queue_family_indices[1]) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

	log_u32(queue_family_indices[0]);
	log_u32(queue_family_indices[1]);

	extensions[0] = "VK_KHR_swapchain";

	vulkan_logical_device_create_info_t logical_device_create_info =
	{
		.queue_family_indices = queue_family_indices,
		.queue_family_index_count = (queue_family_indices[0] == queue_family_indices[1]) ? 1 : 2,
		.extensions = extensions,
		.extension_count = 1,
		.features = minimum_required_features
	};
	renderer->logical_device = vulkan_logical_device_create(physical_device, &logical_device_create_info);
	memory_allocator_dealloc(renderer->allocator, minimum_required_features);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	buf_push_string(&log_buffer, "Selected color attachment format: ");
	vk_format_to_string("\t", surface_format.format, &log_buffer);
	buf_push_newline(&log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
	buf_free(&log_buffer);
)

	//Set up graphics queue
	vkGetDeviceQueue(renderer->logical_device->vo_handle, queue_family_indices[0], 0, &renderer->vo_graphics_queue);

	// setup command pool
	renderer->vo_command_pool = vulkan_command_pool_create(renderer, queue_family_indices[0], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	// auxiliary command buffer will be used by vulkan_swapchain_create for transitioning its images to presentatble initially
	// so, it must be created before any call to vulkan_swapchain_create or functions alike.
	vulkan_command_buffer_allocatev(renderer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, renderer->vo_command_pool, 1, &renderer->vo_aux_command_buffer);

	//Create Swapchain
	vulkan_swapchain_create_info_t swapchain_info =
	{
		.image_count = image_count,
		.vo_image_format = surface_format.format,
		.vo_image_color_space = surface_format.colorSpace,
		.vo_image_extent = image_extent,
		.queue_family_indices = queue_family_indices,
		.queue_family_index_count = (queue_family_indices[0] == queue_family_indices[1]) ? 1 : 2,
		.vo_present_mode = present_mode
	};
	memcopy(&renderer->swapchain_create_info, &swapchain_info, vulkan_swapchain_create_info_t);
	renderer->swapchain = vulkan_swapchain_create(renderer, &swapchain_info);

	image_count = renderer->swapchain->image_count;
	renderer->max_frames_in_flight = clamp_u32(create_info->max_frames_in_flight, 1u, image_count);
	if(create_info->max_frames_in_flight != renderer->max_frames_in_flight)
		DEBUG_LOG_WARNING("Requested max number of in flight frames %" PRIu32 " can't be allowed, it is now set to: %" PRIu32, create_info->max_frames_in_flight, renderer->max_frames_in_flight);
	else debug_log_info("Max number of in flight frames: %" PRIu32, renderer->max_frames_in_flight);

	// create semaphores
	renderer->render_present_sync_primitives.primitive_count = renderer->max_frames_in_flight;
	renderer->render_present_sync_primitives.vo_image_available_semaphores = create_semaphores(renderer, renderer->render_present_sync_primitives.primitive_count);
	renderer->render_present_sync_primitives.vo_render_finished_semaphores = create_semaphores(renderer, renderer->render_present_sync_primitives.primitive_count);
	renderer->render_present_sync_primitives.vo_fences = create_fences(renderer, renderer->render_present_sync_primitives.primitive_count, 
														renderer->is_pipeline_frame ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

	// setup command buffers
	// update the image_count variable as the actual number of images allocated for the swapchain might be greater than what had been requested
	renderer->vo_command_buffers = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_CMD_BUFFER_ARRAY, VkCommandBuffer, renderer->max_frames_in_flight);
	vulkan_command_buffer_allocatev(renderer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, renderer->vo_command_pool, renderer->max_frames_in_flight, renderer->vo_command_buffers);
	log_msg("Command Buffers has been allocated successfully\n");

	//Create descripter pool
	VkDescriptorPoolSize sizes[3] =
	{
		{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 10 },
		{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 40 },
		{ .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 10 }
	};
	VkDescriptorPoolCreateInfo pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = 3,
		.pPoolSizes = &sizes[0],
		.maxSets = 60,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
	};
	vkCall(vkCreateDescriptorPool(renderer->logical_device->vo_handle, &pool_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &renderer->vo_descriptor_pool));
	log_msg("Descriptor pool has been allocated successfully\n");

	renderer->global_set_layout = create_global_set_layout(renderer);
	renderer->object_set_layout = create_object_set_layout(renderer);
	renderer->camera_set_layout = create_camera_set_layout(renderer);
	setup_global_set(renderer);

	/* create GPU buffer (till now the logical device has been created) to store window details to be made available to shaders */
	renderer->screen_info.struct_def = create_screen_info_struct(renderer->allocator);
	/* calculate the size of the struct and offsets of the fields inside it */
	struct_descriptor_recalculate(&renderer->screen_info.struct_def);
	/* get the field handles, to later use them to modify the field values */
	renderer->screen_info.resolution_field = struct_descriptor_get_field_handle(&renderer->screen_info.struct_def, "resolution");
	renderer->screen_info.dpi_field = struct_descriptor_get_field_handle(&renderer->screen_info.struct_def, "dpi");
	renderer->screen_info.size_field = struct_descriptor_get_field_handle(&renderer->screen_info.struct_def, "size");
	renderer->screen_info.matrix_field = struct_descriptor_get_field_handle(&renderer->screen_info.struct_def, "matrix");
	/* create gpu buffer with the required size */
	vulkan_buffer_create_info_t buffer_create_info =
	{
		.size = struct_descriptor_sizeof(&renderer->screen_info.struct_def),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	renderer->screen_info.buffer = vulkan_buffer_create(renderer, &buffer_create_info);

	/* write to the descriptor with binding VULKAN_DESCRIPTOR_BINDING_SCREEN */
	vulkan_descriptor_set_write_uniform_buffer(&renderer->global_set, VULKAN_DESCRIPTOR_BINDING_SCREEN, renderer->screen_info.buffer);

	/* map the gpu buffer to the host memory */
	struct_descriptor_map(&renderer->screen_info.struct_def, vulkan_buffer_map(renderer->screen_info.buffer));

	/* set the display resolution value */
	AUTO resolution = display_get_resolution();
	struct_descriptor_set_uvec2(&renderer->screen_info.struct_def, renderer->screen_info.size_field, CAST_TO(uint*, &resolution));

	/* set the display dpi value */
	AUTO dpi = display_get_dpi();
	struct_descriptor_set_uvec2(&renderer->screen_info.struct_def, renderer->screen_info.dpi_field, CAST_TO(uint*, &dpi));

	/* set the screen size and screen matrix
	 * NOTE: this function is called whenever the window is resized, but we are calling it here for the first time to populate
	 * window size and screen projection matrix, otherwise the values for the 'matrix' and 'window_size' fields in 'displayInfo' UBO will be garbage */
	update_screen_info(renderer->window, renderer);

	subscription = (event_subscription_create_info_t)
	{
		.handler = EVENT_HANDLER(update_screen_info),
		.handler_data = (void*)renderer,
		.wait_for = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_NOTHING_BIT
	};
	renderer->screen_info.update_handle = event_subscribe(renderer->window->on_resize_event, &subscription);

	// create the shader and material library
	renderer->shader_library = vulkan_shader_library_create(renderer);
	renderer->material_library = vulkan_material_library_create(renderer, renderer->shader_library);

	// create render pass pool
	renderer->render_pass_pool = vulkan_render_pass_pool_create(renderer);

	// create camera system
	renderer->camera_system = vulkan_camera_system_create(renderer);
	return renderer;
}

static void setup_global_set(vulkan_renderer_t* renderer)
{
	vulkan_descriptor_set_create_info_t set_create_info =
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &renderer->global_set_layout
	};
	vulkan_descriptor_set_create_no_alloc_ext(renderer, &set_create_info, &renderer->global_set);
	log_msg("Global descriptor set has been created successfully\n");
}

static vulkan_descriptor_set_layout_t create_object_set_layout(vulkan_renderer_t* renderer)
{
	VkDescriptorSetLayoutBinding binding =
	{
		.binding = VULKAN_DESCRIPTOR_BINDING_TRANSFORM,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
	};

	var (vulkan_descriptor_set_layout_t, layout);
	vulkan_descriptor_set_layout_create_no_alloc_ext(renderer, &binding, 1, ptr (layout));
	log_msg("Object descriptor set layout has been created successfully\n");
	return val (layout);
}

static vulkan_descriptor_set_layout_t create_camera_set_layout(vulkan_renderer_t* renderer)
{
	VkDescriptorSetLayoutBinding binding =
	{
		.binding = VULKAN_DESCRIPTOR_BINDING_CAMERA_PROPERTIES,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
	};

	var (vulkan_descriptor_set_layout_t, layout);
	vulkan_descriptor_set_layout_create_no_alloc_ext(renderer, &binding, 1, ptr (layout));
	log_msg("Camera descriptor set layout has been created successfully\n");
	return val (layout);
}

static vulkan_descriptor_set_layout_t create_global_set_layout(vulkan_renderer_t* renderer)
{
	VkDescriptorSetLayoutBinding bindings[3] =
	{
		{
			.binding = VULKAN_DESCRIPTOR_BINDING_CAMERA,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
		},
		{
			.binding = VULKAN_DESCRIPTOR_BINDING_LIGHT,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
		},
		{
			.binding = VULKAN_DESCRIPTOR_BINDING_SCREEN,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
		}
	};

	var (vulkan_descriptor_set_layout_t, layout);
	vulkan_descriptor_set_layout_create_no_alloc_ext(renderer, bindings, 3, ptr (layout));
	log_msg("Global descriptor set layout has been created successfully\n");
	return val (layout);
}

static void vulkan_wait_current_frame_commands_to_finish(vulkan_renderer_t* renderer)
{
	u32 current_frame_index = renderer->current_frame_index;
	
	/* wait for the rendering for the frame with index 'current_frame_index' to finish before we can use its resources like 
	 * command buffers, and swapchain images etc. */
	vkCall(vkWaitForFences(renderer->logical_device->vo_handle, 1, &renderer->render_present_sync_primitives.vo_fences[current_frame_index], VK_TRUE, U64_MAX));
	vkCall(vkResetFences(renderer->logical_device->vo_handle, 1, &renderer->render_present_sync_primitives.vo_fences[current_frame_index]));
}

RENDERER_API void vulkan_renderer_begin_frame(vulkan_renderer_t* renderer)
{
	u32 current_frame_index = renderer->current_frame_index;
	
	/* if frame pipelining is enabled, then wait for the execution of commands to finish for this frame
	 * we do not have to wait if frame pipelining is disabled as it is already done in vulkan_render_end_frame() */
	if(renderer->is_pipeline_frame)
		vulkan_wait_current_frame_commands_to_finish(renderer);

	vulkan_swapchain_acquire_next_image(renderer->swapchain, renderer->render_present_sync_primitives.vo_image_available_semaphores[current_frame_index]);

	VkCommandBuffer cb = renderer->vo_command_buffers[current_frame_index];

	// WARNING: enabling command buffer reset and dragging the window results in a crash, not sure why?
	vulkan_command_buffer_reset(cb, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	vulkan_command_buffer_begin(cb, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkImageSubresourceRange subresource =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	vulkan_command_image_layout_transition(cb, renderer->swapchain->vo_images[renderer->swapchain->current_image_index],
		&subresource,
		/* oldLayout: */ VK_IMAGE_LAYOUT_UNDEFINED,
		/* newLayout: */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		/* srcAccess: */ VK_ACCESS_MEMORY_READ_BIT,
		/* dstAccess: */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		/* srcStage: */ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		/* dstStage: */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

RENDERER_API void vulkan_renderer_end_frame(vulkan_renderer_t* renderer)
{
	u32 current_frame_index = renderer->current_frame_index;
	VkCommandBuffer cb = renderer->vo_command_buffers[current_frame_index];

	VkImageSubresourceRange subresource =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	vulkan_command_image_layout_transition(cb, renderer->swapchain->vo_images[renderer->swapchain->current_image_index],
		&subresource,
		/* oldLayout: */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		/* newLayout: */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		/* srcAccess: */ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		/* dstAccess: */ VK_ACCESS_MEMORY_READ_BIT,
		/* srcStage: */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		/* dstStage: */ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

	vulkan_command_buffer_end(renderer->vo_command_buffers[current_frame_index]);
}

RENDERER_API void vulkan_renderer_update(vulkan_renderer_t* renderer)
{
	u32 current_frame_index = renderer->current_frame_index;
	VkCommandBuffer cb = renderer->vo_command_buffers[current_frame_index];
	vulkan_queue_submit(renderer->vo_graphics_queue,
								cb,
								renderer->render_present_sync_primitives.vo_image_available_semaphores[current_frame_index],
								VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								renderer->render_present_sync_primitives.vo_render_finished_semaphores[current_frame_index],
								renderer->render_present_sync_primitives.vo_fences[current_frame_index]);
	/* if frame pipelining is not enabled, then wait for the execution of commands to finish for this frame */
	if(!renderer->is_pipeline_frame)
		vulkan_wait_current_frame_commands_to_finish(renderer);

	// NOTE: if this returns 'false', that means we need to recreate our swapchain and its images,
	// however, we can ignore the return value here, because the swapchain will be recreated automatically whenever window is resized
	vulkan_queue_present(renderer->vo_graphics_queue,
						renderer->swapchain->vo_handle,
						renderer->swapchain->current_image_index,
						renderer->render_present_sync_primitives.vo_render_finished_semaphores[current_frame_index]);
	render_window_poll_events(renderer->window);
	renderer->current_frame_index = (1u + renderer->current_frame_index) % renderer->max_frames_in_flight;
}

RENDERER_API bool vulkan_renderer_is_running(vulkan_renderer_t* renderer)
{
	return !render_window_should_close(renderer->window);
}

RENDERER_API void vulkan_renderer_wait_idle(vulkan_renderer_t* renderer)
{
	vulkan_queue_wait_idle(renderer->vo_graphics_queue);
}

RENDERER_API void vulkan_renderer_destroy(vulkan_renderer_t* renderer)
{
	vulkan_renderer_wait_idle(renderer);

	event_unsubscribe(renderer->window->on_resize_event, renderer->screen_info.update_handle);
	event_unsubscribe(renderer->window->on_resize_event, renderer->swapchain_recreate_handle);

	// destroy camera system
	vulkan_camera_system_destroy(renderer->camera_system);
	vulkan_camera_system_release_resources(renderer->camera_system);

	// destroy the render pass pool
	vulkan_render_pass_pool_destroy(renderer->render_pass_pool);
	vulkan_render_pass_pool_release_resources(renderer->render_pass_pool);

	// destroy the material library
	vulkan_material_library_destroy(renderer->material_library);
	vulkan_material_library_release_resources(renderer->material_library);

	// destroy the shader library
	vulkan_shader_library_destroy(renderer->shader_library);
	vulkan_shader_library_release_resources(renderer->shader_library);

	/* unmap the gpu buffer's data before destroying it */
	vulkan_buffer_unmap(renderer->screen_info.buffer);
	vulkan_buffer_destroy(renderer->screen_info.buffer);
	vulkan_buffer_release_resources(renderer->screen_info.buffer);

	// release resources for screen info struct definition and its fields
	struct_descriptor_destroy(renderer->allocator, &renderer->screen_info.struct_def);

	// destroy global set
	vulkan_descriptor_set_destroy(&renderer->global_set);
	vulkan_descriptor_set_release_resources(&renderer->global_set);

	// destroy set layouts
	vulkan_descriptor_set_layout_destroy(&renderer->camera_set_layout);
	vulkan_descriptor_set_layout_release_resources(&renderer->camera_set_layout);
	vulkan_descriptor_set_layout_destroy(&renderer->object_set_layout);
	vulkan_descriptor_set_layout_release_resources(&renderer->object_set_layout);
	vulkan_descriptor_set_layout_destroy(&renderer->global_set_layout);
	vulkan_descriptor_set_layout_release_resources(&renderer->global_set_layout);

	vkDestroyDescriptorPool(renderer->logical_device->vo_handle, renderer->vo_descriptor_pool, VULKAN_ALLOCATION_CALLBACKS(renderer));
	
	vkFreeCommandBuffers(renderer->logical_device->vo_handle, renderer->vo_command_pool, renderer->max_frames_in_flight, renderer->vo_command_buffers);
	memory_allocator_dealloc(renderer->allocator, renderer->vo_command_buffers);

	// destroy swapchain
	vulkan_swapchain_destroy(renderer->swapchain);
	vulkan_swapchain_release_resources(renderer->swapchain);

	vkFreeCommandBuffers(renderer->logical_device->vo_handle, renderer->vo_command_pool, 1, &renderer->vo_aux_command_buffer);

	vkDestroyCommandPool(renderer->logical_device->vo_handle, renderer->vo_command_pool, VULKAN_ALLOCATION_CALLBACKS(renderer));

	// destroy fences
	destroy_fences(renderer, renderer->render_present_sync_primitives.vo_fences, renderer->render_present_sync_primitives.primitive_count);

	// destroy semaphores
	destroy_semaphores(renderer, renderer->render_present_sync_primitives.vo_image_available_semaphores, renderer->render_present_sync_primitives.primitive_count);
	destroy_semaphores(renderer, renderer->render_present_sync_primitives.vo_render_finished_semaphores, renderer->render_present_sync_primitives.primitive_count);

	// destroy logical device
	vulkan_logical_device_destroy(renderer->logical_device);
	vulkan_logical_device_release_resources(renderer->logical_device);

	// NOTE: VkSurfaceKHR must be destroyed after the swapchain
	vkDestroySurfaceKHR(renderer->instance->handle, renderer->vo_surface, VULKAN_ALLOCATION_CALLBACKS(renderer));
	render_window_destroy(renderer->window);

	// destroy instance
	vulkan_instance_destroy(renderer->instance);
	vulkan_instance_release_resources(renderer->instance);

	vulkan_allocator_destroy(renderer->vk_allocator);
	memory_allocator_dealloc(renderer->allocator, renderer);
	LOG_MSG("Renderer exited successfully\n");
}

static void recreate_swapchain(void* _window, void* _renderer)
{
	AUTO window = CAST_TO(render_window_t*, _window);
	vulkan_renderer_t* renderer = _renderer;
	VkSurfaceCapabilitiesKHR surface_capabilities = vulkan_physical_device_get_surface_capabilities(renderer->physical_device, renderer->vo_surface);
	renderer->swapchain_create_info.vo_image_extent = find_extent(&surface_capabilities, window);
	vulkan_swapchain_refresh(renderer->swapchain, &renderer->swapchain_create_info);
	debug_log_info("Swapchain recreate success");
}

static void update_screen_info(void* _window, void* _renderer)
{
	AUTO window = CAST_TO(render_window_t*, _window);
	vulkan_renderer_t* renderer = _renderer;

	/* set the window size value */
	AUTO size = iextent2d(window->width, window->height);
	struct_descriptor_set_uvec2(&renderer->screen_info.struct_def, renderer->screen_info.size_field, CAST_TO(uint*, &size));

	/* set the screen matrix */
	AUTO matrix = mat4_ortho_projection(-0.04f, 100, size.height, (f32)size.width / (f32)size.height);
	mat4_move(matrix, mat4_transpose(matrix));
	struct_descriptor_set_mat4(&renderer->screen_info.struct_def, renderer->screen_info.matrix_field, CAST_TO(f32*, &matrix));
}

