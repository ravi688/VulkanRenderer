#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>

vulkan_image_t* vulkan_image_new()
{
	vulkan_image_t* image = heap_new(vulkan_image_t);
	memset(image, 0, sizeof(vulkan_image_t));
	return image;
}

vulkan_image_t* vulkan_image_create(renderer_t* renderer, vulkan_image_create_info_t* create_info)
{
	assert(create_info != NULL);
	assert(!((create_info->type == VK_IMAGE_TYPE_2D) && (create_info->depth > 1)));
	assert(create_info->depth != 0);
	vulkan_image_t* image = vulkan_image_new();
	image->renderer = renderer;
	VkImageCreateInfo image_info =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = create_info->type,
		.extent = { .width = create_info->width, .height = create_info->height, .depth = create_info->depth },
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = create_info->format,
		.tiling = create_info->tiling,
		.initialLayout = create_info->layout,
		.usage = create_info->usage_mask,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.flags = 0 //optional
	};
	vkCall(vkCreateImage(renderer->vk_device, &image_info, NULL, &image->handle));
	image->memory = vk_get_device_memory_for_image(renderer->vk_device, renderer->vk_physical_device, image->handle, create_info->memory_properties_mask);
	image->type = create_info->type;
	image->format = create_info->format;
	image->aspect_mask = create_info->aspect_mask;
	image->layout = create_info->layout;
	image->width = create_info->width;
	image->height = create_info->height;
	image->depth = create_info->depth;
	return image;
}

void vulkan_image_destroy(vulkan_image_t* image)
{
	assert(image != NULL);
	vkDestroyImage(image->renderer->vk_device, image->handle, NULL);
	vkFreeMemory(image->renderer->vk_device, image->memory, NULL);
}

void vulkan_image_release_resources(vulkan_image_t* image)
{
	heap_free(image);
}


void vulkan_image_transition_layout_to(vulkan_image_t* image, VkImageLayout layout)
{
	if(layout  == image->layout)
	{
		log_wrn("Image transtion layout warning | new layout is same as the previous one\n");
		return;
	}
	renderer_t* renderer = image->renderer;
	VkCommandBuffer command_buffer = vk_get_begin_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool);
	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = image->layout,
		.newLayout = layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image->handle,
		.subresourceRange.aspectMask = image->aspect_mask,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
	};
	VkPipelineStageFlags src_pipeline_stage;
	VkPipelineStageFlags dst_pipeline_stage;
	if((image->layout == VK_IMAGE_LAYOUT_UNDEFINED) && (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_pipeline_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if((image->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		src_pipeline_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
		LOG_FETAL_ERR("Image layout transition error | transition from %u to %u isn't defined\n", image->layout, layout);
	
	vkCmdPipelineBarrier(command_buffer, src_pipeline_stage, dst_pipeline_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
	vk_end_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool, command_buffer, renderer->vk_graphics_queue);
	image->layout = layout;
}

void vulkan_image_copy_from_buffer(vulkan_image_t* image, vulkan_buffer_t* buffer)
{
	assert(image->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	renderer_t* renderer = image->renderer;
	VkCommandBuffer command_buffer = vk_get_begin_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool);
	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = image->aspect_mask,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = 1,

		.imageOffset = (VkOffset3D){ 0, 0, 0 },
		.imageExtent = (VkExtent3D){ image->width, image->height, image->depth }
	};
	vkCmdCopyBufferToImage(command_buffer, buffer->handle, image->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	vk_end_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool, command_buffer, renderer->vk_graphics_queue);
}

VkImageView vulkan_image_create_image_view(vulkan_image_t* image)
{
	VkImageViewCreateInfo view_create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image->handle,
		.viewType = image->type,
		.format = image->format,
		.subresourceRange.aspectMask = image->aspect_mask,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1
	};
	VkImageView image_view;
	vkCall(vkCreateImageView(image->renderer->vk_device, &view_create_info, NULL, &image_view));
	return image_view;
}