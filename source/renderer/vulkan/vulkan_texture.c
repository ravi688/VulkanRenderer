
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

vulkan_texture_t* vulkan_texture_new()
{
	vulkan_texture_t* texture = heap_new(vulkan_texture_t);
	memset(texture, 0, sizeof(vulkan_texture_t));
	return texture;
}

vulkan_texture_t* vulkan_texture_create(renderer_t* renderer, vulkan_texture_create_info_t* create_info)
{
	ASSERT_NOT_NULL(renderer);
	ASSERT_NOT_NULL(create_info);
	ASSERT_NOT_NULL(create_info->data);
	assert(create_info->channel_count == 4);

	vulkan_texture_t* texture = vulkan_texture_new();

	/* ------------------- COPY HOST IMAGE MEMORY TO THE GPU LOCAL MEMORY -----------------------------------*/
	void* texture_data = create_info->data;
	uint32_t texture_width = create_info->width, texture_height = create_info->height;
	uint32_t texture_size = texture_width * texture_height * 4;

	/* Staging Buffer */
	vulkan_buffer_create_info_t staging_buffer_info = 
	{
		.data = texture_data,
		.size = texture_size,
		.usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t* staging_buffer = vulkan_buffer_create(renderer, &staging_buffer_info);

	/* Image */
	vulkan_image_create_info_t image_info =
	{
		.type = VK_IMAGE_TYPE_2D,
		.width = texture_width,
		.height = texture_height,
		.depth = 1,
		.format = VK_FORMAT_B8G8R8A8_SRGB,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage_mask = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT
	};
	texture->image = vulkan_image_create(renderer, &image_info);

	/* Transition Image Layout from Undefined to Transfer Destination Optimal */
	vulkan_image_transition_layout_to(texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	/* Copy staging buffer to image buffer */
	vulkan_image_copy_from_buffer(texture->image, staging_buffer);

	/* Transition Image Layout from Transfer Destination Optimal to Shader Read Only Optimal */
	vulkan_image_transition_layout_to(texture->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	/* Destroy Staging buffer and memory */
	vulkan_buffer_destroy(staging_buffer, renderer);
	vulkan_buffer_release_resources(staging_buffer);
	/*--------------------------------------------------------------------------------------------------*/

	/*------------------------------------IMAGE VIEWS AND SAMPLERS--------------------------------------*/
	texture->image_view = vulkan_image_get_image_view(texture->image);

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
	vulkan_image_destroy(texture->image);
}

void vulkan_texture_release_resources(vulkan_texture_t* texture)
{
	ASSERT_NOT_NULL(texture);
	vulkan_image_release_resources(texture->image);
	heap_free(texture);
}
