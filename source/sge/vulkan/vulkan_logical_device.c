/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_logical_device.c is a part of VulkanRenderer

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
#include <sge/internal/vulkan/vulkan_logical_device.h>
#include <sge/internal/vulkan/vulkan_physical_device.h>
#include <sge/internal/vulkan/vulkan_result.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/assert.h>
#include <sge/debug.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h>

SGE_API vulkan_logical_device_t* vulkan_logical_device_new(memory_allocator_t* allocator)
{
	vulkan_logical_device_t* device = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_LOGICAL_DEVICE, vulkan_logical_device_t);
	memzero(device, vulkan_logical_device_t);
	VULKAN_OBJECT_INIT(device, VULKAN_OBJECT_TYPE_LOGICAL_DEVICE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return device;	
}

SGE_API vulkan_logical_device_t* vulkan_logical_device_create(vulkan_physical_device_t* physical_device, vulkan_logical_device_create_info_t* device_create_info)
{
	vulkan_logical_device_t* device = vulkan_logical_device_new(physical_device->renderer->allocator);
	device->renderer = physical_device->renderer;
	u32* family_indices = device_create_info->queue_family_indices;
	u32 family_index_count = device_create_info->queue_family_index_count;
	
	// union operation, complexity O(n) == linear

	u32 max_index = (family_indices == NULL) ? 0 : family_indices[0];
	for(u32 i = 1; i < family_index_count; i++)		// find the max index
		if(family_indices[i] > max_index) max_index = family_indices[i];

	u32 look_up[max_index + 1];
	memset(look_up, 0, sizeof(u32) * (max_index + 1));	// intiailize the look up table with zeros
	u32 union_result[family_index_count]; 				// allocate for maximum possible elements in the final result
	u32 queue_family_count = 0;
	for(u32 i = 0; i < family_index_count; i++) 		// union operation
	{
		if(look_up[family_indices[i]] == 1)
			continue;
		look_up[family_indices[i]] = 1;
		union_result[queue_family_count] = family_indices[i];
		queue_family_count++;
	}

	// TODO: Make priorities configurable
	float priority = 1;

	VkDeviceQueueCreateInfo* queue_create_infos = memory_allocator_alloc_obj_array(physical_device->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DEVICE_QUEUE_CREATE_INFO_ARRAY, VkDeviceQueueCreateInfo, queue_family_count);
	memset(queue_create_infos, 0, sizeof(VkDeviceQueueCreateInfo) * queue_family_count);
	for(u32 i = 0; i < queue_family_count; i++)
	{
		queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[i].queueFamilyIndex = union_result[i];
		// TODO: Make queue count configurable
		queue_create_infos[i].queueCount = 1;
		queue_create_infos[i].pQueuePriorities = &priority;
	}
	log_msg("Queue count: %u\n", queue_family_count);
	
	const char* extensions[device_create_info->extension_count];
	u32 extension_count = 0;
	for(u32 i = 0; i < device_create_info->extension_count; i++)
	{
		if(!vulkan_physical_device_is_extension_supported(physical_device, device_create_info->extensions[i]))
		{
			LOG_WRN("Device extension \"%s\" isn't supported, ignored\n", device_create_info->extensions[i]);
			continue;
		}
		extensions[extension_count] = device_create_info->extensions[extension_count];
		extension_count++;
	}

	VkDeviceCreateInfo create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queue_family_count,
		.pQueueCreateInfos = queue_create_infos,
		.enabledExtensionCount = extension_count,
		.ppEnabledExtensionNames = (extension_count == 0) ? NULL : extensions,
		.pEnabledFeatures = device_create_info->features
	};

	vkCall(vkCreateDevice(physical_device->vo_handle, &create_info, VULKAN_ALLOCATION_CALLBACKS(physical_device->renderer), &device->vo_handle));

	memory_allocator_dealloc(physical_device->renderer->allocator, queue_create_infos);

	log_msg("Logical device created successfully\n");
	return device;	
}

SGE_API void vulkan_logical_device_destroy(vulkan_logical_device_t* device)
{
	vkDestroyDevice(device->vo_handle, VULKAN_ALLOCATION_CALLBACKS(device->renderer));
	log_msg("Logical device destroyed successfully\n");
}

SGE_API void vulkan_logical_device_release_resources(vulkan_logical_device_t* device)
{
	if(VULKAN_OBJECT_IS_INTERNAL(device))
		memory_allocator_dealloc(device->renderer->allocator, device);
}

SGE_API VkQueue vulkan_logical_device_get_queue(vulkan_logical_device_t* device, u32 family_index, u32 queue_index)
{
	VkQueue vo_queue;
	vkGetDeviceQueue(device->vo_handle, family_index, queue_index, &vo_queue);
	return vo_queue;
}
