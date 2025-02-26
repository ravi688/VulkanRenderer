/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_instance_buffer.c is a part of VulkanRenderer

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
#include <sge/internal/vulkan/vulkan_instance_buffer.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/assert.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h> 			// memcopyv

// constructors and destructors
SGE_API void vulkan_instance_buffer_create(vulkan_renderer_t* renderer, vulkan_instance_buffer_create_info_t* create_info, vulkan_instance_buffer_t* out_instance_buffer)
{
	_debug_assert__(out_instance_buffer != NULL);
	_debug_assert__(create_info->stride != 0);
	VULKAN_OBJECT_MEMZERO(out_instance_buffer, vulkan_instance_buffer_t);
	multi_buffer_create(create_info->stride, create_info->capacity, &out_instance_buffer->host_buffer);
	vulkan_buffer_init(&out_instance_buffer->device_buffer);
	out_instance_buffer->renderer = renderer;
	out_instance_buffer->has_device_buffer = false;
	if(create_info->capacity > 0)
	{
		vulkan_buffer_create_info_t _create_info =
		{
			.stride = create_info->stride,
			.count = create_info->capacity,
			.vo_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		VULKAN_OBJECT_INIT(&out_instance_buffer->device_buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_buffer_create_no_alloc(renderer, &_create_info, &out_instance_buffer->device_buffer);
		out_instance_buffer->has_device_buffer = true;
	}
}

SGE_API void vulkan_instance_buffer_destroy(vulkan_instance_buffer_t* instance_buffer)
{
	if(instance_buffer->has_device_buffer)
		vulkan_buffer_destroy(&instance_buffer->device_buffer);
	multi_buffer_clear(&instance_buffer->host_buffer);
}

SGE_API void vulkan_instance_buffer_release_resources(vulkan_instance_buffer_t* instance_buffer)
{
	if(instance_buffer->has_device_buffer)
		vulkan_buffer_release_resources(&instance_buffer->device_buffer);
	multi_buffer_free(&instance_buffer->host_buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(instance_buffer))
		memory_allocator_dealloc(instance_buffer->renderer->allocator, instance_buffer);
}

// getters
SGE_API multi_buffer_t* vulkan_instance_buffer_get_host_buffer(vulkan_instance_buffer_t* instance_buffer)
{
	AUTO buffer = &instance_buffer->host_buffer;
	instance_buffer->is_dirty = true;
	return buffer;
}

SGE_API vulkan_buffer_t* vulkan_instance_buffer_get_device_buffer(vulkan_instance_buffer_t* instance_buffer)
{
	if(instance_buffer->has_device_buffer)
		return &instance_buffer->device_buffer;
	else
		return NULL;
}

// logic functions
SGE_API bool vulkan_instance_buffer_commit(vulkan_instance_buffer_t* instance_buffer, bool OUT is_resized)
{
	if(!instance_buffer->is_dirty)
		return false;

	instance_buffer->is_dirty = false;

	multi_buffer_t* host_buffer = &instance_buffer->host_buffer;
	vulkan_buffer_t* device_buffer = &instance_buffer->device_buffer;
	u32 count = multi_buffer_get_combined_sub_buffers_count(host_buffer);

	bool _is_resized = false;

	// here count is capacity of the device_buffer
	if(count > device_buffer->count)
	{
		if(instance_buffer->has_device_buffer)
		{
			vulkan_buffer_destroy(device_buffer);
			vulkan_buffer_release_resources(device_buffer);
		}
		vulkan_buffer_create_info_t create_info =
		{
			.stride = buf_get_element_size(&host_buffer->buffer),
			.count = count,
			.vo_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		VULKAN_OBJECT_INIT(device_buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_buffer_create_no_alloc(instance_buffer->renderer, &create_info, device_buffer);
		instance_buffer->has_device_buffer = true;
		_is_resized = true;
	}

	/* this case will be true when the host buffer doesn't contain any characters, and the device buffer hasn't ever created since the creation of this vulkan_instance_buffer_t object */
	if(!instance_buffer->has_device_buffer) return false;

	device_buffer->count = count;
	device_buffer->size = device_buffer->stride * count;
	if(device_buffer->count == 0) return false;

	// copy the elements from all the sub buffers to the device buffer contiguously (no gaps, as there could be due to capacities of the sub buffers)
	void* ptr = vulkan_buffer_map(device_buffer);
	multi_buffer_flatcopy_to(host_buffer, ptr);
	vulkan_buffer_unmap(device_buffer);
	if(is_resized != NULL)
		OUT is_resized = _is_resized;
	return true;
}
