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

	memzero(image, vulkan_image_t);

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
	// TODO
	// heap_free(image);
}

#define UNDEFINED_LAYOUT_TRANSITION(old, final) LOG_FETAL_ERR("Undefined layout transition, %u --> %u\n", old, final)

static void get_access_and_stage_flags(VkAccessFlags OUT src_access, VkAccessFlags OUT dst_access, VkPipelineStageFlags OUT src_stage, VkPipelineStageFlags OUT dst_stage, VkImageLayout oldLayout, VkImageLayout finalLayout)
{
	switch(oldLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		{
			OUT src_access = 0;
			OUT src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			switch(finalLayout)
			{
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					OUT dst_access = VK_ACCESS_TRANSFER_WRITE_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					OUT dst_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				break;
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					OUT dst_access = VK_ACCESS_SHADER_READ_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
				default:
					UNDEFINED_LAYOUT_TRANSITION(oldLayout, finalLayout);
			}
		}
		break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			OUT src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
			OUT src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			switch(finalLayout)
			{
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					OUT dst_access = VK_ACCESS_SHADER_READ_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;
				default:
					UNDEFINED_LAYOUT_TRANSITION(oldLayout, finalLayout);
			}
		}
		break;
	}
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
	get_access_and_stage_flags(&barrier.srcAccessMask, &barrier.dstAccessMask, &src_pipeline_stage, &dst_pipeline_stage, image->vo_layout, vo_layout);
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

RENDERER_API void vulkan_image_upload_data(vulkan_image_t* image, void* data, u32 size)
{
	assert(size > 0);
	assert(data != NULL);
	// prepare staging buffer
	vulkan_buffer_create_info_t staging_buffer_info = 
	{
		.data = data,
		.size = size,
		.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t* staging_buffer = vulkan_buffer_create(image->renderer, &staging_buffer_info);

	// transition image layout from undefined to transfer destination optimal
	vulkan_image_transition_layout_to(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// copy device memory of staging buffer to the device memory of the image
	vulkan_image_copy_from_buffer(image, staging_buffer);

	// destroy staging buffer and it's device memory
	vulkan_buffer_destroy(staging_buffer);
	vulkan_buffer_release_resources(staging_buffer);
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
