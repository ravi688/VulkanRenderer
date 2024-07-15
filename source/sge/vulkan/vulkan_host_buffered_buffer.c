/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_host_buffered_buffer.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/memory_allocator.h>

SGE_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_new(memory_allocator_t* allocator)
{
	vulkan_host_buffered_buffer_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_HOST_BUFFERED_BUFFER, vulkan_host_buffered_buffer_t);
	memzero(buffer, vulkan_host_buffered_buffer_t);
	VULKAN_OBJECT_INIT(buffer, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	return buffer;
}

SGE_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_create(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info)
{
	vulkan_host_buffered_buffer_t* buffer = vulkan_host_buffered_buffer_new(renderer->allocator);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

SGE_API void vulkan_host_buffered_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info, vulkan_host_buffered_buffer_t OUT buffer)
{
	VULKAN_OBJECT_MEMZERO(buffer, vulkan_host_buffered_buffer_t);
	buffer->renderer = renderer;
	/* if capacity is non zero (> 0) then create a GPU side buffer */
	if(create_info->capacity > 0)
	{
		vulkan_buffer_create_info_t buffer_create_info = 
		{
			.count = create_info->capacity,
			.stride = create_info->stride,
			.vo_usage_flags = create_info->vo_usage_flags,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		VULKAN_OBJECT_INIT(&buffer->device_buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_buffer_create_no_alloc(renderer, &buffer_create_info, &buffer->device_buffer);
		buffer->has_device_buffer = true;
	}
	buffer->vo_usage_flags = create_info->vo_usage_flags;
	/* create a host side buffer */
	buffer->host_buffer = memory_allocator_buf_create(renderer->allocator, create_info->stride, create_info->capacity, 0);
}

SGE_API void vulkan_host_buffered_buffer_destroy(vulkan_host_buffered_buffer_t* buffer)
{
	if(buffer->has_device_buffer)
	{
		vulkan_buffer_destroy(&buffer->device_buffer);
		vulkan_buffer_release_resources(&buffer->device_buffer);
		buffer->has_device_buffer = false;
	}
	buf_clear(&buffer->host_buffer, NULL);
}

SGE_API void vulkan_host_buffered_buffer_release_resources(vulkan_host_buffered_buffer_t* buffer)
{
	buf_free(&buffer->host_buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(buffer))
		memory_allocator_dealloc(buffer->renderer->allocator, buffer);
}


SGE_API buffer_t* vulkan_host_buffered_buffer_get_host_buffer(vulkan_host_buffered_buffer_t* buffer)
{
	AUTO host_buffer = &buffer->host_buffer;
	buffer->is_dirty = true;
	return host_buffer;
}

SGE_API vulkan_buffer_t* vulkan_host_buffered_buffer_get_device_buffer(vulkan_host_buffered_buffer_t* buffer)
{
	return buffer->has_device_buffer ? &buffer->device_buffer : NULL;
}


SGE_API bool vulkan_host_buffered_buffer_commit(vulkan_host_buffered_buffer_t* buffer, bool OUT is_resized)
{
	if(!buffer->is_dirty)
		return false;

	buffer->is_dirty = false;

	buffer_t* host_buffer = vulkan_host_buffered_buffer_get_host_buffer_readonly(buffer);
	vulkan_buffer_t* device_buffer = vulkan_host_buffered_buffer_get_device_buffer(buffer);
	
	u32 size = buf_get_element_count(host_buffer) * buf_get_element_size(host_buffer);

	if(size == 0)
		/* no update on the gpu side buffer */
		return false;

	/* if there is no vulkan_buffer constructed or the size of the vulkan_buffer is less than that of the host buffer, then reconstruct it. */
	if((!buffer->has_device_buffer) || (buffer->has_device_buffer && (size > vulkan_buffer_get_size(device_buffer))))
	{
		/* destory the previously constructed vulkan buffer object */
		if(buffer->has_device_buffer)
		{
			_debug_assert__(vulkan_buffer_get_size(device_buffer) != 0);
			vulkan_buffer_destroy(device_buffer);
			vulkan_buffer_release_resources(device_buffer);
		}

		/* create a new vulkan buffer object */
		vulkan_buffer_create_info_t create_info = 
		{
			.size = size,
			.vo_usage_flags = buffer->vo_usage_flags,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	
		};
		VULKAN_OBJECT_INIT(device_buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		vulkan_buffer_create_no_alloc(buffer->renderer, &create_info, device_buffer);
		buffer->has_device_buffer = true;
		if(is_resized != NULL)
			OUT is_resized = true;
	}

	_debug_assert__(buffer->has_device_buffer == true);
	
	/* copy the host side buffer to the GPU side buffer */
	vulkan_buffer_copy_data(device_buffer, 0, buf_get_ptr(host_buffer), size);

	/* updated the gpu side buffer */
	return true;
}

