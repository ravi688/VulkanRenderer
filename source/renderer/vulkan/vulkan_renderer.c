//TODO: Write a numeric type conversion warning systme; because of this printf("%u", <float> value) => outputs zero if <float> value < 1

#include <renderer/renderer.h>
#include <renderer/render_window.h>
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <string/string.h>


#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_to_string.h>

#include <stdio.h> 		// puts

static void renderer_on_window_resize(render_window_t* window, void* renderer);

render_window_t* renderer_get_window(renderer_t* renderer) { return renderer->window; }

static vulkan_physical_device_t* get_lowest_score_device(vulkan_physical_device_t** devices, u32 count);

static vulkan_physical_device_t* get_highest_score_device(vulkan_physical_device_t** devices, u32 count)
{
	if(count == 0)
		LOG_FETAL_ERR("No vulkan physical device found\n");

	vulkan_physical_device_t* integrated_gpu = NULL;
	vulkan_physical_device_t* discrete_gpu = NULL;
	for(int i = 0; i < count; i++)
	{
		VkPhysicalDeviceFeatures* features = vulkan_physical_device_get_features(devices[i]);
		VkPhysicalDeviceProperties* properties = vulkan_physical_device_get_properties(devices[i]);
		switch(properties->deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				integrated_gpu = devices[i];
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				discrete_gpu = devices[i];
				break;
		}
	}
	if((integrated_gpu == NULL) && (discrete_gpu == NULL))
		LOG_FETAL_ERR("No integrated or discrete vulkan gpu found!\n");

	void* gpu = (discrete_gpu == NULL) ? integrated_gpu : discrete_gpu;
	// gpu = integrated_gpu;
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

renderer_t* renderer_init(u32 width, u32 height, const char* title, bool full_screen)
{
	renderer_t* renderer = heap_new(renderer_t);
	memset(renderer, 0, sizeof(renderer_t));

	//Create vulkan instance and choose a physical device
	const char* extensions[2] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
	renderer->instance = vulkan_instance_create(extensions, 2);
	vulkan_physical_device_t** physical_devices = vulkan_instance_get_physical_devices(renderer->instance);
	u32 physical_device_count = vulkan_instance_get_physical_device_count(renderer->instance);

DEBUG_BLOCK
(	
	BUFFER log_buffer = buf_create(sizeof(char), 0, 0);
	vulkan_instance_to_string(renderer->instance, &log_buffer);
	for(u32 i = 0; i < physical_device_count; i++)
		vulkan_physical_device_to_string(physical_devices[i], &log_buffer);
	buf_push_char(&log_buffer, 0);
	log_msg(buf_get_ptr(&log_buffer));
)
	
	vulkan_physical_device_t* physical_device = get_highest_score_device(physical_devices, physical_device_count);
	renderer->physical_device = physical_device;


	// create window
	renderer->window = render_window_init(width, height, title, full_screen);
	render_window_subscribe_on_resize(renderer->window, renderer_on_window_resize, renderer);

	// create surface
	render_window_get_vulkan_surface(renderer->window, &(renderer->instance->handle), &(renderer->surface));

	VkSurfaceCapabilitiesKHR surface_capabilities = vulkan_physical_device_get_surface_capabilities(physical_device, renderer->surface);
DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	vk_surface_capabilities_to_string("Surface capabilities: \n", &surface_capabilities, &log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)

	// setup the surface format
	u32 surface_format_count;
	VkSurfaceFormatKHR* surface_formats = vulkan_physical_device_get_surface_formats(physical_device, renderer->surface, &surface_format_count);
	VkSurfaceFormatKHR surface_format = find_surface_format(surface_formats, surface_format_count, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
	buf_push_string(&log_buffer, "Supported surface formats: \n");
	for(u32 i = 0 ; i < surface_format_count; i++)
		vk_surface_format_to_string("\t\t", surface_formats[i], &log_buffer);
	buf_push_null(&log_buffer);
	log_msg(buf_get_ptr(&log_buffer));
)
	heap_free(surface_formats);
	
	// setup the present mode
	u32 present_mode_count;
	VkPresentModeKHR* present_modes = vulkan_physical_device_get_present_modes(physical_device, renderer->surface, &present_mode_count);
	VkPresentModeKHR present_mode = find_present_mode(present_modes, present_mode_count, VK_PRESENT_MODE_MAILBOX_KHR);

DEBUG_BLOCK
(
	buf_set_element_count(&log_buffer, 0);
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
		vulkan_physical_device_find_queue_family_index_for_surface(physical_device, renderer->surface)	// presentation queue family index
	};
	if(queue_family_indices[0] == U32_MAX)
		LOG_FETAL_ERR("No queue found supporting graphics capabilities\n");
	if(queue_family_indices[1] == U32_MAX)
		LOG_FETAL_ERR("No queue found supporting presentation capabilities to the created surface\n");

	extensions[0] = "VK_KHR_swapchain";

	vulkan_logical_device_create_info_t logical_device_create_info =
	{
		.queue_family_indices = queue_family_indices,
		.queue_family_index_count = 2,
		.extensions = extensions,
		.extension_count = 1,
		.features = minimum_required_features
	};
	renderer->logical_device = vulkan_logical_device_create(physical_device, &logical_device_create_info);
	heap_free(minimum_required_features);

	//Create Renderpass
	renderer->vk_render_pass = vk_get_render_pass(renderer->logical_device->handle, VK_FORMAT_B8G8R8A8_SRGB);

	//Create Swapchain
	vulkan_swapchain_create_info_t swapchain_info =
	{
		.image_count = image_count,
		.image_format = surface_format.format,
		.image_color_space = surface_format.colorSpace,
		.image_extent = image_extent,
		.image_sharing_mode = (queue_family_indices[0] == queue_family_indices[1]) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
		.queue_family_indices = (queue_family_indices[0] == queue_family_indices[1]) ? NULL : queue_family_indices,
		.queue_family_index_count = (queue_family_indices[0] == queue_family_indices[1]) ? 0 : 2,
		.present_mode = present_mode
	};
	memcpy(&renderer->swapchain_create_info, &swapchain_info, sizeof(vulkan_swapchain_create_info_t));
	renderer->swapchain = vulkan_swapchain_create(renderer, &swapchain_info);

	//Create command buffers
	renderer->vk_command_pool = vk_get_command_pool(renderer->logical_device->handle, vk_get_graphics_queue_family_index(renderer->physical_device->handle));
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->logical_device->handle, renderer->vk_command_pool, renderer->swapchain->image_count);

	//Set up graphics queue
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->logical_device->handle, vk_get_graphics_queue_family_index(renderer->physical_device->handle), 0);

	//Create descripter pool
	renderer->vk_descriptor_pool = vk_get_descriptor_pool(renderer->logical_device->handle);

	return renderer;
}


void renderer_begin_frame(renderer_t* renderer, float r, float g, float b, float a)
{
	vulkan_swapchain_acquire_next_image(renderer->swapchain, renderer);
	vkCall(vkResetCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT));
	VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vkCall(vkBeginCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], &begin_info));

	VkClearValue clear_values[2] = 
	{
		{ .color = { .float32 = { r, g, b, a } } },
		{ .depthStencil = { .depth = 1, .stencil = 0 } } 		// depth clear, every fragment would pass the depth test i.e. < 1
	};
	VkRenderPassBeginInfo render_pass_begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = (VkOffset2D) { 0, 0 },
		.renderArea.extent = (VkExtent2D) { renderer->window->width, renderer->window->height },
		.framebuffer = renderer->swapchain->framebuffers[renderer->swapchain->current_image_index],
		.renderPass = renderer->vk_render_pass,
		.clearValueCount = 2,
		.pClearValues = &clear_values[0]
	};

	vkCmdBeginRenderPass(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void renderer_end_frame(renderer_t* renderer)
{
	vkCmdEndRenderPass(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]);
	vkCall(vkEndCommandBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]));
}

void renderer_update(renderer_t* renderer)
{
	uint32_t wait_destination_mask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submit_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index]),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->swapchain->image_available_semaphore),
		.pWaitDstStageMask = &wait_destination_mask,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &(renderer->swapchain->render_finished_semaphore)
	};

	vkQueueSubmit(renderer->vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);

	VkPresentInfoKHR present_info =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains = &(renderer->swapchain->handle),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->swapchain->render_finished_semaphore),
		.pImageIndices = &(renderer->swapchain->current_image_index)
	};

	vkQueuePresentKHR(renderer->vk_graphics_queue, &present_info);
	vkQueueWaitIdle(renderer->vk_graphics_queue);
	vkDeviceWaitIdle(renderer->logical_device->handle);
	render_window_poll_events(renderer->window);
}

bool renderer_is_running(renderer_t* renderer)
{
	return !render_window_should_close(renderer->window);
}

void renderer_terminate(renderer_t* renderer)
{
	vkDestroySurfaceKHR(renderer->instance->handle, renderer->surface, NULL);
	render_window_destroy(renderer->window);
	vulkan_swapchain_destroy(renderer->swapchain, renderer);
	vulkan_swapchain_release_resources(renderer->swapchain);
	vkDestroyDescriptorPool(renderer->logical_device->handle, renderer->vk_descriptor_pool, NULL);
	vkDestroyRenderPass(renderer->logical_device->handle, renderer->vk_render_pass, NULL);
	vkFreeCommandBuffers(renderer->logical_device->handle, renderer->vk_command_pool, renderer->vk_command_buffers.value1, renderer->vk_command_buffers.value2);
	vkDestroyCommandPool(renderer->logical_device->handle, renderer->vk_command_pool, NULL);
	vulkan_logical_device_destroy(renderer->logical_device);
	vulkan_logical_device_release_resources(renderer->logical_device);
	vulkan_instance_destroy(renderer->instance);
	vulkan_instance_release_resources(renderer->instance);
	heap_free(renderer);
	LOG_MSG("Renderer exited successfully\n");
}

static void renderer_on_window_resize(render_window_t* window, void* _renderer)
{
	renderer_t* renderer = _renderer;
	log_msg("Window is resized: %u, %u\n", window->width, window->height);
	VkSurfaceCapabilitiesKHR surface_capabilities = vulkan_physical_device_get_surface_capabilities(renderer->physical_device, renderer->surface);
	renderer->swapchain_create_info.image_extent = find_extent(&surface_capabilities, window);
	vulkan_swapchain_refresh(renderer->swapchain, renderer, &renderer->swapchain_create_info);
}
