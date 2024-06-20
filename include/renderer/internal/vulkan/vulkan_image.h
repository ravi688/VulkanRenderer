/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_image.h is a part of VulkanRenderer

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

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef struct vulkan_image_create_info_t
{
	VkImageCreateFlags vo_flags;						// VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT for cube map images
	VkImageType vo_type;								// VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D or VK_IMAGE_TYPE_3D
	VkFormat vo_format;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
	VkImageTiling vo_tiling; 							// VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL
	VkImageLayout vo_layout;	 						// VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED
	VkImageUsageFlags vo_usage_mask; 					// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	VkMemoryPropertyFlags vo_memory_properties_mask;  	// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPRETY_HOST_COHERENT_BIT 
	VkImageAspectFlags vo_aspect_mask; 				// VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT
} vulkan_image_create_info_t;

typedef struct vulkan_image_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkImage vo_handle;
	VkDeviceMemory vo_memory;
	VkImageType vo_type;
	VkFormat vo_format;
	VkImageAspectFlags vo_aspect_mask;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
} vulkan_image_t;

#define VULKAN_IMAGE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_image_t*, VULKAN_OBJECT_TYPE_IMAGE, ptr)
#define VULKAN_IMAGE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_image_t*, VULKAN_OBJECT_TYPE_IMAGE, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_image_t* vulkan_image_new(memory_allocator_t* allocator);
RENDERER_API vulkan_image_t* vulkan_image_create(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info);
RENDERER_API void vulkan_image_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* out_image);
RENDERER_API void vulkan_image_destroy(vulkan_image_t* image);
RENDERER_API void vulkan_image_release_resources(vulkan_image_t* image);

END_CPP_COMPATIBLE
