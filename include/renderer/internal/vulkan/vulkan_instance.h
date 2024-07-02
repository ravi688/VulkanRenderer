/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_instance.h is a part of VulkanRenderer

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

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_physical_device_t vulkan_physical_device_t;

typedef struct vulkan_instance_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkInstance handle;

	// physical devices
	vulkan_physical_device_t* physical_devices;
	u32 physical_device_count;

	// extensions
	VkExtensionProperties* extension_properties;
	u32 extension_count;
} vulkan_instance_t;

#define VULKAN_INSTANCE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_instance_t*, VULKAN_OBJECT_TYPE_INSTANCE, ptr)
#define VULKAN_INSTANCE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_instance_t*, VULKAN_OBJECT_TYPE_INSTANCE, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API vulkan_instance_t* vulkan_instance_new(memory_allocator_t* allocator);
SGE_API vulkan_instance_t* vulkan_instance_create(vulkan_renderer_t* renderer, const char* const* extensions, u32 extension_count, const char* const* layers, u32 layer_count);
SGE_API void vulkan_instance_destroy(vulkan_instance_t* instance);
SGE_API void vulkan_instance_release_resources(vulkan_instance_t* instance);

// getters
SGE_API u32 vulkan_instance_get_physical_device_count(vulkan_instance_t* instance);
SGE_API u32 vulkan_instance_get_extension_count(vulkan_instance_t* instance, const char* layer_name);
SGE_API vulkan_physical_device_t* vulkan_instance_get_physical_devices(vulkan_instance_t* instance);
SGE_API VkExtensionProperties* vulkan_instance_get_extension_properties(vulkan_instance_t* instance, const char* layer_name);

// returning bools
SGE_API bool vulkan_instance_is_extension_supported(vulkan_instance_t* instance, const char* extension, const char* layer_name);

/* 	vulkan_instance_get_filter_for_supported_layers: returns boolean filter array designating which layers are supported and which are not
 *	layer_count: number of layers (const char*)
 * 	layers: list of required layer names (const char* const*)
 */
SGE_API bool* vulkan_instance_get_filter_for_supported_layers(memory_allocator_t* allocator, u32 layer_count, const char* const* layers);

// to string (s)
SGE_API void vulkan_instance_to_string(vulkan_instance_t* instance, BUFFER* string_buffer);

END_CPP_COMPATIBLE
