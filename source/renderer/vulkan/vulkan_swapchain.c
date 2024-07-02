/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_swapchain.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/internal/vulkan/vulkan_queue.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

static void create_swapchain(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info);
static void destroy_swapchain(vulkan_swapchain_t* swapchain);

SGE_API vulkan_swapchain_t* vulkan_swapchain_new(memory_allocator_t* allocator)
{
	vulkan_swapchain_t* swapchain = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SWAPCHAIN, vulkan_swapchain_t);
	memzero(swapchain, vulkan_swapchain_t);
	VULKAN_OBJECT_INIT(swapchain, VULKAN_OBJECT_TYPE_SWAPCHAIN, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return swapchain;
}

SGE_API vulkan_swapchain_t* vulkan_swapchain_create(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info)
{
	// allocate memory and initialize it
	vulkan_swapchain_t* swapchain = vulkan_swapchain_new(renderer->allocator);
	vulkan_swapchain_create_no_alloc(renderer, create_info, swapchain);
	return swapchain;
}

SGE_API void vulkan_swapchain_create_no_alloc(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info, vulkan_swapchain_t OUT swapchain)
{
	VULKAN_OBJECT_MEMZERO(swapchain, vulkan_swapchain_t);

	swapchain->renderer = renderer;
	// create swapchain, it allocates some memory for the first time
	create_swapchain(swapchain, create_info);
	log_msg("Swapchain created successfully\n");
}


SGE_API void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info)
{
	destroy_swapchain(swapchain);
	create_swapchain(swapchain, create_info);
}

SGE_API void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain)
{
	destroy_swapchain(swapchain);
	
	log_msg("Swapchain destroyed successfully\n");
}

SGE_API void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain)
{
	memory_allocator_dealloc(swapchain->renderer->allocator, swapchain->vo_image_views);
	memory_allocator_dealloc(swapchain->renderer->allocator, swapchain->vo_images);
	if(VULKAN_OBJECT_IS_INTERNAL(swapchain))
		memory_allocator_dealloc(swapchain->renderer->allocator, swapchain);
}

SGE_API u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, VkSemaphore signal_semaphore)
{
	vkCall(vkAcquireNextImageKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, UINT64_MAX, signal_semaphore, VK_NULL_HANDLE, &(swapchain->current_image_index)));
	return swapchain->current_image_index;
}

static void transition_image_to_layout_present_KHR(vulkan_swapchain_t* swapchain)
{
	VkCommandBuffer cb = swapchain->renderer->vo_aux_command_buffer;

	vulkan_command_buffer_begin(cb, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkImageSubresourceRange subresource = 
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	u32 count = swapchain->image_count;
	for(u32 i = 0; i < count; i++)
	{
		vulkan_command_image_layout_transition(cb, swapchain->vo_images[i],
			&subresource,
			/* oldLayout: */ VK_IMAGE_LAYOUT_UNDEFINED,
			/* newLayout: */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			/* srcAccess: */ VK_ACCESS_NONE_KHR,
			/* dstAccess: */ VK_ACCESS_MEMORY_READ_BIT,
			/* srcStage: */ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			/* dstStage: */ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
	}
	vulkan_command_buffer_end(cb);
	vulkan_queue_submit(swapchain->renderer->vo_graphics_queue, cb, 
			/* waitSemaphore: */ VK_NULL_HANDLE, 
			/* waitDstStage: */ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
			/* signalSemaphore: */ VK_NULL_HANDLE,
		 	/* signalFence: */ VK_NULL_HANDLE);
	vulkan_queue_wait_idle(swapchain->renderer->vo_graphics_queue);
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
	vkCall(vkCreateSwapchainKHR(swapchain->renderer->logical_device->vo_handle, &swapchain_create_info, VULKAN_ALLOCATION_CALLBACKS(swapchain->renderer), &swapchain->vo_handle));

	// cache the variables because it is needed everyowhere
	bool image_count_changed = swapchain->image_count != create_info->image_count;
	swapchain->image_count = create_info->image_count;
	swapchain->vo_image_extent = create_info->vo_image_extent;
	swapchain->vo_image_format = create_info->vo_image_format;

	// if the swapchain has to be recreated then no allcation should happen
	if(swapchain->vo_images == NULL || image_count_changed)
	{
		// deallocate the previous image buffer
		if(swapchain->vo_images != NULL)
		{
			memory_allocator_dealloc(swapchain->renderer->allocator, swapchain->vo_images);
			memory_allocator_dealloc(swapchain->renderer->allocator, swapchain->vo_image_views);
		}

		vkCall(vkGetSwapchainImagesKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, &swapchain->image_count, NULL));
		_debug_assert__(swapchain->image_count != 0);
		swapchain->vo_images = memory_allocator_alloc_obj_array(swapchain->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_ARRAY, VkImage, swapchain->image_count);
		swapchain->vo_image_views = memory_allocator_alloc_obj_array(swapchain->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, swapchain->image_count);
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
		vkCall(vkCreateImageView(swapchain->renderer->logical_device->vo_handle, &createInfo, VULKAN_ALLOCATION_CALLBACKS(swapchain->renderer), &swapchain->vo_image_views[i]));
	}
	swapchain->current_image_index = 0;

	transition_image_to_layout_present_KHR(swapchain);
}

static void destroy_swapchain(vulkan_swapchain_t* swapchain)
{
	vkDestroySwapchainKHR(swapchain->renderer->logical_device->vo_handle, swapchain->vo_handle, VULKAN_ALLOCATION_CALLBACKS(swapchain->renderer));
	for(u32 i = 0; i < swapchain->image_count; i++)
		vkDestroyImageView(swapchain->renderer->logical_device->vo_handle, swapchain->vo_image_views[i], VULKAN_ALLOCATION_CALLBACKS(swapchain->renderer));
}
