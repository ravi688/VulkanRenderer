/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_swapchain.h is a part of VulkanRenderer

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

typedef struct vulkan_swapchain_create_info_t
{
	VkFormat vo_image_format;
	VkColorSpaceKHR vo_image_color_space;
	u32 image_count;
	VkExtent2D vo_image_extent;
	VkPresentModeKHR vo_present_mode;
	u32 queue_family_index_count;
	u32* queue_family_indices;
} vulkan_swapchain_create_info_t;

typedef struct vulkan_swapchain_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkSwapchainKHR vo_handle;
	VkImage* vo_images;
	VkImageView* vo_image_views;
	VkExtent2D vo_image_extent;
	VkFormat vo_image_format;
	u32 image_count;
	u32 current_image_index;
} vulkan_swapchain_t;

#define VULKAN_SWAPCHAIN(ptr) VULKAN_OBJECT_UP_CAST(vulkan_swapchain_t*, VULKAN_OBJECT_TYPE_SWAPCHAIN, ptr)
#define VULKAN_SWAPCHAIN_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_swapchain_t*, VULKAN_OBJECT_TYPE_SWAPCHAIN, ptr)

BEGIN_CPP_COMPATIBLE

SGE_API vulkan_swapchain_t* vulkan_swapchain_new(memory_allocator_t* allocator);
SGE_API vulkan_swapchain_t* vulkan_swapchain_create(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info);
SGE_API void vulkan_swapchain_create_no_alloc(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info, vulkan_swapchain_t OUT swapchain);
SGE_API void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain);
SGE_API void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain);
SGE_API void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info);
SGE_API u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, VkSemaphore signal_semaphore);

END_CPP_COMPATIBLE
