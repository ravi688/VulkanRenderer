/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_attachment.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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


#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>

// TODO: remove it after debugging
#include <renderer/debug.h>

RENDERER_API vulkan_attachment_t* vulkan_attachment_new(memory_allocator_t* allocator)
{
	vulkan_attachment_t* attachment = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT, vulkan_attachment_t);
	memzero(attachment, vulkan_attachment_t);
	return attachment;
}

RENDERER_API vulkan_attachment_t* vulkan_attachment_create(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info)
{
	vulkan_attachment_t* attachment = vulkan_attachment_new(renderer->allocator);
	vulkan_attachment_create_no_alloc(renderer, create_info, attachment);
	return attachment;
}

static VkFormat get_image_format_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_FORMAT_D32_SFLOAT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_STENCIL isn't supported yet!\n");
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL isn't supported yet!\n");
		default:
			LOG_FETAL_ERR("Unsupported attachment type %u for any of the image format\n", type);
	}
	return VK_FORMAT_UNDEFINED;
}

static VkImageUsageFlags get_image_usage_from_format(VkFormat format, vulkan_attachment_next_pass_usage_t usage)
{
	VkImageUsageFlags flags = 0;
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			break;
		case VK_FORMAT_D32_SFLOAT:
			flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any of the image usages\n", format);
	}
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT) ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0;
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED) ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
	return flags;
}

static VkImageUsageFlags get_image_usage_from_attachment_type(vulkan_attachment_type_t type, vulkan_attachment_next_pass_usage_t usage)
{
	VkImageUsageFlags flags = 0;
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			 flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			 break;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default:
			LOG_FETAL_ERR("Unsupported attachment type %u for any of the image usages\n", type);
	}
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT) ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0;
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED) ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
	return flags;
}

static VkImageAspectFlags get_image_aspect_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			return VK_IMAGE_ASPECT_STENCIL_BIT;
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			return VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;
		default:
			LOG_FETAL_ERR("Unsupported attachment type : %u\n", type);
	}
	return 0;
}

static VkImageAspectFlags get_image_aspect_from_format(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		default:
			LOG_FETAL_ERR("Unsupported format %u for any of the image aspect flags\n", format);
	}
	return 0;
}


static VkSampler create_sampler(vulkan_renderer_t* renderer)
{
	VkSamplerCreateInfo cInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1.0f, // Optional
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,	// Optional
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 1.0f
	};

	VkSampler sampler;
	vkCall(vkCreateSampler(renderer->logical_device->vo_handle, &cInfo, VULKAN_ALLOCATION_CALLBACKS(renderer), &sampler));
	return sampler;
}

UNUSED_FUNCTION static VkImageLayout get_layout_from_format(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		default:
			LOG_FETAL_ERR("Unsupported attachment format %u\n", format);
	}
	return VK_IMAGE_LAYOUT_UNDEFINED;
}

RENDERER_API void vulkan_attachment_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info, vulkan_attachment_t OUT attachment)
{
	memzero(attachment, vulkan_attachment_t);
	attachment->renderer = renderer;

	// create image for this attachment
	vulkan_image_create_info_t image_create_info = 
	{
		.vo_type = VK_IMAGE_TYPE_2D,
		.vo_format = create_info->format ? create_info->format : get_image_format_from_attachment_type(create_info->type),
		.width = create_info->width,
		.height = create_info->height,
		.depth = 1,
		.layer_count = 1,
		.vo_tiling = VK_IMAGE_TILING_OPTIMAL,
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.vo_usage_mask = create_info->format ? get_image_usage_from_format(create_info->format, create_info->next_pass_usage) : get_image_usage_from_attachment_type(create_info->type, create_info->next_pass_usage),
		.vo_memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.vo_aspect_mask = create_info->format ? get_image_aspect_from_format(create_info->format) : get_image_aspect_from_attachment_type(create_info->type)
	};
	vulkan_image_create_no_alloc(renderer, &image_create_info, &attachment->image);

	// create image view for this attachment
	vulkan_image_view_create_info_t view_create_info =
	{
		.image = &attachment->image,
		.vo_layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.view_type = VULKAN_IMAGE_VIEW_TYPE_2D,
		.base_mip_level = 0,
		.level_count = 1,
		.base_array_layer = 0,
		.layer_count = 1
	};
	vulkan_image_view_create_no_alloc(renderer, &view_create_info, &attachment->image_view);

	// NOTE: in case of VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT we don't need any sampler
	if((create_info->next_pass_usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED) != 0)
	{
		attachment->vo_descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		attachment->vo_sampler = create_sampler(renderer);
	}
	else
	{
		if((create_info->next_pass_usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT) != 0)
			attachment->vo_descriptor_type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		attachment->vo_sampler = VK_NULL_HANDLE;
	}

	/* cache the view create info and image create info to be later used while refreshing the attachment */
	attachment->image_create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_VULKAN_IMAGE_CREATE_INFO, vulkan_image_create_info_t);
	memcopy(attachment->image_create_info, &image_create_info, vulkan_image_create_info_t);
	attachment->view_create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_VULKAN_IMAGE_VIEW_CREATE_INFO, vulkan_image_view_create_info_t);
	memcopy(attachment->view_create_info, &view_create_info, vulkan_image_view_create_info_t);
}

RENDERER_API void vulkan_attachment_destroy(vulkan_attachment_t* attachment)
{
	vulkan_image_destroy(&attachment->image);
	vulkan_image_view_destroy(&attachment->image_view);
	if(attachment->vo_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(attachment->renderer->logical_device->vo_handle, attachment->vo_sampler, VULKAN_ALLOCATION_CALLBACKS(attachment->renderer));
		attachment->vo_sampler = VK_NULL_HANDLE;
	}
}

RENDERER_API void vulkan_attachment_release_resources(vulkan_attachment_t* attachment)
{
	memory_allocator_dealloc(attachment->renderer->allocator, attachment->image_create_info);
	memory_allocator_dealloc(attachment->renderer->allocator, attachment->view_create_info);
	// TODO
	// heap_free(attachment);
}

RENDERER_API void vulkan_attachment_recreate(vulkan_attachment_t* attachment, vulkan_attachment_recreate_info_t* info)
{
	/* recreate VkImage object */
	/* we can't just call vulkan_attachment_destroy as we need to persist the sampler object (recreating that is a redundant thing) */
	vulkan_image_destroy(&attachment->image);
	attachment->image_create_info->width = info->width;
	attachment->image_create_info->height = info->height;
	vulkan_image_create_no_alloc(attachment->renderer, attachment->image_create_info, &attachment->image);
	
	/* recreate VkImageView object */
	vulkan_image_view_destroy(&attachment->image_view);
	vulkan_image_view_create_no_alloc(attachment->renderer, attachment->view_create_info, &attachment->image_view);
}

RENDERER_API vulkan_image_t* vulkan_attachment_get_image(vulkan_attachment_t* attachment)
{
	return &attachment->image;
}

RENDERER_API vulkan_image_view_t* vulkan_attachment_get_image_view(vulkan_attachment_t* attachment)
{
	return &attachment->image_view;
}

