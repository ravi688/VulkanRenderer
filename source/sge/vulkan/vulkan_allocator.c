/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_allocator.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h>

static void* VKAPI_CALL vulkan_allocation(void* user_data, size_t size, size_t align, VkSystemAllocationScope allocation_scope)
{
	CAN_BE_UNUSED_VARIABLE vulkan_allocator_t* vk_allocator = VULKAN_ALLOCATOR(user_data);
	size = size + (align - (size % align)) % align;
	void* address = memory_allocator_aligned_alloc(vk_allocator->allocator, MEMORY_ALLOCATION_TYPE_VKAPI, size, align);
	_debug_assert__((CAST_TO(u64, address) % align) == 0);
	return address;
}

static void* VKAPI_CALL vulkan_reallocation(void* user_data, void* original, size_t size, size_t align, VkSystemAllocationScope allocation_scope)
{
	CAN_BE_UNUSED_VARIABLE vulkan_allocator_t* vk_allocator = VULKAN_ALLOCATOR(user_data);
	if((size == 0) && (original != NULL))
	{
		memory_allocator_dealloc(vk_allocator->allocator, original);
		return NULL;
	}
	void* address = memory_allocator_aligned_realloc(vk_allocator->allocator, original, MEMORY_ALLOCATION_TYPE_VKAPI, size, align);
	_debug_assert__((CAST_TO(u64, address) % align) == 0);
	return address;
}

static void VKAPI_CALL vulkan_free(void* user_data, void* memory)
{
	if(memory == NULL)
		return;
	CAN_BE_UNUSED_VARIABLE AUTO allocator = VULKAN_ALLOCATOR(user_data)->allocator;
#ifdef MEMORY_METRICS
	if(!memory_allocator_is_allocation_exists(allocator, memory))
	{
		debug_log_warning("Vulkan Allocator: Unable to free memory block at %p, usable size: %u bytes, no such allocation was made by vulkan_allocator_t", memory, heap_usable_size(memory));
		return;
	}
#endif
	memory_allocator_dealloc(allocator, memory);
}

static void VKAPI_CALL vulkan_internal_allocation_notification(void* user_data, size_t size, VkInternalAllocationType allocation_type, VkSystemAllocationScope allocation_scope)
{

}

static void VKAPI_CALL vulkan_internal_allocation_free_notification(void* user_data, size_t size, VkInternalAllocationType allocation_type, VkSystemAllocationScope allocation_scope)
{

}


static INLINE VkAllocationCallbacks get_allocation_callbacks(vulkan_allocator_t* allocator)
{
	return (VkAllocationCallbacks)
	{
		.pUserData = allocator,
		.pfnAllocation = vulkan_allocation,
		.pfnReallocation = vulkan_reallocation,
		.pfnFree = vulkan_free,
		.pfnInternalAllocation = vulkan_internal_allocation_notification,
		.pfnInternalFree = vulkan_internal_allocation_free_notification
	};
}

SGE_API vulkan_allocator_t* vulkan_allocator_create(memory_allocator_t* allocator)
{
	vulkan_allocator_t* vk_allocator = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ALLOCATOR, vulkan_allocator_t);
	memzero(vk_allocator, vulkan_allocator_t);
	VULKAN_OBJECT_INIT(vk_allocator, VULKAN_OBJECT_TYPE_ALLOCATOR, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	vk_allocator->allocator = allocator;
	vk_allocator->vo_callbacks = get_allocation_callbacks(vk_allocator);
	vk_allocator->current_allocation_type = MEMORY_ALLOCATION_TYPE_VKAPI;
	return vk_allocator;
}

SGE_API void vulkan_allocator_destroy(vulkan_allocator_t* allocator)
{
	if(VULKAN_OBJECT_IS_INTERNAL(allocator))
		memory_allocator_dealloc(allocator->allocator, allocator);
}

SGE_API VkAllocationCallbacks* vulkan_allocator_get_native_callbacks(vulkan_allocator_t* allocator, memory_allocation_type_t allocation_type)
{
	allocator->current_allocation_type = allocation_type;
	return &allocator->vo_callbacks;
}

