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

#define VULKAN_ALLOCATION_CALLBACKS_FOR_TYPE(driver, allocation_type) vulkan_allocator_get_native_callbacks((driver)->vk_allocator, allocation_type)
#define VULKAN_ALLOCATION_CALLBACKS(driver) NULL
/* returns the internal allocation callbacks for vulkan API */
RENDERER_API VkAllocationCallbacks* vulkan_allocator_get_native_callbacks(vulkan_allocator_t* allocator, memory_allocation_type_t allocation_type);

END_CPP_COMPATIBLE
