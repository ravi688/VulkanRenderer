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

#include <stdio.h> 		// puts

static void renderer_on_window_resize(render_window_t* window, void* renderer);

render_window_t* renderer_get_window(renderer_t* renderer) { return renderer->window; }

vulkan_physical_device_t* get_lowest_score_device(vulkan_physical_device_t** devices, u32 count);

vulkan_physical_device_t* get_highest_score_device(vulkan_physical_device_t** devices, u32 count)
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

DEBUG_BLOCK
(
	void* gpu = (discrete_gpu == NULL) ? integrated_gpu : discrete_gpu;
	log_msg("Using the gpu: %s\n", vulkan_physical_device_get_properties(gpu)->deviceName);
)

	return gpu;
}

//TODO: Wrapp this physical device selection & creation of logical device into a single function
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
	renderer->vk_physical_device = physical_device->handle;


	// create window
	renderer->window = render_window_init(width, height, title, full_screen);
	render_window_subscribe_on_resize(renderer->window, renderer_on_window_resize, renderer);

	// create surface
	render_window_get_vulkan_surface(renderer->window, &(renderer->instance->handle), &(renderer->surface));
	
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
		.queue_family_indice_count = 2,
		.extensions = extensions,
		.extension_count = 1,
		.features = minimum_required_features
	};
	renderer->vk_device = vulkan_logical_device_create(physical_device, &logical_device_create_info)->handle;
	heap_free(minimum_required_features);

	//Create Renderpass
	renderer->vk_render_pass = vk_get_render_pass(renderer->vk_device, VK_FORMAT_B8G8R8A8_SRGB);

	//Create Swapchain
	renderer->swapchain = vulkan_swapchain_create(renderer->window, renderer);

	//Create command buffers
	renderer->vk_command_pool = vk_get_command_pool(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device));
	renderer->vk_command_buffers = vk_get_command_buffers(renderer->vk_device, renderer->vk_command_pool, renderer->swapchain->image_count);

	//Set up graphics queue
	renderer->vk_graphics_queue = vk_get_device_queue(renderer->vk_device, vk_get_graphics_queue_family_index(renderer->vk_physical_device), 0);

	//Create descripter pool
	renderer->vk_descriptor_pool = vk_get_descriptor_pool(renderer->vk_device);

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
		.pSwapchains = &(renderer->swapchain->swapchain),
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(renderer->swapchain->render_finished_semaphore),
		.pImageIndices = &(renderer->swapchain->current_image_index)
	};

	vkQueuePresentKHR(renderer->vk_graphics_queue, &present_info);
	vkQueueWaitIdle(renderer->vk_graphics_queue);
	vkDeviceWaitIdle(renderer->vk_device);
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
	vkDestroyDescriptorPool(renderer->vk_device, renderer->vk_descriptor_pool, NULL);
	vkDestroyRenderPass(renderer->vk_device, renderer->vk_render_pass, NULL);
	vkFreeCommandBuffers(renderer->vk_device, renderer->vk_command_pool, renderer->vk_command_buffers.value1, renderer->vk_command_buffers.value2);
	vkDestroyCommandPool(renderer->vk_device, renderer->vk_command_pool, NULL);
	vkDestroyDevice(renderer->vk_device, NULL);
	vulkan_instance_destroy(renderer->instance);
	vulkan_instance_release_resources(renderer->instance);
	heap_free(renderer);
}

static void renderer_on_window_resize(render_window_t* window, void* _renderer)
{
	renderer_t* renderer = _renderer;
	log_msg("Window is resized: %u, %u\n", window->width, window->height);
	vulkan_swapchain_refresh(renderer->swapchain, renderer);
}
