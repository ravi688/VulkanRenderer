/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_physical_device.h is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_physical_device_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkPhysicalDevice vo_handle;
	VkPhysicalDeviceProperties vo_properties;			// cached properties
	VkPhysicalDeviceFeatures vo_features;				// cached features
	VkQueueFamilyProperties* vo_queue_family_properties;
	u32 queue_family_count;
	VkExtensionProperties* vo_extension_properties;
	u32 extension_count;
	//BUFFER /* vulkan_logical_device_t* */ logical_devices; 			// all the logical devices created with this physical device
} vulkan_physical_device_t;

#define VULKAN_PHYSICAL_DEVICE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_physical_device_t*, VUKLAN_OBJECT_TYPE_PHYSICAL_DEVICE, ptr)
#define VULKAN_PHYSICAL_DEVICE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_physical_device_t*, VUKLAN_OBJECT_TYPE_PHYSICAL_DEVICE, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_physical_device_t* vulkan_physical_device_new(memory_allocator_t* allocator);
RENDERER_API vulkan_physical_device_t* vulkan_physical_device_create(vulkan_renderer_t* renderer, VkPhysicalDevice vk_device);
RENDERER_API void vulkan_physical_device_create_no_alloc(vulkan_renderer_t* renderer, VkPhysicalDevice vk_device, vulkan_physical_device_t OUT device);
RENDERER_API void vulkan_physical_device_destroy(vulkan_physical_device_t* device);
RENDERER_API void vulkan_physical_device_release_resources(vulkan_physical_device_t* device);

// getters
RENDERER_API VkPresentModeKHR* vulkan_physical_device_get_present_modes(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count);
RENDERER_API VkSurfaceFormatKHR* vulkan_physical_device_get_surface_formats(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count);
RENDERER_API VkSurfaceCapabilitiesKHR vulkan_physical_device_get_surface_capabilities(vulkan_physical_device_t* device, VkSurfaceKHR surface);
RENDERER_API VkPhysicalDeviceLimits* vulkan_physical_device_get_limits(vulkan_physical_device_t* device);
RENDERER_API VkPhysicalDeviceProperties* vulkan_physical_device_get_properties(vulkan_physical_device_t* device);
RENDERER_API VkPhysicalDeviceFeatures* vulkan_physical_device_get_features(vulkan_physical_device_t* device);
RENDERER_API VkQueueFamilyProperties* vulkan_physical_device_get_queue_family_properties(vulkan_physical_device_t* device);
RENDERER_API u32 vulkan_physical_device_get_queue_family_count(vulkan_physical_device_t* device);
// returns U32_MAX if not found
RENDERER_API u32 vulkan_physical_device_find_queue_family_index(vulkan_physical_device_t* device, VkQueueFlags queue_flags);
RENDERER_API u32 vulkan_physical_device_find_queue_family_index_for_surface(vulkan_physical_device_t* device, VkSurfaceKHR surface);
RENDERER_API u32 vulkan_physical_device_find_memory_type(vulkan_physical_device_t* device, u32 required_memory_type_bits, VkMemoryPropertyFlags required_memory_properties);
RENDERER_API VkFormat vulkan_physical_device_find_supported_format(vulkan_physical_device_t* device, const VkFormat* const formats, u32 format_count, VkImageTiling tiling, VkFormatFeatureFlags format_features);

// returning bools
RENDERER_API bool vulkan_physical_device_meets_feature_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceFeatures* required_features);
RENDERER_API bool vulkan_physical_device_meets_property_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceProperties* required_properties);
RENDERER_API bool vulkan_physical_device_is_extension_supported(vulkan_physical_device_t* device, const char* extension);

// to string (s)
RENDERER_API void vulkan_physical_device_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_limits_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_properties_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_extensions_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);

END_CPP_COMPATIBLE
