/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_to_string.h is a part of VulkanRenderer

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
#include <bufferlib/buffer.h>
#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

// VkFormat
RENDERER_API void vk_format_to_string(const char* description, VkFormat format, BUFFER* string_buffer);

// VkSurfaceFormatKHR
RENDERER_API void vk_present_mode_to_string(const char* description, VkPresentModeKHR present_mode, BUFFER* string_buffer);

// VkPresentModeKHR
RENDERER_API void vk_surface_format_to_string(const char* description, VkSurfaceFormatKHR surface_format, BUFFER* string_buffer);

// VkSurfaceTransformFlagBits
RENDERER_API void vk_surface_transform_flag_bits_to_string(const char* description, VkSurfaceTransformFlagBitsKHR transform, BUFFER* string_buffer);

// VkSurfaceTransformFlags
RENDERER_API void vk_surface_transform_flags_to_string(const char* description, VkSurfaceTransformFlagsKHR transform_flags, BUFFER* string_buffer);

// VkSurfaceCapabilitiesKHR
RENDERER_API void vk_surface_capabilities_to_string(const char* description, VkSurfaceCapabilitiesKHR* capabilities, BUFFER* string_buffer);

// VkBool32
RENDERER_API void vk_bool32_to_string(const char* description, VkBool32 value, BUFFER* string_buffer);

// VkSampleCountFlags
RENDERER_API void vk_sample_count_flags_to_string(const char* description, VkSampleCountFlags flags, BUFFER* string_buffer);

// VkPhysicalDeviceType
RENDERER_API void vk_physical_device_type_to_string(const char* description, VkPhysicalDeviceType type, BUFFER* string_buffer);

// VkPhysicalDeviceProperties
RENDERER_API void vk_physical_device_properties_to_string(const char* description, VkPhysicalDeviceProperties* properties, BUFFER* string_buffer);

// VkPhysicalDeviceLimits
RENDERER_API void vk_physical_device_limits_to_string(const char* description, VkPhysicalDeviceLimits* limits, BUFFER* string_buffer);

END_CPP_COMPATIBLE
