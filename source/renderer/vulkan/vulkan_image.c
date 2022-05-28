#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_queue.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>


static VkCommandBuffer get_single_time_command_buffer(vulkan_renderer_t* renderer);
static void end_single_time_command_buffer(vulkan_renderer_t* renderer, VkCommandBuffer vo_command_buffer);

RENDERER_API vulkan_image_t* vulkan_image_new()
{
	vulkan_image_t* image = heap_new(vulkan_image_t);
	memset(image, 0, sizeof(vulkan_image_t));
	return image;
}

RENDERER_API void vulkan_image_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* image)
{
	assert(create_info != NULL);
	assert(!((create_info->vo_type == VK_IMAGE_TYPE_2D) && (create_info->depth > 1)));
	assert(create_info->depth != 0);
	assert(image != NULL);
	image->renderer = renderer;
	VkImageCreateInfo image_info =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = create_info->vo_flags,
		.imageType = create_info->vo_type,
		.extent = { .width = create_info->width, .height = create_info->height, .depth = create_info->depth },
		.mipLevels = 1,
		.arrayLayers = create_info->layer_count,
		.format = create_info->vo_format,
		.tiling = create_info->vo_tiling,
		.initialLayout = create_info->vo_layout,
		.usage = create_info->vo_usage_mask,
		.sharingMode = renderer->vo_sharing_mode,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.flags = 0 //optional
	};
	vkCall(vkCreateImage(renderer->logical_device->vo_handle, &image_info, NULL, &image->vo_handle));
	
	// get memory requirements
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(renderer->logical_device->vo_handle, image->vo_handle, &memory_requirements);

	// allocate device memory
	VkMemoryAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = vulkan_physical_device_find_memory_type(renderer->physical_device, memory_requirements.memoryTypeBits, create_info->vo_memory_properties_mask)
	};
	vkCall(vkAllocateMemory(renderer->logical_device->vo_handle, &alloc_info, NULL, &image->vo_memory));
	vkCall(vkBindImageMemory(renderer->logical_device->vo_handle, image->vo_handle, image->vo_memory, 0));
	
	image->vo_type = create_info->vo_type;
	image->vo_format = create_info->vo_format;
	image->vo_aspect_mask = create_info->vo_aspect_mask;
	image->vo_layout = create_info->vo_layout;
	image->width = create_info->width;
	image->height = create_info->height;
	image->depth = create_info->depth;
	image->layer_count = create_info->layer_count;
}


RENDERER_API vulkan_image_t* vulkan_image_create(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info)
{
	vulkan_image_t* image = vulkan_image_new();
	vulkan_image_create_no_alloc(renderer, create_info, image);
	return image;
}

RENDERER_API void vulkan_image_destroy(vulkan_image_t* image)
{
	assert(image != NULL);
	vkDestroyImage(image->renderer->logical_device->vo_handle, image->vo_handle, NULL);
	vkFreeMemory(image->renderer->logical_device->vo_handle, image->vo_memory, NULL);
}

RENDERER_API void vulkan_image_release_resources(vulkan_image_t* image)
{
	heap_free(image);
}


RENDERER_API void vulkan_image_transition_layout_to(vulkan_image_t* image, VkImageLayout vo_layout)
{
	if(vo_layout  == image->vo_layout)
	{
		log_wrn("Image transtion layout warning | new layout is same as the previous one\n");
		return;
	}
	vulkan_renderer_t* renderer = image->renderer;
	VkCommandBuffer vo_command_buffer = get_single_time_command_buffer(renderer);
	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = image->vo_layout,
		.newLayout = vo_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image->vo_handle,
		.subresourceRange.aspectMask = image->vo_aspect_mask,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = image->layer_count,
	};
	VkPipelineStageFlags src_pipeline_stage;
	VkPipelineStageFlags dst_pipeline_stage;
	if((image->vo_layout == VK_IMAGE_LAYOUT_UNDEFINED) && (vo_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_pipeline_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if((image->vo_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (vo_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		src_pipeline_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if((image->vo_layout == VK_IMAGE_LAYOUT_UNDEFINED) && (vo_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL))
	{
		barrier.srcAccessMask = 0;
    	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		src_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_pipeline_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
		LOG_FETAL_ERR("Image layout transition error | transition from %u to %u isn't defined\n", image->vo_layout, vo_layout);
	
	vkCmdPipelineBarrier(vo_command_buffer, src_pipeline_stage, dst_pipeline_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
	end_single_time_command_buffer(renderer, vo_command_buffer);
	image->vo_layout = vo_layout;
}

RENDERER_API void vulkan_image_copy_from_buffer(vulkan_image_t* image, vulkan_buffer_t* buffer)
{
	assert(image->vo_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vulkan_renderer_t* renderer = image->renderer;
	VkCommandBuffer vo_command_buffer = get_single_time_command_buffer(renderer);
	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = image->vo_aspect_mask,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = image->layer_count,

		.imageOffset = (VkOffset3D){ 0, 0, 0 },
		.imageExtent = (VkExtent3D){ image->width, image->height, image->depth }
	};
	vkCmdCopyBufferToImage(vo_command_buffer, buffer->vo_handle, image->vo_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	end_single_time_command_buffer(renderer, vo_command_buffer);
}


static VkCommandBuffer get_single_time_command_buffer(vulkan_renderer_t* renderer)
{
	VkCommandBuffer vo_buffer = vulkan_command_buffer_allocate(renderer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, renderer->vo_command_pool);
	vulkan_command_buffer_begin(vo_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	return vo_buffer;
}

static void end_single_time_command_buffer(vulkan_renderer_t* renderer, VkCommandBuffer vo_command_buffer)
{
	vkEndCommandBuffer(vo_command_buffer);
	vulkan_queue_submit(renderer->vo_graphics_queue, vo_command_buffer, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);
	vulkan_queue_wait_idle(renderer->vo_graphics_queue);
	vkFreeCommandBuffers(renderer->logical_device->vo_handle, renderer->vo_command_pool, 1, &vo_command_buffer);
}
