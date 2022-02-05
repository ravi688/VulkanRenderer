
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_physical_device_t vulkan_physical_device_t;


typedef struct vulkan_logical_device_create_info_t
{
	// queues to create
	u32* queue_family_indices;
	u32 queue_family_index_count;

	// extensions to enable
	const char* const* extensions;
	u32 extension_count;

	// features to enable
	VkPhysicalDeviceFeatures* features;

} vulkan_logical_device_create_info_t;
 
typedef struct vulkan_logical_device_t
{
	VkDevice handle;
} vulkan_logical_device_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_logical_device_t* vulkan_logical_device_new();
RENDERER_API vulkan_logical_device_t* vulkan_logical_device_create(vulkan_physical_device_t* physical_device, vulkan_logical_device_create_info_t* device_create_info);
RENDERER_API void vulkan_logical_device_destroy(vulkan_logical_device_t* device);
RENDERER_API void vulkan_logical_device_release_resources(vulkan_logical_device_t* device);


// getters
RENDERER_API VkQueue vulkan_logical_device_get_queue(vulkan_logical_device_t* device, u32 family_index, u32 queue_index);

END_CPP_COMPATIBLE
