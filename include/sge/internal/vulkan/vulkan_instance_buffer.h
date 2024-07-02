/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_instance_buffer.h is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_buffer.h>
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/multi_buffer.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;

typedef struct vulkan_instance_buffer_create_info_t
{
	u32 stride;			// element size
	u32 capacity;		// initial capacity, zero is also a valid capacity here
} vulkan_instance_buffer_create_info_t;

typedef struct vulkan_instance_buffer_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_buffer_t device_buffer;			// buffer in the device memory
	multi_buffer_t host_buffer;				// buffer in the host memory
	bool has_device_buffer; 				// initially only host-side buffer is created and used; so it would be false if *_commit never called
	bool is_dirty;
} vulkan_instance_buffer_t;

#define VULKAN_INSTANCE_BUFFER(ptr) VULKAN_OBJECT_UP_CAST(vulkan_instance_t*, VULKAN_OBJECT_TYPE_INSTANCE_BUFFER, ptr)
#define VULKAN_INSTANCE_BUFFER_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_instance_t*, VULKAN_OBJECT_TYPE_INSTANCE_BUFFER, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API void vulkan_instance_buffer_create(vulkan_renderer_t* renderer, vulkan_instance_buffer_create_info_t* create_info, vulkan_instance_buffer_t* out_instance_buffer);
SGE_API void vulkan_instance_buffer_destroy(vulkan_instance_buffer_t* instance_buffer);
SGE_API void vulkan_instance_buffer_release_resources(vulkan_instance_buffer_t* instance_buffer);

// getters
SGE_API multi_buffer_t* vulkan_instance_buffer_get_host_buffer(vulkan_instance_buffer_t* instance_buffer);
SGE_API vulkan_buffer_t* vulkan_instance_buffer_get_device_buffer(vulkan_instance_buffer_t* instance_buffer);

UNUSED_FUNCTION static bool vulkan_instance_buffer_has_device_buffer(vulkan_instance_buffer_t* instance_buffer)
{
	return vulkan_instance_buffer_get_device_buffer(instance_buffer) != NULL;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_instance_buffer_set_dirty(vulkan_instance_buffer_t* instance_buffer, bool is_dirty)
{
	instance_buffer->is_dirty = is_dirty;
}

// logic functions

// returns true when this buffer should be binded in the pipeline, otherwise false
SGE_API bool vulkan_instance_buffer_commit(vulkan_instance_buffer_t* instance_buffer, bool OUT is_resized);

END_CPP_COMPATIBLE
