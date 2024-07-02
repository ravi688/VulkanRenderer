/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_image_view.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_command_buffer.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/internal/vulkan/vulkan_queue.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>

static VkImageViewType get_view_type(vulkan_image_view_type_t type);
static u32 get_layer_count(vulkan_image_view_type_t type);

SGE_API vulkan_image_view_t* vulkan_image_view_new(memory_allocator_t* allocator)
{
	vulkan_image_view_t* view = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE_VIEW, vulkan_image_view_t);
	memzero(view, vulkan_image_view_t);
	VULKAN_OBJECT_INIT(view, VULKAN_OBJECT_TYPE_IMAGE_VIEW, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return view;
}

SGE_API vulkan_image_view_t* vulkan_image_view_create(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info)
{
	vulkan_image_view_t* view = vulkan_image_view_new(renderer->allocator);
	vulkan_image_view_create_no_alloc(renderer, create_info, view);
	return view;
}

SGE_API void vulkan_image_view_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info, vulkan_image_view_t OUT view)
{
	VULKAN_OBJECT_MEMZERO(view, vulkan_image_view_t);

	view->renderer = renderer;
	view->type = create_info->view_type;
	view->image = create_info->image;
	view->vo_layout = create_info->vo_layout;
	VkImageViewCreateInfo view_create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = create_info->image->vo_handle,
		.viewType = get_view_type(create_info->view_type),
		.format = create_info->image->vo_format,
		.subresourceRange.aspectMask = create_info->image->vo_aspect_mask,
		.subresourceRange.baseMipLevel = create_info->base_mip_level,
		.subresourceRange.levelCount = create_info->level_count,
		.subresourceRange.baseArrayLayer = create_info->base_array_layer,
		.subresourceRange.layerCount = (create_info->layer_count == 0) ? get_layer_count(create_info->view_type) : create_info->layer_count
	};

	_debug_assert__(view_create_info.subresourceRange.levelCount > 0);
	assert((view_create_info.subresourceRange.baseArrayLayer + view_create_info.subresourceRange.layerCount) <= create_info->image->layer_count);
	
	memcopy(&view->vo_subresource_range, &view_create_info.subresourceRange, VkImageSubresourceRange);
	switch(create_info->image->vo_format)
	{
		case VK_FORMAT_R8G8B8A8_UNORM:
    	case VK_FORMAT_R8G8B8A8_SNORM:
    	case VK_FORMAT_R8G8B8A8_USCALED:
    	case VK_FORMAT_R8G8B8A8_SSCALED:
    	case VK_FORMAT_R8G8B8A8_UINT:
    	case VK_FORMAT_R8G8B8A8_SINT:
    	case VK_FORMAT_R8G8B8A8_SRGB:
    		view_create_info.components = (VkComponentMapping) { VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R };
    	break;
    	default:
    		view_create_info.components = (VkComponentMapping) { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	}
	vkCall(vkCreateImageView(view->renderer->logical_device->vo_handle, &view_create_info, VULKAN_ALLOCATION_CALLBACKS(view->renderer), &view->vo_handle));
}

SGE_API void vulkan_image_view_destroy(vulkan_image_view_t* view)
{
	vkDestroyImageView(view->renderer->logical_device->vo_handle, view->vo_handle, VULKAN_ALLOCATION_CALLBACKS(view->renderer));
	view->vo_handle = VK_NULL_HANDLE;
}

SGE_API void vulkan_image_view_release_resources(vulkan_image_view_t* view)
{
	if(VULKAN_OBJECT_IS_INTERNAL(view))
		memory_allocator_dealloc(view->renderer->allocator, view);
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
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
					OUT dst_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				break;
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					OUT dst_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			OUT src_access = VK_ACCESS_SHADER_READ_BIT;
			OUT src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			switch(finalLayout)
			{
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					OUT dst_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					OUT dst_access = VK_ACCESS_MEMORY_WRITE_BIT;
					OUT dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
				default:
					UNDEFINED_LAYOUT_TRANSITION(oldLayout, finalLayout);
			}
		}
		break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			OUT src_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			OUT src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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
		default:
			UNDEFINED_LAYOUT_TRANSITION(oldLayout, finalLayout);
	}
}

SGE_API void vulkan_image_view_transition_layout_to(vulkan_image_view_t* view, u32 flags, VkImageLayout vo_layout)
{
	if((vo_layout  == view->vo_layout) && (flags != VULKAN_IMAGE_VIEW_DONT_CARE_OLD_LAYOUT))
	{
		log_wrn("Image transtion layout warning | new layout is same as the previous one\n");
		return;
	}
	vulkan_renderer_t* renderer = view->renderer;
	VkImageLayout old_layout = (flags == VULKAN_IMAGE_VIEW_DONT_CARE_OLD_LAYOUT) ? VK_IMAGE_LAYOUT_UNDEFINED : view->vo_layout;
	VkPipelineStageFlags src_pipeline_stage;
	VkPipelineStageFlags dst_pipeline_stage;
	VkAccessFlags src_access_mask;
	VkAccessFlags dst_access_mask;
	get_access_and_stage_flags(&src_access_mask, &dst_access_mask, &src_pipeline_stage, &dst_pipeline_stage, old_layout, vo_layout);
	
	vulkan_command_buffer_begin(renderer->vo_aux_command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vulkan_command_image_layout_transition(renderer->vo_aux_command_buffer, view->image->vo_handle,
			&view->vo_subresource_range,
			old_layout, vo_layout,
			src_access_mask, dst_access_mask,
			src_pipeline_stage, dst_pipeline_stage);
	vulkan_command_buffer_end(renderer->vo_aux_command_buffer);
	vulkan_queue_submit(renderer->vo_graphics_queue, renderer->vo_aux_command_buffer, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_NULL_HANDLE);
	vulkan_queue_wait_idle(renderer->vo_graphics_queue);
	view->vo_layout = vo_layout;
}

u32 get_texel_size_from_format(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_R8G8B8A8_UNORM:
			return 4; // bytes
		default:
			LOG_FETAL_ERR("Unsupported format type: %u\n", format);
	}
	return 0;
}

SGE_API void vulkan_image_view_copy_from_buffer(vulkan_image_view_t* view, vulkan_buffer_t* buffer)
{
	_debug_assert__(view->vo_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	_debug_assert__(view->image->depth == 1);
	vulkan_renderer_t* renderer = view->renderer;
	VkBufferImageCopy region =
	{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = view->vo_subresource_range.aspectMask,
		.imageSubresource.mipLevel = view->vo_subresource_range.baseMipLevel,
		.imageSubresource.baseArrayLayer = view->vo_subresource_range.baseArrayLayer,
		.imageSubresource.layerCount = view->vo_subresource_range.layerCount,
		.imageOffset = (VkOffset3D){ 0, 0, 0 },
		.imageExtent = (VkExtent3D){ view->image->width, view->image->height, view->image->depth }
	};
	vulkan_command_buffer_begin(renderer->vo_aux_command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vkCmdCopyBufferToImage(renderer->vo_aux_command_buffer, buffer->vo_handle, view->image->vo_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	vulkan_command_buffer_end(renderer->vo_aux_command_buffer);
	vulkan_queue_submit(renderer->vo_graphics_queue, renderer->vo_aux_command_buffer, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_NULL_HANDLE);
	vulkan_queue_wait_idle(renderer->vo_graphics_queue);
}

SGE_API void vulkan_image_view_upload_data(vulkan_image_view_t* view, void* data)
{
	_debug_assert__(data != NULL);
	
	// TODO: vulkan_image_get_size(view->image);
	u32 size = view->image->width * view->image->height * get_texel_size_from_format(view->image->vo_format);
	
	// prepare staging buffer
	vulkan_buffer_create_info_t staging_buffer_info = 
	{
		.data = data,
		.size = size,
		.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t staging_buffer;
	vulkan_buffer_create_no_alloc(view->renderer, &staging_buffer_info, &staging_buffer);

	// transition image layout from undefined to transfer destination optimal
	vulkan_image_view_transition_layout_to(view, VULKAN_IMAGE_VIEW_DONT_CARE_OLD_LAYOUT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// copy device memory of staging buffer to the device memory of the image
	vulkan_image_view_copy_from_buffer(view, &staging_buffer);

	// destroy staging buffer and it's device memory
	vulkan_buffer_destroy(&staging_buffer);
}

static VkImageViewType get_view_type(vulkan_image_view_type_t type)
{
	switch(type)
	{
    	case VULKAN_IMAGE_VIEW_TYPE_1D:
    		return VK_IMAGE_VIEW_TYPE_1D;
    	case VULKAN_IMAGE_VIEW_TYPE_2D:
    		return VK_IMAGE_VIEW_TYPE_2D;
    	case VULKAN_IMAGE_VIEW_TYPE_3D:
    		return VK_IMAGE_VIEW_TYPE_3D;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE:
    		return VK_IMAGE_VIEW_TYPE_CUBE;
    	case VULKAN_IMAGE_VIEW_TYPE_1D_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    	case VULKAN_IMAGE_VIEW_TYPE_2D_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    	default:
    		log_wrn("Vulkan image view type is invalid, using VK_IMAGE_VIEW_TYPE_2D\n");
    		return VK_IMAGE_VIEW_TYPE_2D;
	}
}

static u32 get_layer_count(vulkan_image_view_type_t type)
{
	switch(type)
	{
		case VULKAN_IMAGE_VIEW_TYPE_1D:
    	case VULKAN_IMAGE_VIEW_TYPE_2D:
    	case VULKAN_IMAGE_VIEW_TYPE_3D:
    		return 1;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE:
    		return 6;
    	case VULKAN_IMAGE_VIEW_TYPE_1D_ARRAY:
    	case VULKAN_IMAGE_VIEW_TYPE_2D_ARRAY:
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE_ARRAY:
    		log_wrn("Layer count isn't defined for VULKAN_IMAGE_VIEW_TYPE_*_ARRAY types, using 1\n");
    		return 1;
    	default:
    		log_wrn("Vulkan image view type is invalid, using VK_IMAGE_VIEW_TYPE_2D\n");
    		return VK_IMAGE_VIEW_TYPE_2D;
	}
}
