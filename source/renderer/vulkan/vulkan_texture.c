#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

/*
	description:
		creates a default sampler with linear filtering and repeat mode
	params:
		renderer: pointer to vulkan_renderer_t object
	returns:
		VkSampler object
 */
static VkSampler get_default_sampler(vulkan_renderer_t* renderer);

/*
	description:
		creates a cube sampler with linear filtering and clamped mode
	params:
		renderer: pointer to vulkan_renderer_t object
	returns:
		VkSampler object
 */
static VkSampler get_cubmap_sampler(vulkan_renderer_t* renderer);

/*
	description:
		creats a 2d vulkan texture object
	params:
		texture: pointer to the vulkan_texture_t object created with vulkan_texture_new()
		data: pointer to the vulkan_texture_data_t object (representing only 1 object)
		type: valid values are VULKAN_TEXTURE_TYPE_ALBEDO and VULKAN_TEXTURE_TYPE_NORMAL. VULKAN_TEXTURE_TYPE_CUBE shouldn't be used here, though no error would occur.
	returns:
		nothing
 */
static void vulkan_texture_create_2d(vulkan_texture_t* texture, vulkan_texture_data_t* data, vulkan_texture_type_t type);

/*
	description:
		creats a 3d vulkan texture object
	params:
		texture: pointer to the vulkan_texture_t object created with vulkan_texture_new()
		data: pointer to the vulkan_texture_data_t object (representing only 1 object)
	returns:
		nothing
 */
static void vulkan_texture_create_3d(vulkan_texture_t* texture, vulkan_texture_data_t* data);

/*
	description:
		creats a cube map vulkan texture object
	params:
		texture: pointer to the vulkan_texture_t object created with vulkan_texture_new()
		data: pointer to the vulkan_texture_data_t object (representing array of texture data)
		count: number of vulkan_texture_data_t object that 'data' points to; valid values are 1 and 6 only
	returns:
		nothing
 */
static void vulkan_texture_create_cube(vulkan_texture_t* texture, vulkan_texture_data_t* data, u32 count);


RENDERER_API vulkan_texture_t* vulkan_texture_new()
{
	vulkan_texture_t* texture = heap_new(vulkan_texture_t);
	memset(texture, 0, sizeof(vulkan_texture_t));
	return texture;
}

RENDERER_API vulkan_texture_t* vulkan_texture_create(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info)
{
	assert(create_info != NULL);
	assert(create_info->data != NULL);
	assert(create_info->data_count != 0);
	vulkan_texture_t* texture = heap_new(vulkan_texture_t);
	vulkan_texture_create_no_alloc(renderer, create_info, texture);
	return texture;
}

RENDERER_API void vulkan_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info, vulkan_texture_t OUT texture)
{
	// for now every texture must have channel count equals to 4
#ifdef GLOBAL_DEBUG
	for(u32 i = 0; i < create_info->data_count; i++)
		assert(create_info->data[i].channel_count == 4);
#endif // GLOBAL_DEBUG

	memzero(texture, vulkan_texture_t);

	texture->renderer = renderer;
	texture->vo_descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	switch(create_info->type)
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
			vulkan_texture_create_2d(texture, create_info->data, create_info->type);		
		break;
		case VULKAN_TEXTURE_TYPE_CUBE:
			vulkan_texture_create_cube(texture, create_info->data, create_info->data_count);
		break;
		default:
			LOG_FETAL_ERR("Invalid vulkan texture type\n");
	}
}

RENDERER_API void vulkan_texture_destroy(vulkan_texture_t* texture)
{
	assert(texture != NULL);
	vkDestroySampler(texture->renderer->logical_device->vo_handle, texture->vo_image_sampler, NULL);
	vulkan_image_view_destroy(&texture->image_view);
	vulkan_image_destroy(&texture->image);
}

RENDERER_API void vulkan_texture_release_resources(vulkan_texture_t* texture)
{
	assert(texture != NULL);
	vulkan_image_view_release_resources(&texture->image_view);
	vulkan_image_release_resources(&texture->image);
	// TODO
	// heap_free(texture);
}


static void vulkan_texture_create_2d(vulkan_texture_t* texture, vulkan_texture_data_t* data, vulkan_texture_type_t type)
{
	void* texture_data = data->data;
	u32 texture_width = data->width;
	u32 texture_height = data->height;
	u32 texture_size = texture_width * texture_height * 4;

	// prepare staging buffer
	vulkan_buffer_create_info_t staging_buffer_info = 
	{
		.data = texture_data,
		.size = texture_size,
		.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t* staging_buffer = vulkan_buffer_create(texture->renderer, &staging_buffer_info);

	// create image
	
	VkFormat format;
	switch(type)
	{
		// for albedo the format should be VK_FORMAT_R8G8B8A8_SRGB for best color
		case VULKAN_TEXTURE_TYPE_ALBEDO:
			format = VK_FORMAT_R8G8B8A8_SRGB;
		break;
		case VULKAN_TEXTURE_TYPE_NORMAL:
			format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	vulkan_image_create_info_t image_info =
	{
		.vo_flags = 0,		// optional
		.vo_type = VK_IMAGE_TYPE_2D,
		.width = texture_width,
		.height = texture_height,
		.depth = 1,
		.layer_count = 1,
		.vo_format = format,
		.vo_tiling = VK_IMAGE_TILING_OPTIMAL,
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.vo_usage_mask = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.vo_memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.vo_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT
	};
	vulkan_image_create_no_alloc(texture->renderer, &image_info, &texture->image);

	// transition image layout from undefined to transfer destination optimal
	vulkan_image_transition_layout_to(&texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// copy device memory of staging buffer to the device memory of the image
	vulkan_image_copy_from_buffer(&texture->image, staging_buffer);

	// transition image layout from transfer destination optimal to shader read only optimal
	vulkan_image_transition_layout_to(&texture->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// destroy staging buffer and it's device memory
	vulkan_buffer_destroy(staging_buffer);
	vulkan_buffer_release_resources(staging_buffer);

	// create 2d image view
	vulkan_image_view_create_info_t view_create_info = { .image = &texture->image, .view_type = VULKAN_IMAGE_VIEW_TYPE_2D };
 	vulkan_image_view_create_no_alloc(texture->renderer, &view_create_info, &texture->image_view);
	
	// create default sampler
	texture->vo_image_sampler = get_default_sampler(texture->renderer);
}

static void vulkan_texture_create_3d(vulkan_texture_t* texture, vulkan_texture_data_t* data);
static void vulkan_texture_create_cube(vulkan_texture_t* texture, vulkan_texture_data_t* data, u32 count)
{
	// for now, we will be using 6 separate textures for a cube map texture
	assert(count == 6);

	u32 texture_width = data->width;
	u32 texture_height = data->height;
	u32 texture_size = texture_width * texture_height * 4;
	u8 channel_count = data->channel_count;

	// check if all vulkan_texture_data_t objects have identical dimensions and channel count
	for(u32 i = 1; i < 6; i++)
		if((texture_width != data[i].width) || (texture_height != data[i].height) || (channel_count != data[i].channel_count))
			LOG_FETAL_ERR("For cube map all 6 textures must have identical dimensions and number of channels\n");

	// prepare staging buffer
	vulkan_buffer_create_info_t staging_buffer_info = 
	{
		.data = NULL,		// optional
		.stride = texture_size,
		.count = 6,
		.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t* staging_buffer = vulkan_buffer_create(texture->renderer, &staging_buffer_info);

	// copy textures sequentially into the staging buffer
	for(u32 i = 0; i < 6; i++)
		vulkan_buffer_copy_data(staging_buffer, texture_size * i, data[i].data, texture_size);

	// create image
	vulkan_image_create_info_t image_info =
	{
		.vo_flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,		// required for cube map images
		.vo_type = VK_IMAGE_TYPE_2D,
		.width = texture_width,
		.height = texture_height,
		.depth = 1,
		.layer_count = 6,
		// for VULKAN_TEXTURE_TYPE_CUBE, format should be VK_FORMAT_R8G8B8A8_SRGB for best color
		.vo_format = VK_FORMAT_R8G8B8A8_SRGB,
		.vo_tiling = VK_IMAGE_TILING_OPTIMAL,
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.vo_usage_mask = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.vo_memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.vo_aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT
	};
	vulkan_image_create_no_alloc(texture->renderer, &image_info, &texture->image);

	// transition image layout from undefined to transfer destination optimal
	vulkan_image_transition_layout_to(&texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// copy device memory of staging buffer to the device memory of the image
	vulkan_image_copy_from_buffer(&texture->image, staging_buffer);

	// transition image layout from transfer destination optimal to shader read only optimal
	vulkan_image_transition_layout_to(&texture->image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// destroy staging buffer and it's device memory
	vulkan_buffer_destroy(staging_buffer);
	vulkan_buffer_release_resources(staging_buffer);

	// create 2d image view
	vulkan_image_view_create_info_t view_create_info = { .image = &texture->image, .view_type = VULKAN_IMAGE_VIEW_TYPE_CUBE };
	vulkan_image_view_create_no_alloc(texture->renderer, &view_create_info, &texture->image_view);

	// create default sampler
	texture->vo_image_sampler = get_cubmap_sampler(texture->renderer);
}


static VkSampler get_default_sampler(vulkan_renderer_t* renderer)
{
	VkSamplerCreateInfo create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_NEVER,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f
	};
	VkSampler sampler;
	vkCall(vkCreateSampler(renderer->logical_device->vo_handle, &create_info, NULL, &sampler));
	return sampler;
}

static VkSampler get_cubmap_sampler(vulkan_renderer_t* renderer)
{
	VkSamplerCreateInfo create_info =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_NEVER,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f
	};
	VkSampler sampler;
	vkCall(vkCreateSampler(renderer->logical_device->vo_handle, &create_info, NULL, &sampler));
	return sampler;
}
