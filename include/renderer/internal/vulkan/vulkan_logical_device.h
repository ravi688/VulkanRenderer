/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_logical_device.h is a part of VulkanRenderer

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

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_physical_device_t vulkan_physical_device_t;


typedef struct vulkan_logical_device_create_info_t
{
	// queues to create
	u32* queue_family_indices;
	u32 queue_family_index_count;

	// extensions to enable
	const char* const* extensions;
	u32 extension_count;

	// features to enable
	VkPhysicalDeviceFeatures* features;

} vulkan_logical_device_create_info_t;
 
typedef struct vulkan_logical_device_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkDevice vo_handle;
} vulkan_logical_device_t;

#define VULKAN_LOGICAL_DEVICE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_logical_device_t*, VULKAN_OBJECT_TYPE_LOGICAL_DEVICE, ptr)
#define VULKAN_LOGICAL_DEVICE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_logical_device_t*, VULKAN_OBJECT_TYPE_LOGICAL_DEVICE, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_logical_device_t* vulkan_logical_device_new(memory_allocator_t* allocator);
RENDERER_API vulkan_logical_device_t* vulkan_logical_device_create(vulkan_physical_device_t* physical_device, vulkan_logical_device_create_info_t* device_create_info);
RENDERER_API void vulkan_logical_device_destroy(vulkan_logical_device_t* device);
RENDERER_API void vulkan_logical_device_release_resources(vulkan_logical_device_t* device);


// getters
RENDERER_API VkQueue vulkan_logical_device_get_queue(vulkan_logical_device_t* device, u32 family_index, u32 queue_index);

END_CPP_COMPATIBLE
