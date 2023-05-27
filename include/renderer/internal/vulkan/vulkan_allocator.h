/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_allocator.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
#include <vulkan/vulkan.h>
#include <renderer/memory_allocation_type.h>

typedef struct vulkan_allocator_t
{
	memory_allocator_t* allocator;
	VkAllocationCallbacks vo_callbacks;
	memory_allocation_type_t current_allocation_type;
} vulkan_allocator_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API vulkan_allocator_t* vulkan_allocator_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_allocator_destroy(vulkan_allocator_t* allocator);

#ifdef USE_VULKAN_ALLOCATOR
# 	define VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, allocation_type) vulkan_allocator_get_native_callbacks((driver)->vk_allocator, allocation_type)
#else
#	define VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, allocation_type) NULL
#endif /* VULKAN_ALLOCATION_CALLBACKS */
#define VULKAN_ALLOCATION_CALLBACKS(driver) VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, MEMORY_ALLOCATION_TYPE_VKAPI)
/* returns the internal allocation callbacks for vulkan API */
RENDERER_API VkAllocationCallbacks* vulkan_allocator_get_native_callbacks(vulkan_allocator_t* allocator, memory_allocation_type_t allocation_type);

END_CPP_COMPATIBLE
