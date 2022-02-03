
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/render_window.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer, vulkan_swapchain_create_info_t* create_info);
static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer);
static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer);

vulkan_swapchain_t* vulkan_swapchain_new()
{
	vulkan_swapchain_t* swapchain = heap_new(vulkan_swapchain_t);
	memset(swapchain, 0, sizeof(vulkan_swapchain_t));
	return swapchain;
}

vulkan_swapchain_t* vulkan_swapchain_create(renderer_t* renderer, vulkan_swapchain_create_info_t* create_info)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	assert(renderer->window != NULL);
	assert(renderer->surface != VK_NULL_HANDLE);
	vulkan_swapchain_t* swapchain = vulkan_swapchain_new();
	create_swapchain(swapchain, renderer, create_info);
	swapchain->image_available_semaphore = vk_get_semaphore(renderer->logical_device->handle);
	swapchain->render_finished_semaphore = vk_get_semaphore(renderer->logical_device->handle);
	log_msg("Swapchain created successfully\n");
	return swapchain;
}


void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, renderer_t* renderer, vulkan_swapchain_create_info_t* create_info)
{
	destroy_swapchain(swapchain, renderer);
	create_swapchain(swapchain, renderer, create_info);
}

void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	destroy_semaphores(swapchain, renderer);
	destroy_swapchain(swapchain, renderer);
	log_msg("Swapchain destroyed successfully\n");
}

u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	vkAcquireNextImageKHR(renderer->logical_device->handle, swapchain->handle, UINT64_MAX, swapchain->image_available_semaphore, VK_NULL_HANDLE, &(swapchain->current_image_index));
	return swapchain->current_image_index;
}

void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain)
{
	vulkan_image_view_release_resources(swapchain->depth_image_view);
	vulkan_image_release_resources(swapchain->depth_image);
	heap_free(swapchain->framebuffer_image_views);
	heap_free(swapchain->image_views);
	heap_free(swapchain->images);
	heap_free(swapchain->framebuffers);
	heap_free(swapchain);
}

static void create_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer, vulkan_swapchain_create_info_t* create_info)
{
	assert(renderer->vk_render_pass != VK_NULL_HANDLE);

	VkSwapchainCreateInfoKHR swapchain_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = renderer->surface,
		.minImageCount = create_info->image_count,
		.imageFormat = create_info->image_format,
		.imageExtent = create_info->image_extent,
		.imageColorSpace = create_info->image_color_space,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = create_info->present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	};
	swapchain->handle = vk_get_swapchain(renderer->logical_device->handle, &swapchain_create_info);
	swapchain->image_count = create_info->image_count;
	swapchain->image_extent = create_info->image_extent;
	log_msg("Swapchain image count: %u\n", swapchain->image_count);
	log_msg("Swapchain image size: (%u, %u)\n", swapchain->image_extent.width, swapchain->image_extent.height);

	// if the swapchain has to be recreated then no allocation should happen, use *_out versions instead
	if(swapchain->images == NULL)
		swapchain->images = vk_get_images(renderer->logical_device->handle, swapchain->handle).value2;
	else vk_get_images_out(renderer->logical_device->handle, swapchain->handle, swapchain->images, swapchain->image_count);
	
	// if the swapchain has to be recreated then no allocation should happen, use *_out versions instead
	if(swapchain->image_views == NULL)
		swapchain->image_views = vk_get_image_views(renderer->logical_device->handle, create_info->image_format, swapchain->image_count, swapchain->images).value2;
	else vk_get_image_views_out(renderer->logical_device->handle, create_info->image_format, swapchain->image_count, swapchain->images, swapchain->image_views);

	vulkan_image_create_info_t depth_create_info = 
	{
		.flags = 0, 		// optional 
		.type = VK_IMAGE_TYPE_2D,
		.format = create_info->depth_format,
		.width = create_info->image_extent.width,
		.height = create_info->image_extent.height,
		.depth = 1,
		.layer_count = 1,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage_mask = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT
	};
	if((depth_create_info.format == VK_FORMAT_D24_UNORM_S8_UINT) || (depth_create_info.format == VK_FORMAT_D32_SFLOAT_S8_UINT))
		depth_create_info.aspect_mask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	
	// if the swapchain has to be recreated then allocation should happen, use *_no_alloc versions instead
	if(swapchain->depth_image == NULL)
		swapchain->depth_image = vulkan_image_create(renderer, &depth_create_info);
	else 
		vulkan_image_create_no_alloc(renderer, &depth_create_info, swapchain->depth_image);
	
	if(swapchain->depth_image_view == NULL)
		swapchain->depth_image_view = vulkan_image_view_create(swapchain->depth_image, VULKAN_IMAGE_VIEW_TYPE_2D);
	else
		vulkan_image_view_create_no_alloc(swapchain->depth_image, VULKAN_IMAGE_VIEW_TYPE_2D, swapchain->depth_image_view);

	VkImageView* attachments_lists[swapchain->image_count];
	uint32_t attachments_count[swapchain->image_count];

	// if the swapchain has to be recreated then no allocation should happen	
	if(swapchain->framebuffer_image_views == NULL)
		swapchain->framebuffer_image_views = heap_newv(VkImageView, 2 * swapchain->image_count);
	
	for(uint32_t i = 0; i < swapchain->image_count; i++) 
	{
		swapchain->framebuffer_image_views[2 * i + 0] = *(swapchain->image_views + i);
		swapchain->framebuffer_image_views[2 * i + 1] = swapchain->depth_image_view->handle;
		attachments_lists[i] = &swapchain->framebuffer_image_views[2 * i];
		attachments_count[i] = 2;
	}

	// if the swapchain has to be recreated then no allocation should happen, use *_out versions instead
	if(swapchain->framebuffers == NULL)
		swapchain->framebuffers = vk_get_framebuffers(	renderer->logical_device->handle, renderer->vk_render_pass, 
														swapchain->image_extent,
														1,
														&attachments_lists[0],
														&attachments_count[0], swapchain->image_count).value2;
	else
		vk_get_framebuffers_out(	renderer->logical_device->handle, renderer->vk_render_pass, 
									swapchain->image_extent, 
									1, 
									&attachments_lists[0], &attachments_count[0], 
									swapchain->image_count, swapchain->framebuffers);
	swapchain->current_image_index = 0;
}


static void destroy_semaphores(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySemaphore(renderer->logical_device->handle, swapchain->image_available_semaphore, NULL);
	vkDestroySemaphore(renderer->logical_device->handle, swapchain->render_finished_semaphore, NULL);
}

static void destroy_swapchain(vulkan_swapchain_t* swapchain, renderer_t* renderer)
{
	vkDestroySwapchainKHR(renderer->logical_device->handle, swapchain->handle, NULL);
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyFramebuffer(renderer->logical_device->handle, swapchain->framebuffers[i], NULL);
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyImageView(renderer->logical_device->handle, swapchain->image_views[i], NULL);
	vulkan_image_view_destroy(swapchain->depth_image_view);
	vulkan_image_destroy(swapchain->depth_image);
}
