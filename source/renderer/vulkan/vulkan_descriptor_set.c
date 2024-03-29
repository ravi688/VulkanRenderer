/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_descriptor_set.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_new(memory_allocator_t* allocator)
{
	vulkan_descriptor_set_t* set = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET, vulkan_descriptor_set_t);
	memzero(set, vulkan_descriptor_set_t);
	set->vo_handle = VK_NULL_HANDLE;
	return set;
}

RENDERER_API void vulkan_descriptor_set_create_no_alloc(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set)
{
	memzero(set, vulkan_descriptor_set_t);
	set->renderer = renderer;
	set->vo_pool = create_info->vo_pool;
	if(create_info->layout->vo_handle == VK_NULL_HANDLE)
	{
		set->vo_handle = VK_NULL_HANDLE;
		return;
	}
	VkDescriptorSetAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = create_info->vo_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &create_info->layout->vo_handle
	};
	vkCall(vkAllocateDescriptorSets(set->renderer->logical_device->vo_handle, &alloc_info, &set->vo_handle));
}

RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_create(vulkan_renderer_t* renderer, vulkan_descriptor_set_create_info_t* create_info)
{
	vulkan_descriptor_set_t* set = vulkan_descriptor_set_new(renderer->allocator);
	vulkan_descriptor_set_create_no_alloc(renderer, create_info, set);
	return set;
}

RENDERER_API void vulkan_descriptor_set_destroy(vulkan_descriptor_set_t* set)
{
	if(set->vo_handle == VK_NULL_HANDLE) return;
	vkCall(vkFreeDescriptorSets(set->renderer->logical_device->vo_handle, set->vo_pool, 1, &set->vo_handle));
	set->vo_handle = VK_NULL_HANDLE;
}

RENDERER_API void vulkan_descriptor_set_release_resources(vulkan_descriptor_set_t* set)
{
	// TODO:
	// heap_free(set);
}

RENDERER_API void vulkan_descriptor_set_bind(vulkan_descriptor_set_t* set, u32 set_number, vulkan_pipeline_layout_t* pipeline_layout)
{
	u32 image_index = set->renderer->swapchain->current_image_index;
	vkCmdBindDescriptorSets(set->renderer->vo_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout->vo_handle, set_number, 1, &set->vo_handle, 0, NULL);
}

static VkImageLayout get_layout(VkFormat format)
{
	switch(format)
	{
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8_SRGB:
		case VK_FORMAT_R8G8_SRGB:
		case VK_FORMAT_R8G8B8_SRGB:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		default:
			LOG_FETAL_ERR("Unsupport format for image layout during descriptor write: %u\n", format);
	}
	return VK_IMAGE_LAYOUT_UNDEFINED;
}

RENDERER_API void vulkan_descriptor_set_write_texture(vulkan_descriptor_set_t* set, u32 binding_index, vulkan_texture_t* texture)
{
	_debug_assert__(texture->image_view.vo_handle != VK_NULL_HANDLE);
	
	// NOTE: texture->vo_image_sampler might be VK_NULL_HANDLE in case of subpassInput
	// _debug_assert__(texture->vo_image_sampler != VK_NULL_HANDLE);
	VkDescriptorImageInfo image_info =
	{
		.imageLayout = get_layout(texture->image.vo_format),
		.imageView = texture->image_view.vo_handle,
		.sampler = texture->vo_image_sampler
	};
	VkWriteDescriptorSet descriptor_write =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = set->vo_handle,
		.dstBinding = binding_index,
		.dstArrayElement = 0,
		.descriptorType = texture->vo_descriptor_type,
		.descriptorCount = 1,
		.pImageInfo = &image_info,
	};
	vkUpdateDescriptorSets(set->renderer->logical_device->vo_handle, 1, &descriptor_write, 0, NULL);
}

RENDERER_API void vulkan_descriptor_set_write_uniform_buffer(vulkan_descriptor_set_t* set, u32 binding_index, vulkan_buffer_t* buffer)
{
	VkDescriptorBufferInfo buffer_info =
	{
		.buffer = buffer->vo_handle,
		.offset = 0,
		.range = buffer->size
	};
	VkWriteDescriptorSet descriptor_write =
	{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = set->vo_handle,
		.dstBinding = binding_index,
		.dstArrayElement = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.pBufferInfo = &buffer_info,
	};
	vkUpdateDescriptorSets(set->renderer->logical_device->vo_handle, 1, &descriptor_write, 0, NULL);
}
