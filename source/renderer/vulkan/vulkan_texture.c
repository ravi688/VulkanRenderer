
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

vulkan_texture_t* vulkan_texture_create(renderer_t* renderer, vulkan_texture_create_info_t* create_info)
{
	ASSERT_NOT_NULL(renderer);
	ASSERT_NOT_NULL(create_info);
	ASSERT_NOT_NULL(create_info->data);
	assert(create_info->channel_count == 4);

	vulkan_texture_t* texture = heap_new(vulkan_texture_t);
	memset(texture, 0, sizeof(vulkan_texture_t));

	/* ------------------- COPY HOST IMAGE MEMORY TO THE GPU LOCAL MEMORY -----------------------------------*/
	void* texture_data = create_info->data;
	uint32_t texture_width = create_info->width, texture_height = create_info->height;
	uint32_t texture_size = texture_width * texture_height * 4;

	/* Staging Buffer */
	VkBuffer staging_buffer = vk_get_buffer(renderer->vk_device, texture_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
	VkDeviceMemory staging_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, staging_buffer, texture_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(renderer->vk_device, staging_buffer_memory, 0, texture_size, 0, &data);
	memcpy(data, texture_data, texture_size);
	vkUnmapMemory(renderer->vk_device, staging_buffer_memory);

	/* Image */
	VkImageCreateInfo image_info =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.extent = (VkExtent3D) { .width = texture_width, .height = texture_height, .depth = 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.flags = 0 //optional
	};
	vkCall(vkCreateImage(renderer->vk_device, &image_info, NULL, &(texture->image)));
	texture->image_memory = vk_get_device_memory_for_image(renderer->vk_device, renderer->vk_physical_device, texture->image, texture_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	/* Transition Image Layout from Undefined to Transfer Destination Optimal */
	VkCommandBuffer command_buffer = vk_get_begin_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool);
	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = texture->image,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT
	};
	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
	vk_end_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool, command_buffer, renderer->vk_graphics_queue);

	/* Copy staging buffer to image buffer */
	command_buffer = vk_get_begin_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool);
	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = 1,

		.imageOffset = (VkOffset3D){ 0, 0, 0 },
		.imageExtent = (VkExtent3D) { texture_width, texture_height, 1 }
	};
	vkCmdCopyBufferToImage(command_buffer, staging_buffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	vk_end_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool, command_buffer, renderer->vk_graphics_queue);

	/* Transition Image Layout from Transfer Destination Optimal to Shader Read Only Optimal */
	command_buffer = vk_get_begin_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool);
	barrier = (VkImageMemoryBarrier)
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = texture->image,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
	};
	vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
	vk_end_single_time_command_buffer(renderer->vk_device, renderer->vk_command_pool, command_buffer, renderer->vk_graphics_queue);

	/* Destroy Staging buffer and memory */
	vkDestroyBuffer(renderer->vk_device, staging_buffer, NULL);
	vkFreeMemory(renderer->vk_device, staging_buffer_memory, NULL);
	/*--------------------------------------------------------------------------------------------------*/

	/*------------------------------------IMAGE VIEWS AND SAMPLERS--------------------------------------*/
	VkImageViewCreateInfo view_create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = texture->image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1
	};
	vkCall(vkCreateImageView(renderer->vk_device, &view_create_info, NULL, &(texture->image_view)));

	VkSamplerCreateInfo sampler_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f,
	};
	vkCall(vkCreateSampler(renderer->vk_device, &sampler_create_info, NULL, &texture->image_sampler));
	/*------------------------------------------------------------------------------------*/
	return texture;
}

void vulkan_texture_destroy(vulkan_texture_t* texture, renderer_t* renderer)
{
	ASSERT_NOT_NULL(texture);
	ASSERT_NOT_NULL(renderer);
	vkDestroySampler(renderer->vk_device, texture->image_sampler, NULL);
	/* NOTE: VkImageView should be destroyed before VkImage */
	vkDestroyImageView(renderer->vk_device, texture->image_view, NULL);
	vkDestroyImage(renderer->vk_device, texture->image, NULL);
	vkFreeMemory(renderer->vk_device, texture->image_memory, NULL);
}

void vulkan_texture_release_resources(vulkan_texture_t* texture)
{
	ASSERT_NOT_NULL(texture);
	heap_free(texture);
}
