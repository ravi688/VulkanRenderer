/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_allocator.h is a part of VulkanRenderer

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
#include <vulkan/vulkan.h>
#include <sge/memory_allocation_type.h>
#include <sge/internal/vulkan/vulkan_object.h>

typedef struct vulkan_allocator_t
{
	__VULKAN_OBJECT__;
	memory_allocator_t* allocator;
	VkAllocationCallbacks vo_callbacks;
	memory_allocation_type_t current_allocation_type;
} vulkan_allocator_t;

#define VULKAN_ALLOCATOR(ptr) VULKAN_OBJECT_UP_CAST(vulkan_allocator_t*, VULKAN_OBJECT_TYPE_ALLOCATOR, ptr)
#define VULKAN_ALLOCATOR_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_allocator_t*, VULKAN_OBJECT_TYPE_ALLOCATOR, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API vulkan_allocator_t* vulkan_allocator_create(memory_allocator_t* allocator);
SGE_API void vulkan_allocator_destroy(vulkan_allocator_t* allocator);

#ifdef USE_VULKAN_ALLOCATOR
# 	define VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, allocation_type) vulkan_allocator_get_native_callbacks((driver)->vk_allocator, allocation_type)
#else
#	define VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, allocation_type) NULL
#endif /* VULKAN_ALLOCATION_CALLBACKS */
#define VULKAN_ALLOCATION_CALLBACKS(driver) VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, MEMORY_ALLOCATION_TYPE_VKAPI)
/* returns the internal allocation callbacks for vulkan API */
SGE_API VkAllocationCallbacks* vulkan_allocator_get_native_callbacks(vulkan_allocator_t* allocator, memory_allocation_type_t allocation_type);

END_CPP_COMPATIBLE
