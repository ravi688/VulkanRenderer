/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_buffer.c is a part of VulkanRenderer

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
#include <sge/internal/vulkan/vulkan_buffer.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <sge/assert.h>

SGE_API void vulkan_buffer_init(vulkan_buffer_t* buffer)
{
	/* vulkan_buffer_t might be composited into another object's stack memory */
	memzero(buffer, vulkan_buffer_t);
}

SGE_API vulkan_buffer_t* vulkan_buffer_new(memory_allocator_t* allocator)
{
	vulkan_buffer_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_BUFFER, vulkan_buffer_t);
	memzero(buffer, vulkan_buffer_t);
	VULKAN_OBJECT_INIT(buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return buffer;
}


SGE_API vulkan_buffer_t* vulkan_buffer_create(vulkan_renderer_t* renderer, vulkan_buffer_create_info_t* create_info)
{
	vulkan_buffer_t* buffer = vulkan_buffer_new(renderer->allocator);
	vulkan_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

SGE_API void vulkan_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_buffer_create_info_t* create_info, vulkan_buffer_t* buffer)
{
	_debug_assert__(((create_info->stride != 0) && (create_info->count != 0)) || (create_info->size != 0));

	VULKAN_OBJECT_MEMZERO(buffer, vulkan_buffer_t);
	buffer->renderer = renderer;
	u32 buffer_size = (create_info->size == 0) ? (create_info->stride * create_info->count) : create_info->size;
	
	// create vulkan buffer object
	VkBufferCreateInfo buffer_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = buffer_size,
		.usage = create_info->vo_usage_flags,
		.sharingMode = create_info->vo_sharing_mode
	};
	vkCall(vkCreateBuffer(renderer->logical_device->vo_handle, &buffer_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &buffer->vo_handle));

	// get memory requirements
    VkMemoryRequirements vo_memory_requirements;
	vkGetBufferMemoryRequirements(renderer->logical_device->vo_handle, buffer->vo_handle, &vo_memory_requirements);

	// allocate device memory
	VkMemoryAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = vo_memory_requirements.size,
		.memoryTypeIndex = vulkan_physical_device_find_memory_type(renderer->physical_device, vo_memory_requirements.memoryTypeBits, create_info->vo_memory_property_flags)
	};
	vkCall(vkAllocateMemory(renderer->logical_device->vo_handle, &alloc_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &buffer->vo_memory));
	vkCall(vkBindBufferMemory(renderer->logical_device->vo_handle, buffer->vo_handle, buffer->vo_memory, 0));

	buffer->stride = create_info->stride;
	buffer->count = create_info->count;
	buffer->size = buffer_size;
	if(create_info->data != NULL)
		vulkan_buffer_copy_data(buffer, 0, create_info->data, buffer->size);
}

SGE_API void vulkan_buffer_destroy(vulkan_buffer_t* buffer)
{
	vkDestroyBuffer(buffer->renderer->logical_device->vo_handle, buffer->vo_handle, VULKAN_ALLOCATION_CALLBACKS(buffer->renderer));
	vkFreeMemory(buffer->renderer->logical_device->vo_handle, buffer->vo_memory, VULKAN_ALLOCATION_CALLBACKS(buffer->renderer));
}

SGE_API void vulkan_buffer_release_resources(vulkan_buffer_t* buffer)
{
	if(VULKAN_OBJECT_IS_INTERNAL(buffer))
		memory_allocator_dealloc(buffer->renderer->allocator, buffer);
}

SGE_API void vulkan_buffer_copy_data(vulkan_buffer_t* buffer, u32 buffer_offset, void* data, u32 data_size)
{
	_debug_assert__((data != NULL) || (data_size == 0));
	if(data_size == 0)
		return;
	_debug_assert__((buffer_offset + data_size) <= buffer->size);

	void* ptr;
	vkMapMemory(buffer->renderer->logical_device->vo_handle, buffer->vo_memory, 0, buffer->size, 0, &ptr);
	memcopyv(ptr + buffer_offset, data, u8, data_size);
	vkUnmapMemory(buffer->renderer->logical_device->vo_handle, buffer->vo_memory);
}

SGE_API void* vulkan_buffer_map(vulkan_buffer_t* buffer)
{
	_debug_assert_wrn__(buffer->size != 0);
	void* ptr;
	vkMapMemory(buffer->renderer->logical_device->vo_handle, buffer->vo_memory, 0, buffer->size, 0, &ptr);
	IF_DEBUG(buffer->dbg_is_mapped = true);
	return ptr;
}

SGE_API void vulkan_buffer_unmap(vulkan_buffer_t* buffer)
{
	DEBUG_IF(buffer->dbg_is_mapped)
		vkUnmapMemory(buffer->renderer->logical_device->vo_handle, buffer->vo_memory);
	DEBUG_ELSE(
		debug_log_error("Failed to unmap vulkan buffer as it hasn't already been mapped");
		);
}
