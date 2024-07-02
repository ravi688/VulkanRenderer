/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_host_buffered_buffer.h is a part of VulkanRenderer

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

#pragma once

#include <sge/defines.h>
#include <bufferlib/buffer.h>
#include <sge/internal/vulkan/vulkan_buffer.h>
#include <sge/internal/vulkan/vulkan_object.h>

typedef struct vulkan_host_buffered_buffer_create_info_t
{
	/* element size,
	 * for-example: if we have an element of type vec3 and it has alignment of 16,
	 * then array of such an elements will have stride = sizeof(vec3) + (alignof(vec3) - sizeof(vec3) % alignof(vec3)) % (alignof(vec3)) */
	u32 stride;
	/* number of elements that can be stored initially */
	u32 capacity;
	/* usage flags for the device buffer */
	VkBufferUsageFlags vo_usage_flags;
} vulkan_host_buffered_buffer_create_info_t;

typedef BUFFER buffer_t;

/* a GPU buffer having a coherent copy on the host side */
typedef struct vulkan_host_buffered_buffer_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_buffer_t device_buffer;
	buffer_t host_buffer;
	VkBufferUsageFlags vo_usage_flags;
	bool has_device_buffer;
	bool is_dirty;
} vulkan_host_buffered_buffer_t;

#define VULKAN_HOST_BUFFERED_BUFFER(typed_ptr) VULKAN_OBJECT_UP_CAST(vulkan_host_buffered_buffer_t*, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, typed_ptr)
#define VULKAN_HOST_BUFFERED_BUFFER_CONST(typed_ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_host_buffered_buffer_t*, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, typed_ptr)


BEGIN_CPP_COMPATIBLE

SGE_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_new(memory_allocator_t* allocator);
SGE_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_create(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info);
SGE_API void vulkan_host_buffered_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info, vulkan_host_buffered_buffer_t OUT buffer);
SGE_API void vulkan_host_buffered_buffer_destroy(vulkan_host_buffered_buffer_t* buffer);
SGE_API void vulkan_host_buffered_buffer_release_resources(vulkan_host_buffered_buffer_t* buffer);

SGE_API buffer_t* vulkan_host_buffered_buffer_get_host_buffer(vulkan_host_buffered_buffer_t* buffer);
SGE_API vulkan_buffer_t* vulkan_host_buffered_buffer_get_device_buffer(vulkan_host_buffered_buffer_t* buffer);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_host_buffered_buffer_set_dirty(vulkan_host_buffered_buffer_t* buffer, bool is_dirty)
{
	buffer->is_dirty = is_dirty;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_host_buffered_buffer_is_dirty(vulkan_host_buffered_buffer_t* buffer)
{
	return buffer->is_dirty;
}

/* returns true if either the exisiting VkBuffer has been updated with new data or VkBuffer is recreated upon resize */
SGE_API bool vulkan_host_buffered_buffer_commit(vulkan_host_buffered_buffer_t* buffer, bool OUT is_resized);

END_CPP_COMPATIBLE
