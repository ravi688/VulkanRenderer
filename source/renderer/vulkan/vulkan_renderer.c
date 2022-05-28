#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_queue.h>
#include <renderer/internal/vulkan/vulkan_to_string.h>
#include <renderer/render_window.h>
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <renderer/defines.h>
#include <renderer/memory_allocator.h>

#include <stdio.h> 		// puts

static void vulkan_renderer_on_window_resize(render_window_t* window, void* renderer);

RENDERER_API render_window_t* vulkan_renderer_get_window(vulkan_renderer_t* renderer) { return renderer->window; }

static vulkan_physical_device_t* get_lowest_score_device(vulkan_physical_device_t** devices, u32 count);

static vulkan_physical_device_t* find_physical_device(vulkan_physical_device_t* devices, u32 count, vulkan_renderer_gpu_type_t type)
{
	if(count == 0)
		LOG_FETAL_ERR("No vulkan physical device found\n");

	vulkan_physical_device_t* integrated_gpu = NULL;
	vulkan_physical_device_t* discrete_gpu = NULL;
	for(int i = 0; i < count; i++)
	{
		VkPhysicalDeviceFeatures* features = vulkan_physical_device_get_features(&devices[i]);
		VkPhysicalDeviceProperties* properties = vulkan_physical_device_get_properties(&devices[i]);
		switch(properties->deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				integrated_gpu = &devices[i];
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				discrete_gpu = &devices[i];
				break;
		}
	}
	if((integrated_gpu == NULL) && (discrete_gpu == NULL))
		LOG_FETAL_ERR("No integrated or discrete vulkan gpu found!\n");

	void* gpu = NULL;
	switch(type)
	{
		case VULKAN_RENDERER_GPU_TYPE_INTEGRATED:
			gpu = (integrated_gpu == NULL) ? discrete_gpu : integrated_gpu;
			break;
		case VULKAN_RENDERER_GPU_TYPE_DISCRETE:
			gpu = (discrete_gpu == NULL) ? integrated_gpu : discrete_gpu;
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
static void setup_global_set(vulkan_renderer_t* renderer);

RENDERER_API vulkan_renderer_t* vulkan_renderer_init(vulkan_renderer_gpu_type_t preferred_gpu_type, u32 width, u32 height, const char* title, bool full_screen)
{
	vulkan_renderer_t* renderer = heap_new(vulkan_renderer_t);
	memset(renderer, 0, sizeof(vulkan_renderer_t));

	// create a vulkan instance with extensions VK_KHR_surface, VK_KHR_win32_surface
	const char* extensions[2] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
	renderer->instance = vulkan_instance_create(extensions, 2);
	vulkan_physical_device_t* physical_devices = vulkan_instance_get_physical_devices(renderer->instance);
	u32 physical_device_count = vulkan_instance_get_physical_device_count(renderer->instance);

DEBUG_BLOCK
(	
	BUFFER log_buffer = buf_create(sizeof(char), 0, 0);
	vulkan_instance_to_string(renderer->instance, &log_buffer);
	for(u32 i = 0; i < physical_device_count; i++)
		vulkan_physical_device_to_string(&physical_devices[i], &log_buffer);
	buf_push_char(&log_buffer, 0);
	log_msg(buf_get_ptr(&log_buffer));
)
	
	vulkan_physical_device_t* physical_device = find_physical_device(physical_devices, physical_device_count, preferred_gpu_type);
	renderer->physical_device = physical_device;


	// create window
	renderer->window = render_window_init(width, height, title, full_screen);
	render_window_subscribe_on_resize(renderer->window, vulkan_renderer_on_window_resize, renderer);

	// create surface
	render_window_get_vulkan_surface(renderer->window, &(renderer->instance->handle), &(renderer->vo_surface));

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
	heap_free(surface_formats);
	
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
	heap_free(present_modes);

	// setup swap extent
	VkExtent2D image_extent = find_extent(&surface_capabilities, renderer->window);

	// setup image count
	u32 image_count = clamp_u32(surface_capabilities.minImageCount + 1, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

	// create logical device
	VkPhysicalDeviceFeatures* minimum_required_features = heap_new(VkPhysicalDeviceFeatures);
	memset(minimum_required_features, 0, sizeof(VkPhysicalDeviceFeatures));

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
	heap_free(minimum_required_features);

	// setup depth buffer format
	// VkFormat* formats = stack_newv(VkFormat, 3);
	// formats[0] = VK_FORMAT_D32_SFLOAT;
	// formats[1] = VK_FORMAT_D32_SFLOAT_S8_UINT;
	// formats[2] = VK_FORMAT_D24_UNORM_S8_UINT;
	// VkFormat depth_format = vulkan_physical_device_find_supported_format(renderer->physical_device, &formats[0], 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	// stack_free(formats);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	// buf_push_string(&log_buffer, "Selected depth attachment format:");
	// vk_format_to_string("\t", depth_format, &log_buffer);
	// buf_push_newline(&log_buffer);
	buf_push_string(&log_buffer, "Selected color attachment format: ");
	vk_format_to_string("\t", surface_format.format, &log_buffer);
	buf_push_newline(&log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)

	// //Create Renderpass
	// vulkan_render_pass_create_info_t render_pass_info =
	// {
	// 	.color_attachment_format = surface_format.format,
	// 	.depth_attachment_format = depth_format
	// };
	// renderer->render_pass = vulkan_render_pass_create(renderer, &render_pass_info);

	// create a shadow render pass for rendering the shadow map
	// vulkan_render_pass_create_info_t shadow_render_pass_info =
	// {
	// 	// color attachment format is not needed
	// 	.depth_attachment_format = depth_format
	// };
	// renderer->shadow_map_render_pass = vulkan_render_pass_create(renderer, &shadow_render_pass_info);

	//Create Swapchain
	vulkan_swapchain_create_info_t swapchain_info =
	{
		.image_count = image_count,
		.vo_image_format = surface_format.format,
		// .depth_format = depth_format,
		.vo_image_color_space = surface_format.colorSpace,
		.vo_image_extent = image_extent,
		.queue_family_indices = queue_family_indices,
		.queue_family_index_count = (queue_family_indices[0] == queue_family_indices[1]) ? 1 : 2,
		.vo_present_mode = present_mode
	};
	memcpy(&renderer->swapchain_create_info, &swapchain_info, sizeof(vulkan_swapchain_create_info_t));
	renderer->swapchain = vulkan_swapchain_create(renderer, &swapchain_info);

	// setup command pool
	renderer->vo_command_pool = vulkan_command_pool_create(renderer, queue_family_indices[0], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	// setup command buffers
	renderer->vo_command_buffers = heap_newv(VkCommandBuffer, renderer->swapchain->image_count);
	vulkan_command_buffer_allocatev(renderer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, renderer->vo_command_pool, renderer->swapchain->image_count, renderer->vo_command_buffers);
	log_msg("Command Buffers has been allocated successfully\n");

	//Set up graphics queue
	vkGetDeviceQueue(renderer->logical_device->vo_handle, queue_family_indices[0], 0, &renderer->vo_graphics_queue); 

	//Create descripter pool
	VkDescriptorPoolSize sizes[3] =
	{
		{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 8 },
		{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 20 },
		{ .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 3 }
	};
	VkDescriptorPoolCreateInfo pool_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = 3,
		.pPoolSizes = &sizes[0],
		.maxSets = 20,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
	};
	vkCall(vkCreateDescriptorPool(renderer->logical_device->vo_handle, &pool_create_info, NULL, &renderer->vo_descriptor_pool));
	log_msg("Descriptor pool has been allocated successfully\n");

	renderer->global_set_layout = create_global_set_layout(renderer);
	renderer->object_set_layout = create_object_set_layout(renderer);
	setup_global_set(renderer);

	// create the shader and material library
	renderer->shader_library = vulkan_shader_library_create(renderer);
	renderer->material_library = vulkan_material_library_create(renderer, renderer->shader_library);
	
	// create render pass pool
	renderer->render_pass_pool = vulkan_render_pass_pool_create(renderer);	

	return renderer;
}

static void setup_global_set(vulkan_renderer_t* renderer)
{
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &renderer->global_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, &renderer->global_set);
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

	vulkan_descriptor_set_layout_t layout;
	vulkan_descriptor_set_layout_create_no_alloc(renderer, &binding, 1, &layout);
	log_msg("Object descriptor set layout has been created successfully\n");
	return layout;
}

static vulkan_descriptor_set_layout_t create_global_set_layout(vulkan_renderer_t* renderer)
{
	VkDescriptorSetLayoutBinding bindings[2] = 
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
		}
	};
	vulkan_descriptor_set_layout_t layout;
	vulkan_descriptor_set_layout_create_no_alloc(renderer, bindings, 2, &layout);
	log_msg("Global descriptor set layout has been created successfully\n");
	return layout;	
}


RENDERER_API void vulkan_renderer_begin_frame(vulkan_renderer_t* renderer)
{
	vulkan_swapchain_acquire_next_image(renderer->swapchain);
	vulkan_command_buffer_reset(renderer->vo_command_buffers[renderer->swapchain->current_image_index], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	vulkan_command_buffer_begin(renderer->vo_command_buffers[renderer->swapchain->current_image_index], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// vulkan_render_pass_begin_info_t begin_info =
	// {
	// 	.framebuffer = renderer->swapchain->framebuffers[renderer->swapchain->current_image_index],
	// 	.depth = 1.1f,
	// 	.stencil = 0,
	// 	.color = { r, g, b, a },
	// 	.width = renderer->swapchain->image_extent.width,
	// 	.height = renderer->swapchain->image_extent.height
	// };
	// vulkan_render_pass_begin(renderer->render_pass, &begin_info);
}

RENDERER_API void vulkan_renderer_end_frame(vulkan_renderer_t* renderer)
{
	// vulkan_render_pass_end(renderer->render_pass);
	vulkan_command_buffer_end(renderer->vo_command_buffers[renderer->swapchain->current_image_index]);
}

RENDERER_API void vulkan_renderer_update(vulkan_renderer_t* renderer)
{
	u32 wait_destination_mask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	vulkan_queue_submit(renderer->vo_graphics_queue,
								renderer->vo_command_buffers[renderer->swapchain->current_image_index],
								renderer->swapchain->vo_image_available_semaphore,
								VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								renderer->swapchain->vo_render_finished_semaphore);
	vulkan_queue_present(renderer->vo_graphics_queue, 
						renderer->swapchain->vo_handle, 
						renderer->swapchain->current_image_index,
						renderer->swapchain->vo_render_finished_semaphore);
	vulkan_queue_wait_idle(renderer->vo_graphics_queue);
	vkCall(vkDeviceWaitIdle(renderer->logical_device->vo_handle));
	render_window_poll_events(renderer->window);
}

RENDERER_API bool vulkan_renderer_is_running(vulkan_renderer_t* renderer)
{
	return !render_window_should_close(renderer->window);
}

RENDERER_API void vulkan_renderer_terminate(vulkan_renderer_t* renderer)
{
	// destroy the shader library
	vulkan_shader_library_destroy(renderer->shader_library);
	vulkan_shader_library_release_resources(renderer->shader_library);

	// destroy the material library
	vulkan_material_library_destroy(renderer->material_library);
	vulkan_material_library_release_resources(renderer->material_library);

	// destroy the render pass pool
	vulkan_render_pass_pool_destroy(renderer->render_pass_pool);
	vulkan_render_pass_pool_release_resources(renderer->render_pass_pool);

	vulkan_descriptor_set_destroy(&renderer->global_set);
	vulkan_descriptor_set_release_resources(&renderer->global_set);
	vulkan_descriptor_set_layout_destroy(&renderer->object_set_layout);
	vulkan_descriptor_set_layout_destroy(&renderer->global_set_layout);

	vkDestroySurfaceKHR(renderer->instance->handle, renderer->vo_surface, NULL);
	render_window_destroy(renderer->window);
	
	// destroy swapchain
	vulkan_swapchain_destroy(renderer->swapchain);
	vulkan_swapchain_release_resources(renderer->swapchain);
	
	vkDestroyDescriptorPool(renderer->logical_device->vo_handle, renderer->vo_descriptor_pool, NULL);
	
	// // destroy render pass	
	// vulkan_render_pass_destroy(renderer->render_pass);
	// vulkan_render_pass_release_resources(renderer->render_pass);

	// // destroy the shadow render pass
	// vulkan_render_pass_destroy(renderer->shadow_map_render_pass);
	// vulkan_render_pass_release_resources(renderer->shadow_map_render_pass);
	
	vkFreeCommandBuffers(renderer->logical_device->vo_handle, renderer->vo_command_pool, renderer->swapchain->image_count, renderer->vo_command_buffers);
	vkDestroyCommandPool(renderer->logical_device->vo_handle, renderer->vo_command_pool, NULL);
	
	// destroy logical device
	vulkan_logical_device_destroy(renderer->logical_device);
	vulkan_logical_device_release_resources(renderer->logical_device);
	
	// destroy instance
	vulkan_instance_destroy(renderer->instance);
	vulkan_instance_release_resources(renderer->instance);
	
	heap_free(renderer->vo_command_buffers);
	heap_free(renderer);
	LOG_MSG("Renderer exited successfully\n");
}

static void vulkan_renderer_on_window_resize(render_window_t* window, void* _renderer)
{
	vulkan_renderer_t* renderer = _renderer;
	log_msg("Window is resized: %u, %u\n", window->width, window->height);
	VkSurfaceCapabilitiesKHR surface_capabilities = vulkan_physical_device_get_surface_capabilities(renderer->physical_device, renderer->vo_surface);
	renderer->swapchain_create_info.vo_image_extent = find_extent(&surface_capabilities, window);
	vulkan_swapchain_refresh(renderer->swapchain, &renderer->swapchain_create_info);
}
