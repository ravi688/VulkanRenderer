#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

static void create_swapchain(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info);
static void destroy_swapchain(vulkan_swapchain_t* swapchain);

RENDERER_API vulkan_swapchain_t* vulkan_swapchain_new()
{
	vulkan_swapchain_t* swapchain = heap_new(vulkan_swapchain_t);
	memset(swapchain, 0, sizeof(vulkan_swapchain_t));
	return swapchain;
}

RENDERER_API vulkan_swapchain_t* vulkan_swapchain_create(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info)
{
	// allocate memory and initialize it
	vulkan_swapchain_t* swapchain = vulkan_swapchain_new();
	vulkan_swapchain_create_no_alloc(renderer, create_info, swapchain);
	return swapchain;
}

RENDERER_API void vulkan_swapchain_create_no_alloc(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info, vulkan_swapchain_t OUT swapchain)
{
	swapchain->renderer = renderer;
	// create swapchain, it allocates some memory for the first time
	create_swapchain(swapchain, create_info);
	log_msg("Swapchain created successfully\n");
}


RENDERER_API void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info)
{
	destroy_swapchain(swapchain);
	create_swapchain(swapchain, create_info);
	log_msg("Swapchain refreshed successfully\n");
}

RENDERER_API void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain)
{
	destroy_swapchain(swapchain);
	
	log_msg("Swapchain destroyed successfully\n");
}

RENDERER_API void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain)
{
	heap_free(swapchain->vo_image_views);
	heap_free(swapchain->vo_images);
	// TODO
	// heap_free(swapchain);
}

RENDERER_API u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain)
{
	vkAcquireNextImageKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, UINT64_MAX, swapchain->renderer->vo_image_available_semaphore, VK_NULL_HANDLE, &(swapchain->current_image_index));
	return swapchain->current_image_index;
}

static void create_swapchain(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info)
{
	// create vulkan swapchain object
	VkSwapchainCreateInfoKHR swapchain_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = swapchain->renderer->vo_surface,
		.minImageCount = create_info->image_count,
		.imageFormat = create_info->vo_image_format,
		.imageExtent = create_info->vo_image_extent,
		.imageColorSpace = create_info->vo_image_color_space,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = swapchain->renderer->vo_sharing_mode,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.presentMode = create_info->vo_present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
	};
	vkCall(vkCreateSwapchainKHR(swapchain->renderer->logical_device->vo_handle, &swapchain_create_info, NULL, &swapchain->vo_handle));

	// cache the variables because it is needed everyowhere
	bool image_count_changed = swapchain->image_count != create_info->image_count;
	swapchain->image_count = create_info->image_count;
	swapchain->vo_image_extent = create_info->vo_image_extent;
	swapchain->vo_image_format = create_info->vo_image_format;

	log_msg("Swapchain image count: %u\n", swapchain->image_count);
	log_msg("Swapchain image size: (%u, %u)\n", swapchain->vo_image_extent.width, swapchain->vo_image_extent.height);

	// if the swapchain has to be recreated then no allcation should happen
	if(swapchain->vo_images == NULL || image_count_changed)
	{
		// deallocate the previous image buffer
		if(swapchain->vo_images != NULL)
		{
			heap_free(swapchain->vo_images);
			heap_free(swapchain->vo_image_views);
		}

		vkCall(vkGetSwapchainImagesKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, &swapchain->image_count, NULL));
		assert(swapchain->image_count != 0);
		swapchain->vo_images = heap_newv(VkImage, swapchain->image_count);
		swapchain->vo_image_views = heap_newv(VkImageView, swapchain->image_count);
	}
	vkCall(vkGetSwapchainImagesKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, &swapchain->image_count, swapchain->vo_images));

	for(u32 i = 0; i < swapchain->image_count; i++)
	{
		VkImageViewCreateInfo createInfo = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchain->vo_images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = create_info->vo_image_format,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1
		};
		vkCall(vkCreateImageView(swapchain->renderer->logical_device->vo_handle, &createInfo, NULL, &swapchain->vo_image_views[i]));
	}
	swapchain->current_image_index = 0;
}

static void destroy_swapchain(vulkan_swapchain_t* swapchain)
{
	vkDestroySwapchainKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, NULL);
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyImageView(swapchain->renderer->logical_device->vo_handle, swapchain->vo_image_views[i], NULL);
}
