/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_image.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_image.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/assert.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h>

SGE_API vulkan_image_t* vulkan_image_new(memory_allocator_t* allocator)
{
	vulkan_image_t* image = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE, vulkan_image_t);
	memzero(image, vulkan_image_t);
	VULKAN_OBJECT_INIT(image, VULKAN_OBJECT_TYPE_IMAGE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return image;
}

SGE_API void vulkan_image_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* image)
{
	assert(!((create_info->vo_type == VK_IMAGE_TYPE_2D) && (create_info->depth > 1)));
	_debug_assert__(create_info->depth != 0);

	VULKAN_OBJECT_MEMZERO(image, vulkan_image_t);

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
		.samples = VK_SAMPLE_COUNT_1_BIT
	};
	vkCall(vkCreateImage(renderer->logical_device->vo_handle, &image_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &image->vo_handle));
	
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
	vkCall(vkAllocateMemory(renderer->logical_device->vo_handle, &alloc_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &image->vo_memory));
	vkCall(vkBindImageMemory(renderer->logical_device->vo_handle, image->vo_handle, image->vo_memory, 0));
	
	image->vo_type = create_info->vo_type;
	image->vo_format = create_info->vo_format;
	image->vo_aspect_mask = create_info->vo_aspect_mask;
	image->width = create_info->width;
	image->height = create_info->height;
	image->depth = create_info->depth;
	image->layer_count = create_info->layer_count;
}


SGE_API vulkan_image_t* vulkan_image_create(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info)
{
	vulkan_image_t* image = vulkan_image_new(renderer->allocator);
	vulkan_image_create_no_alloc(renderer, create_info, image);
	return image;
}

SGE_API void vulkan_image_destroy(vulkan_image_t* image)
{
	_debug_assert__(image != NULL);
	vkDestroyImage(image->renderer->logical_device->vo_handle, image->vo_handle, VULKAN_ALLOCATION_CALLBACKS(image->renderer));
	image->vo_handle = VK_NULL_HANDLE;
	vkFreeMemory(image->renderer->logical_device->vo_handle, image->vo_memory, VULKAN_ALLOCATION_CALLBACKS(image->renderer));
	image->vo_memory = VK_NULL_HANDLE;
}

SGE_API void vulkan_image_release_resources(vulkan_image_t* image)
{
	if(VULKAN_OBJECT_IS_INTERNAL(image))
		memory_allocator_dealloc(image->renderer->allocator, image);
}
