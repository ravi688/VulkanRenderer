#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_to_string.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <string.h> 		// strcmp

// constructors and destructors
RENDERER_API vulkan_physical_device_t* vulkan_physical_device_new(memory_allocator_t* allocator)
{
	vulkan_physical_device_t* device = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_PHYSICAL_DEVICE, vulkan_physical_device_t);
	memzero(device, vulkan_physical_device_t);
	return device;
}

RENDERER_API void vulkan_physical_device_create_no_alloc(vulkan_renderer_t* renderer, VkPhysicalDevice vk_device, vulkan_physical_device_t OUT device)
{
	memzero(device, vulkan_physical_device_t);
	device->renderer = renderer;

	device->vo_handle = vk_device;

	// create list of logical devices to store all the logical devices created with this physical device
	// device->logical_devices = buf_create(sizeof(vulkan_logical_device_t*), 0, 0);

	// cache the physical device properties
	vkGetPhysicalDeviceProperties(device->vo_handle, &device->vo_properties);

	// cache the physical device features
	vkGetPhysicalDeviceFeatures(device->vo_handle, &device->vo_features);

	// cache the queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(device->vo_handle, &device->queue_family_count, NULL);
	_debug_assert__(device->queue_family_count != 0);
	device->vo_queue_family_properties = memory_allocator_alloc_obj_array(device->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_QUEUE_FAMILY_PROPERTIES_ARRAY, VkQueueFamilyProperties, device->queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device->vo_handle, &device->queue_family_count, device->vo_queue_family_properties);

	// cache physical device extensions
	VkResult vo_result = vkEnumerateDeviceExtensionProperties(device->vo_handle, NULL, &device->extension_count, NULL);
	vulkan_result_assert_success(vo_result);
	device->vo_extension_properties = heap_newv(VkExtensionProperties, device->extension_count);
	vo_result = vkEnumerateDeviceExtensionProperties(device->vo_handle, NULL, &device->extension_count, device->vo_extension_properties);
	vulkan_result_assert_success(vo_result);
	vulkan_result_assert_complete(vo_result);
}

RENDERER_API vulkan_physical_device_t* vulkan_physical_device_create(vulkan_renderer_t* renderer, VkPhysicalDevice vk_device)
{
	vulkan_physical_device_t* device = vulkan_physical_device_new(renderer->allocator);
	vulkan_physical_device_create_no_alloc(renderer, vk_device, device);
	return device;
}

RENDERER_API void vulkan_physical_device_destroy(vulkan_physical_device_t* device)
{
	// for(buf_ucount_t i = 0; i < buf_get_element_count(&device->logical_devices); i++)
	// 	vulkan_logical_device_destroy(*buf_get_ptr_at_typeof(&device->logical_devices, vulkan_logical_device_t*, i));
}

RENDERER_API void vulkan_physical_device_release_resources(vulkan_physical_device_t* device)
{
	// for(buf_ucount_t i = 0; i < buf_get_element_count(&device->logical_devices); i++)
	// 	vulkan_logical_device_release_resources(*buf_get_ptr_at_typeof(&device->logical_devices, vulkan_logical_device_t*, i));
	// buf_free(&device->logical_devices);
	memory_allocator_dealloc(device->renderer->allocator, device->vo_queue_family_properties);
	memory_allocator_dealloc(device->renderer->allocator, device->vo_extension_properties);

	// TODO:
	// heap_free(device);
}


// getters
RENDERER_API VkPresentModeKHR* vulkan_physical_device_get_present_modes(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count)
{
	u32 count;
	VkResult vo_result = vkGetPhysicalDeviceSurfacePresentModesKHR(device->vo_handle, surface, &count, NULL);
	vulkan_result_assert_success(vo_result);
	*out_count = count;
	if(count == 0)
	{
		LOG_WRN("Present mode count is zero\n");
		return NULL;
	}
	VkPresentModeKHR* vo_modes = memory_allocator_alloc_obj_array(device->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PRESENT_MODE_KHR_ARRAY, VkPresentModeKHR, count);
	vo_result = vkGetPhysicalDeviceSurfacePresentModesKHR(device->vo_handle, surface, &count, vo_modes);
	vulkan_result_assert_success(vo_result);
	vulkan_result_assert_complete(vo_result);
	return vo_modes;
}

RENDERER_API VkSurfaceFormatKHR* vulkan_physical_device_get_surface_formats(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count)
{
	u32 count;
	VkResult vo_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device->vo_handle, surface, &count, NULL);
	vulkan_result_assert_success(vo_result);
	*out_count = count;
	if(count == 0)
	{
		LOG_WRN("Surface format count is zero\n");
		return NULL;
	}
	VkSurfaceFormatKHR* vo_formats = memory_allocator_alloc_obj_array(device->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SURFACE_FORMAT_KHR_ARRAY, VkSurfaceFormatKHR, count);
	vo_result = vkGetPhysicalDeviceSurfaceFormatsKHR(device->vo_handle, surface, &count, vo_formats);
	vulkan_result_assert_success(vo_result);
	vulkan_result_assert_complete(vo_result);
	return vo_formats;
}

RENDERER_API VkSurfaceCapabilitiesKHR vulkan_physical_device_get_surface_capabilities(vulkan_physical_device_t* device, VkSurfaceKHR surface)
{
	VkSurfaceCapabilitiesKHR vo_capabilities;
	VkResult vo_result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->vo_handle, surface, &vo_capabilities);
	vulkan_result_assert_success(vo_result);
	return vo_capabilities;
}

RENDERER_API VkPhysicalDeviceLimits* vulkan_physical_device_get_limits(vulkan_physical_device_t* device)
{
	return &(vulkan_physical_device_get_properties(device)->limits);
}

RENDERER_API VkPhysicalDeviceProperties* vulkan_physical_device_get_properties(vulkan_physical_device_t* device)
{
	return &device->vo_properties;
}

RENDERER_API VkPhysicalDeviceFeatures* vulkan_physical_device_get_features(vulkan_physical_device_t* device)
{
	return &device->vo_features;
}

RENDERER_API VkQueueFamilyProperties* vulkan_physical_device_get_queue_family_properties(vulkan_physical_device_t* device)
{
	return device->vo_queue_family_properties;
}

RENDERER_API u32 vulkan_physical_device_get_queue_family_count(vulkan_physical_device_t* device)
{
	return device->queue_family_count;
}

RENDERER_API u32 vulkan_physical_device_find_queue_family_index(vulkan_physical_device_t* device, VkQueueFlags queue_flags)
{
	VkQueueFamilyProperties* families = vulkan_physical_device_get_queue_family_properties(device);
	u32 count = vulkan_physical_device_get_queue_family_count(device);
	for(u32 i = 0; i < count; i++)
		if((families[i].queueFlags & queue_flags) == queue_flags)
			return i;
	return U32_MAX; 		// invalid
}

RENDERER_API u32 vulkan_physical_device_find_queue_family_index_for_surface(vulkan_physical_device_t* device, VkSurfaceKHR surface)
{
	VkQueueFamilyProperties* families = vulkan_physical_device_get_queue_family_properties(device);
	u32 count = vulkan_physical_device_get_queue_family_count(device);
	for(u32 i = 0; i < count; i++)
	{
		VkBool32 supported = VK_FALSE;
		VkResult vo_result = vkGetPhysicalDeviceSurfaceSupportKHR(device->vo_handle, i, surface, &supported);
		vulkan_result_assert_success(vo_result);
		if(supported == VK_TRUE)
			return i;
	}
	return U32_MAX; 		// invalid
}

RENDERER_API u32 vulkan_physical_device_find_memory_type(vulkan_physical_device_t* device, u32 required_memory_type_bits, VkMemoryPropertyFlags required_memory_properties)
{
	
	// get physical device memory properties
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(device->vo_handle, &memory_properties);

	for(u32 i = 0; i < memory_properties.memoryTypeCount; i++)
		if((required_memory_type_bits & (1 << 0)) && ((memory_properties.memoryTypes[i].propertyFlags & required_memory_properties) == required_memory_properties))
			return i;

	log_wrn("Couldn't find the required memory type in the physical device, returned U32_MAX\n");
	return U32_MAX; 		// invalid
}

RENDERER_API VkFormat vulkan_physical_device_find_supported_format(vulkan_physical_device_t* device, const VkFormat* const formats, u32 format_count, VkImageTiling tiling, VkFormatFeatureFlags format_features)
{
	VkFormatProperties properties;
	for(uint32_t i = 0; i < format_count; i++)
	{
		VkFormat format = formats[i];
		vkGetPhysicalDeviceFormatProperties(device->vo_handle, format, &properties);
		switch(tiling)
		{
			case VK_IMAGE_TILING_LINEAR:
				if((properties.linearTilingFeatures & format_features) == format_features)
					return format;
			break;
			
			case VK_IMAGE_TILING_OPTIMAL:
				if((properties.optimalTilingFeatures & format_features) == format_features)
					return format;
			break;

			default:
				LOG_FETAL_ERR("Unsupported image tiling\n");
			break;
		}
	}
	log_wrn("No vulkan format is found for image tiling: %u, format features: %u\n", tiling, format_features);
	return VK_FORMAT_UNDEFINED; // invalid
}

// returning bools
RENDERER_API bool vulkan_physical_device_meets_feature_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceFeatures* required_features)
{
	VkPhysicalDeviceFeatures* features = vulkan_physical_device_get_features(device);
	return 	((features->geometryShader == VK_TRUE) || (required_features->geometryShader = VK_FALSE)) 				// for now, just check for geometry shader and tessellation shader
			&& ((features->tessellationShader == VK_TRUE) == (required_features->tessellationShader == VK_FALSE));
}

RENDERER_API bool vulkan_physical_device_meets_property_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceProperties* required_properties)
{
	return vulkan_physical_device_get_properties(device)->deviceType == required_properties->deviceType; // for now, just check for the device type
}

RENDERER_API bool vulkan_physical_device_is_extension_supported(vulkan_physical_device_t* device, const char* extension)
{
	for(u32 i = 0; i < device->extension_count; i++)
		if(strcmp(device->vo_extension_properties[i].extensionName, extension) == 0)
			return true;
	return false;
}


// to string (s)
RENDERER_API void vulkan_physical_device_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	vulkan_physical_device_properties_to_string(device, string_buffer);
	vulkan_physical_device_limits_to_string(device, string_buffer);
	vulkan_physical_device_extensions_to_string(device, string_buffer);
}

RENDERER_API void vulkan_physical_device_limits_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	vk_physical_device_limits_to_string("Limits:\n", vulkan_physical_device_get_limits(device), string_buffer);
}

RENDERER_API void vulkan_physical_device_properties_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	vk_physical_device_properties_to_string("Properties:\n", vulkan_physical_device_get_properties(device), string_buffer);
}

RENDERER_API void vulkan_physical_device_extensions_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	buf_push_string(string_buffer, "Extensions: \n");
	for(u32 i = 0; i < device->extension_count; i++)
	{
		buf_push_char(string_buffer, '\t');
		buf_push_char(string_buffer, '\t');
		buf_push_string(string_buffer, device->vo_extension_properties[i].extensionName);
		buf_push_char(string_buffer, '\n');
	}
}
