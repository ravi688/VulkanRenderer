#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_logical_device.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_logical_device_t* device);
#else
#	define check_pre_condition(device)
#endif /* GLOBAL_DEBUG */

RENDERER_API vulkan_logical_device_t* vulkan_logical_device_new()
{
	vulkan_logical_device_t* device = heap_new(vulkan_logical_device_t);
	memset(device, 0, sizeof(vulkan_logical_device_t));
	return device;	
}

RENDERER_API vulkan_logical_device_t* vulkan_logical_device_create(vulkan_physical_device_t* physical_device, vulkan_logical_device_create_info_t* device_create_info)
{
	assert(device_create_info != NULL);
	vulkan_logical_device_t* device = vulkan_logical_device_new();
	u32* family_indices = device_create_info->queue_family_indices;
	u32 family_index_count = device_create_info->queue_family_index_count;
	
	// union operation, complexity O(n) == linear

	u32 max_index = (family_indices == NULL) ? 0 : family_indices[0];
	for(u32 i = 1; i < family_index_count; i++)		// find the max index
		if(family_indices[i] > max_index) max_index = family_indices[i];

	u32 look_up[max_index + 1];
	memset(look_up, 0, sizeof(u32) * (max_index + 1));	// intiailize the look up table with zeros
	u32 union_result[family_index_count]; 				// allocate for maximum possible elements in the final result
	u32 queue_family_count = 0;
	for(u32 i = 0; i < family_index_count; i++) 		// union operation
	{
		if(look_up[family_indices[i]] == 1)
			continue;
		look_up[family_indices[i]] = 1;
		union_result[queue_family_count] = family_indices[i];
		queue_family_count++;
	}

	// TODO: Make priorities configurable
	float priority = 1;

	VkDeviceQueueCreateInfo* queue_create_infos = heap_newv(VkDeviceQueueCreateInfo, queue_family_count);
	memset(queue_create_infos, 0, sizeof(VkDeviceQueueCreateInfo) * queue_family_count);
	for(u32 i = 0; i < queue_family_count; i++)
	{
		queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[i].queueFamilyIndex = family_indices[i];
		// TODO: Make queue count configurable
		queue_create_infos[i].queueCount = 1;
		queue_create_infos[i].pQueuePriorities = &priority;
	}
	log_msg("Queue count: %u\n", queue_family_count);
	
	const char* extensions[device_create_info->extension_count];
	u32 extension_count = 0;
	for(u32 i = 0; i < device_create_info->extension_count; i++)
	{
		if(!vulkan_physical_device_is_extension_supported(physical_device, device_create_info->extensions[i]))
		{
			LOG_WRN("Device extension \"%s\" isn't supported, ignored\n", device_create_info->extensions[i]);
			continue;
		}
		extensions[extension_count] = device_create_info->extensions[extension_count];
		extension_count++;
	}

	VkDeviceCreateInfo create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queue_family_count,
		.pQueueCreateInfos = queue_create_infos,
		.enabledExtensionCount = extension_count,
		.ppEnabledExtensionNames = (extension_count == 0) ? NULL : extensions,
		.pEnabledFeatures = device_create_info->features
	};

	VkResult result = vkCreateDevice(physical_device->vo_handle, &create_info, NULL, &device->vo_handle);
	vulkan_result_assert_success(result);

	heap_free(queue_create_infos);

	log_msg("Logical device created successfully\n");
	return device;	
}

RENDERER_API void vulkan_logical_device_destroy(vulkan_logical_device_t* device)
{
	vkDestroyDevice(device->vo_handle, NULL);
	log_msg("Logical device destroyed successfully\n");
}

RENDERER_API void vulkan_logical_device_release_resources(vulkan_logical_device_t* device)
{
	heap_free(device);
}

RENDERER_API VkQueue vulkan_logical_device_get_queue(vulkan_logical_device_t* device, u32 family_index, u32 queue_index)
{
	check_pre_condition(device);
	VkQueue vo_queue;
	vkGetDeviceQueue(device->vo_handle, family_index, queue_index, &vo_queue);
	return vo_queue;
}


#ifdef GLOBAL_DEBUG
static void check_pre_condition(vulkan_logical_device_t* device)
{
	assert(device != NULL);
	assert(device->vo_handle != VK_NULL_HANDLE);
}
#endif /* GLOBAL_DEBUG */

