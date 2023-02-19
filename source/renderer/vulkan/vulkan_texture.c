#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>

RENDERER_API vulkan_texture_t* vulkan_texture_new(memory_allocator_t* allocator)
{
	vulkan_texture_t* texture = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_TEXTURE, vulkan_texture_t);
	memzero(texture, vulkan_texture_t);
	return texture;
}

RENDERER_API vulkan_texture_t* vulkan_texture_create(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info)
{
	vulkan_texture_t* texture = vulkan_texture_new(renderer->allocator);
	vulkan_texture_create_no_alloc(renderer, create_info, texture);
	return texture;
}

#define UNSUPPORTED_TEXTURE_TYPE(type) LOG_FETAL_ERR("Unsupported texture type: %u\n", type)

static VkFlags get_flags_from_texture_type(vulkan_texture_type_t type)
{
	if(type & VULKAN_TEXTURE_TYPE_CUBE)
		return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
		case VULKAN_TEXTURE_TYPE_DEPTH:
		case VULKAN_TEXTURE_TYPE_COLOR:
			return 0;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
}

static VkImageType get_image_type_from_texture_type(vulkan_texture_type_t type)
{
	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
		case VULKAN_TEXTURE_TYPE_DEPTH:
		case VULKAN_TEXTURE_TYPE_COLOR:
			return VK_IMAGE_TYPE_2D;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
	return 0;
}

static VkFormat get_format_from_texture_type(vulkan_texture_type_t type)
{
	if(type & VULKAN_TEXTURE_TYPE_RENDER_TARGET)
	{
		switch(type & BIT_MASK32(4))
		{
			case VULKAN_TEXTURE_TYPE_ALBEDO:
				return VK_FORMAT_B8G8R8A8_SRGB; 	// swapchain format
			case VULKAN_TEXTURE_TYPE_COLOR:
			case VULKAN_TEXTURE_TYPE_NORMAL:
				LOG_FETAL_ERR("VULKAN_TEXTURE_TYPE_COLOR or VULKAN_TEXTURE_TYPE_NORMAL can't be used as a render target\n");
			break;
			case VULKAN_TEXTURE_TYPE_DEPTH:
				return VK_FORMAT_D32_SFLOAT;
			default:
				UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
		}
	}

	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case VULKAN_TEXTURE_TYPE_COLOR:
		case VULKAN_TEXTURE_TYPE_NORMAL:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case VULKAN_TEXTURE_TYPE_DEPTH:
			return VK_FORMAT_D32_SFLOAT;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
	return 0;
}

#define UNSUPPORTED_TECHNIQUE(technique) LOG_FETAL_ERR("Unsupported render target technique: %u\n", technique)

static VkImageUsageFlags get_usage(vulkan_texture_type_t type, vulkan_texture_usage_t usage, vulkan_render_target_technique_t technique)
{
	VkImageUsageFlags flags = 0;
	if(usage & VULKAN_TEXTURE_USAGE_SAMPLED)
		flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if(usage & VULKAN_TEXTURE_USAGE_TRANSFER_SRC)
		flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if(usage & VULKAN_TEXTURE_USAGE_TRANSFER_DST)
		flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if(usage & VULKAN_TEXTURE_USAGE_ATTACHMENT)
	{
		switch(type & BIT_MASK32(3))
		{
			case VULKAN_TEXTURE_TYPE_ALBEDO:
			case VULKAN_TEXTURE_TYPE_COLOR:
			case VULKAN_TEXTURE_TYPE_NORMAL:
				flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			break;
			case VULKAN_TEXTURE_TYPE_DEPTH:
				flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
			default:
				UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
		}
	}

	if(usage & VULKAN_TEXTURE_USAGE_RENDER_TARGET)
	{	
		assert_wrn((type & VULKAN_TEXTURE_TYPE_RENDER_TARGET) == VULKAN_TEXTURE_TYPE_RENDER_TARGET);

		switch(technique)
		{
			case VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH:
			{
				switch(type & BIT_MASK32(3))
				{
					case VULKAN_TEXTURE_TYPE_ALBEDO:
					case VULKAN_TEXTURE_TYPE_COLOR:
					case VULKAN_TEXTURE_TYPE_NORMAL:
						flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
					break;
					case VULKAN_TEXTURE_TYPE_DEPTH:
						flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
					break;
					default:
						UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
				}
			}
			break;
			case VULKAN_RENDER_TARGET_TECHNIQUE_COPY:
				flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			break;
			default:
				UNSUPPORTED_TECHNIQUE(technique);
		}
	}
	return flags;
}

static VkImageAspectFlags get_aspect(vulkan_texture_type_t type)
{
	VkImageAspectFlags flags = 0;
	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
		case VULKAN_TEXTURE_TYPE_COLOR:
			flags |= VK_IMAGE_ASPECT_COLOR_BIT;
		break;
		case VULKAN_TEXTURE_TYPE_DEPTH:
			flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		break;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
	return flags;
}

static u32 get_layer_count_from_texture_type(vulkan_texture_type_t type)
{
	if(type & VULKAN_TEXTURE_TYPE_CUBE)
		return 6;

	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
		case VULKAN_TEXTURE_TYPE_COLOR:
		case VULKAN_TEXTURE_TYPE_DEPTH:
			return 1;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
	return 0;
}

static vulkan_image_view_type_t get_view_type_from_texture_type(vulkan_texture_type_t type)
{
	if(type & VULKAN_TEXTURE_TYPE_CUBE)
		return VULKAN_IMAGE_VIEW_TYPE_CUBE;

	switch(type & BIT_MASK32(3))
	{
		case VULKAN_TEXTURE_TYPE_ALBEDO:
		case VULKAN_TEXTURE_TYPE_NORMAL:
		case VULKAN_TEXTURE_TYPE_COLOR:
		case VULKAN_TEXTURE_TYPE_DEPTH:
			return VULKAN_IMAGE_VIEW_TYPE_2D;
		default:
			UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
	}
	return 0;
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
	vkCall(vkCreateSampler(renderer->logical_device->vo_handle, &create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &sampler));
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
	vkCall(vkCreateSampler(renderer->logical_device->vo_handle, &create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &sampler));
	return sampler;
}

static VkSampler get_sampler_from_texture_type(vulkan_renderer_t* renderer, vulkan_texture_type_t type)
{
	if(type & VULKAN_TEXTURE_TYPE_CUBE)
		return get_cubmap_sampler(renderer);
	else
	{
		switch(type & BIT_MASK32(3))
		{
			case VULKAN_TEXTURE_TYPE_ALBEDO:
			case VULKAN_TEXTURE_TYPE_NORMAL:
			case VULKAN_TEXTURE_TYPE_COLOR:
			case VULKAN_TEXTURE_TYPE_DEPTH:
				return get_default_sampler(renderer);
			default:
				UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
		}
	}
}

static vulkan_image_view_t* create_write_image_views(vulkan_texture_t* texture, u32 OUT view_count)
{
	if((texture->type & VULKAN_TEXTURE_TYPE_CUBE) != VULKAN_TEXTURE_TYPE_CUBE)
	{
		OUT view_count = 0;
		return NULL;
	}
	_debug_assert__(texture->image.layer_count >= 6);
	OUT view_count = 6;
	vulkan_image_view_t* views = memory_allocator_alloc_obj_array(texture->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE_VIEW_ARRAY, vulkan_image_view_t, 6);
	for(u32 i = 0; i < 6; i++)
	{
		vulkan_image_view_create_info_t create_info = 
		{
			.image = &texture->image,
			.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
			.view_type = VULKAN_IMAGE_VIEW_TYPE_2D,
			.base_mip_level = 0,
			.level_count = 1,
			.base_array_layer = i,
			.layer_count = 1
		};
		vulkan_image_view_create_no_alloc(texture->renderer, &create_info, &views[i]);
	}
	return views;
}

RENDERER_API void vulkan_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info, vulkan_texture_t OUT texture)
{
	memzero(texture, vulkan_texture_t);

	texture->renderer = renderer;
	texture->vo_descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	texture->width = create_info->width;
	texture->height = create_info->height;
	texture->depth = create_info->depth;
	texture->channel_count = create_info->channel_count;
	texture->type = create_info->type;
	texture->initial_usage = create_info->initial_usage;
	texture->usage = create_info->usage;
	texture->final_usage = create_info->final_usage;
	texture->technique = create_info->technique;
	texture->current_stage = VULKAN_TEXTURE_USAGE_STAGE_UNDEFINED;

	_debug_assert__(create_info->depth == 1); 	// for now, the depth must be 1
	_debug_assert__(create_info->channel_count == 4);

	vulkan_image_create_info_t image_info =
	{
		.vo_flags = get_flags_from_texture_type(texture->type),
		.vo_type = get_image_type_from_texture_type(texture->type),
		.width = create_info->width,
		.height = create_info->height,
		.depth = create_info->depth,
		.layer_count = get_layer_count_from_texture_type(texture->type),
		.vo_format = get_format_from_texture_type(texture->type),
		.vo_tiling = VK_IMAGE_TILING_OPTIMAL,
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.vo_usage_mask = get_usage(texture->type, create_info->initial_usage | create_info->usage | create_info->final_usage, create_info->technique),
		.vo_memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.vo_aspect_mask = get_aspect(texture->type)
	};
	vulkan_image_create_no_alloc(texture->renderer, &image_info, &texture->image);

	vulkan_image_view_create_info_t view_create_info = 
	{ 
		.image = &texture->image, 
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.view_type = get_view_type_from_texture_type(texture->type),
		.base_mip_level = 0,
		.level_count = 1,
		.base_array_layer = 0,
		.layer_count = image_info.layer_count
	};
 	vulkan_image_view_create_no_alloc(texture->renderer, &view_create_info, &texture->image_view);

 	texture->image_views = create_write_image_views(texture, &texture->image_view_count);

 	if((create_info->usage | create_info->initial_usage | create_info->final_usage) & VULKAN_TEXTURE_USAGE_SAMPLED)
 		texture->vo_image_sampler = get_sampler_from_texture_type(texture->renderer, texture->type);
 	else
 		texture->vo_image_sampler = VK_NULL_HANDLE;

	vulkan_texture_set_usage_stage(texture, VULKAN_TEXTURE_USAGE_STAGE_INITIAL);
}

RENDERER_API void vulkan_texture_destroy(vulkan_texture_t* texture)
{
	if(texture->vo_image_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(texture->renderer->logical_device->vo_handle, texture->vo_image_sampler, VULKAN_ALLOCATION_CALLBACKS(texture->renderer));
		texture->vo_image_sampler = VK_NULL_HANDLE;
	}
	vulkan_image_view_destroy(&texture->image_view);
	for(u32 i = 0; i < texture->image_view_count; i++)
		vulkan_image_view_destroy(&texture->image_views[i]);
	vulkan_image_destroy(&texture->image);
}

RENDERER_API void vulkan_texture_release_resources(vulkan_texture_t* texture)
{
	vulkan_image_view_release_resources(&texture->image_view);
	for(u32 i = 0; i < texture->image_view_count; i++)
		vulkan_image_view_release_resources(&texture->image_views[i]);
	vulkan_image_release_resources(&texture->image);
	
	if(texture->image_views != NULL)
		memory_allocator_dealloc(texture->renderer->allocator, texture->image_views);
	// TODO
	// heap_free(texture);
}

static vulkan_texture_usage_t get_usage_from_stage(vulkan_texture_t* texture, vulkan_texture_usage_stage_t stage)
{
	switch(stage)
	{
		case VULKAN_TEXTURE_USAGE_STAGE_INITIAL:
			return texture->initial_usage;
		case VULKAN_TEXTURE_USAGE_STAGE_USAGE:
			return texture->usage;
		case VULKAN_TEXTURE_USAGE_STAGE_FINAL:
			return texture->final_usage;
		default:
			LOG_FETAL_ERR("Unsupported texture usage stage: %u\n", stage);
	}
	return VULKAN_TEXTURE_USAGE_STAGE_UNDEFINED;
}

static vulkan_texture_usage_t get_current_usage(vulkan_texture_t* texture)
{
	return get_usage_from_stage(texture, texture->current_stage);
}

RENDERER_API void vulkan_texture_set_usage_stage(vulkan_texture_t* texture, vulkan_texture_usage_stage_t stage)
{
	if(stage == texture->current_stage)
	{
		log_wrn("stage == texture->current_stage");
	 	return;
	}
	vulkan_texture_usage_t usage = get_usage_from_stage(texture, stage);
	vulkan_texture_type_t type = texture->type;

	switch(usage)
	{
		case VULKAN_TEXTURE_USAGE_NONE:
		break;
		case VULKAN_TEXTURE_USAGE_SAMPLED:
			switch(type & BIT_MASK32(3))
			{
				case VULKAN_TEXTURE_TYPE_ALBEDO:
				case VULKAN_TEXTURE_TYPE_NORMAL:
				case VULKAN_TEXTURE_TYPE_COLOR:
					vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
					break;
				case VULKAN_TEXTURE_TYPE_DEPTH:
					vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
					break;
				default:
					UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
			}
		break;
		case VULKAN_TEXTURE_USAGE_TRANSFER_DST:
			vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		break;
		case VULKAN_TEXTURE_USAGE_TRANSFER_SRC:
			vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		break;
		case VULKAN_TEXTURE_USAGE_PRESENT:
			vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		break;
		case VULKAN_TEXTURE_USAGE_RENDER_TARGET:
		{
			assert_wrn((type & VULKAN_TEXTURE_TYPE_RENDER_TARGET) == VULKAN_TEXTURE_TYPE_RENDER_TARGET);

			switch(texture->technique)
			{
				case VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH:
				{
					switch(type & BIT_MASK32(3))
					{
						case VULKAN_TEXTURE_TYPE_ALBEDO:
						case VULKAN_TEXTURE_TYPE_NORMAL:
						case VULKAN_TEXTURE_TYPE_COLOR:
							vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
						break;
						case VULKAN_TEXTURE_TYPE_DEPTH:
							vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
						break;
						default:
							UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
					}
				}
				break;
				case VULKAN_RENDER_TARGET_TECHNIQUE_COPY:
					vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				break;
			}
		}
		break;
		case VULKAN_TEXTURE_USAGE_ATTACHMENT:
		{
			switch(type & BIT_MASK32(3))
			{
				case VULKAN_TEXTURE_TYPE_ALBEDO:
				case VULKAN_TEXTURE_TYPE_NORMAL:
				case VULKAN_TEXTURE_TYPE_COLOR:
					vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				break;
				case VULKAN_TEXTURE_TYPE_DEPTH:
					vulkan_image_view_transition_layout_to(&texture->image_view, 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
				break;
				default:
					UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
			}
		}
		break;
	}
	texture->current_stage = stage;
}

RENDERER_API void vulkan_texture_upload_data(vulkan_texture_t* texture, u32 data_count, vulkan_texture_data_t* data)
{
	vulkan_texture_usage_t current_usage = get_current_usage(texture);
	if(current_usage != VULKAN_TEXTURE_USAGE_TRANSFER_DST)
	{
		log_err("Uploading texture data isn't possible as the current texture usage doesn't contain VULKAN_TEXTURE_USAGE_TRANSFER_DST flag\n");
		return;
	}
	
	vulkan_buffer_t staging_buffer;
	vulkan_texture_type_t type = texture->type;
	if(type & VULKAN_TEXTURE_TYPE_CUBE)
	{
		_debug_assert__(data_count == 6);
		for(u32 i = 0; i < data_count; i++)
		{
			assert((data[i].width == texture->width) && (data[i].height == texture->height));
			assert(((data[i].channel_count == texture->channel_count) && (data[i].channel_count == 4)));
			assert((data[i].depth == texture->depth) && (data[i].depth == 1));
		}
		u32 texture_size = data[0].width * data[0].height * data[0].channel_count;
		_debug_assert__(texture_size > 0);
		vulkan_buffer_create_info_t create_info = 
		{
			.count = data_count,
			.stride = texture_size,
			.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.vo_sharing_mode = texture->renderer->vo_sharing_mode,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		vulkan_buffer_create_no_alloc(texture->renderer, &create_info, &staging_buffer);

		for(u32 i = 0; i < data_count; i++)
			vulkan_buffer_copy_data(&staging_buffer, texture_size * i, data[i].data, texture_size);

		vulkan_image_view_copy_from_buffer(&texture->image_view, &staging_buffer);

		vulkan_buffer_destroy(&staging_buffer);
	}
	else
	{
		switch(type & BIT_MASK32(3))
		{
			case VULKAN_TEXTURE_TYPE_ALBEDO:
			case VULKAN_TEXTURE_TYPE_COLOR:
			case VULKAN_TEXTURE_TYPE_DEPTH:
			case VULKAN_TEXTURE_TYPE_NORMAL:
			{
				_debug_assert__(data_count >= 1);
				_debug_assert__(data[0].channel_count == 4);
				vulkan_buffer_create_info_t create_info = 
				{
					.data = data[0].data,
					.size = data[0].width * data[0].height * data[0].channel_count,
					.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.vo_sharing_mode = texture->renderer->vo_sharing_mode,
					.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				};
				vulkan_buffer_create_no_alloc(texture->renderer, &create_info, &staging_buffer);

				vulkan_image_view_copy_from_buffer(&texture->image_view, &staging_buffer);

				vulkan_buffer_destroy(&staging_buffer);
			}
			break;
			default:
				UNSUPPORTED_TEXTURE_TYPE(type & BIT_MASK32(3));
		}
	}
}
