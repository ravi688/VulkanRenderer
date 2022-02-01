
#include <renderer/internal/vulkan/vulkan_physical_device.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/internal/vulkan/vulkan_to_string.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <string.h> 		// strcmp

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_physical_device_t* device);
#else
#	define check_pre_condition(device)
#endif /* GLOBAL_DEBUG */

// constructors and destructors
vulkan_physical_device_t* vulkan_physical_device_new()
{
	vulkan_physical_device_t* device = heap_new(vulkan_physical_device_t);
	memset(device, 0, sizeof(vulkan_physical_device_t));
	return device;
}

vulkan_physical_device_t* vulkan_physical_device_create(VkPhysicalDevice vk_device)
{
	vulkan_physical_device_t* device = vulkan_physical_device_new();
	device->handle = vk_device;

	// create list of logical devices to store all the logical devices created with this physical device
	// device->logical_devices = buf_create(sizeof(vulkan_logical_device_t*), 0, 0);

	// cache the physical device properties
	vkGetPhysicalDeviceProperties(device->handle, &device->properties);

	// cache the physical device features
	vkGetPhysicalDeviceFeatures(device->handle, &device->features);

	// cache the queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &device->queue_family_count, NULL);
	assert(device->queue_family_count != 0);
	device->queue_family_properties = heap_newv(VkQueueFamilyProperties, device->queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &device->queue_family_count, device->queue_family_properties);

	// cache physical device extensions
	VkResult result = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &device->extension_count, NULL);
	vulkan_result_assert_success(result);
	device->extension_properties = heap_newv(VkExtensionProperties, device->extension_count);
	result = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &device->extension_count, device->extension_properties);
	vulkan_result_assert_success(result);
	vulkan_result_assert_complete(result);

	return device;
}

void vulkan_physical_device_destroy(vulkan_physical_device_t* device)
{
	// for(buf_ucount_t i = 0; i < buf_get_element_count(&device->logical_devices); i++)
	// 	vulkan_logical_device_destroy(*buf_get_ptr_at_typeof(&device->logical_devices, vulkan_logical_device_t*, i));
}

void vulkan_physical_device_release_resources(vulkan_physical_device_t* device)
{
	// for(buf_ucount_t i = 0; i < buf_get_element_count(&device->logical_devices); i++)
	// 	vulkan_logical_device_release_resources(*buf_get_ptr_at_typeof(&device->logical_devices, vulkan_logical_device_t*, i));
	// buf_free(&device->logical_devices);
	heap_free(device->queue_family_properties);
	heap_free(device->extension_properties);
	heap_free(device);
}


// getters
VkPhysicalDeviceLimits* vulkan_physical_device_get_limits(vulkan_physical_device_t* device)
{
	return &(vulkan_physical_device_get_properties(device)->limits);
}

VkPhysicalDeviceProperties* vulkan_physical_device_get_properties(vulkan_physical_device_t* device)
{
	check_pre_condition(device);
	return &device->properties;
}

VkPhysicalDeviceFeatures* vulkan_physical_device_get_features(vulkan_physical_device_t* device)
{
	check_pre_condition(device);
	return &device->features;
}

VkQueueFamilyProperties* vulkan_physical_device_get_queue_family_properties(vulkan_physical_device_t* device)
{
	check_pre_condition(device);
	return device->queue_family_properties;
}

u32 vulkan_physical_device_get_queue_family_count(vulkan_physical_device_t* device)
{
	check_pre_condition(device);
	return device->queue_family_count;
}

u32 vulkan_physical_device_find_queue_family_index(vulkan_physical_device_t* device, VkQueueFlags queue_flags)
{
	check_pre_condition(device);
	VkQueueFamilyProperties* families = vulkan_physical_device_get_queue_family_properties(device);
	u32 count = vulkan_physical_device_get_queue_family_count(device);
	for(u32 i = 0; i < count; i++)
		if((families[i].queueFlags & queue_flags) == queue_flags)
			return i;
	return U32_MAX; 		// invalid
}

u32 vulkan_physical_device_find_queue_family_index_for_surface(vulkan_physical_device_t* device, VkSurfaceKHR surface)
{
	check_pre_condition(device);
	VkQueueFamilyProperties* families = vulkan_physical_device_get_queue_family_properties(device);
	u32 count = vulkan_physical_device_get_queue_family_count(device);
	for(u32 i = 0; i < count; i++)
	{
		VkBool32 supported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device->handle, i, surface, &supported);
		vulkan_result_assert_success(result);
		if(supported == VK_TRUE)
			return i;
	}
	return U32_MAX; 		// invalid
}

// returning bools
bool vulkan_physical_device_meets_feature_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceFeatures* required_features)
{
	check_pre_condition(device);
	VkPhysicalDeviceFeatures* features = vulkan_physical_device_get_features(device);
	return 	((features->geometryShader == VK_TRUE) || (required_features->geometryShader = VK_FALSE)) 				// for now, just check for geometry shader and tessellation shader
			&& ((features->tessellationShader == VK_TRUE) == (required_features->tessellationShader == VK_FALSE));
}

bool vulkan_physical_device_meets_property_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceProperties* required_properties)
{
	check_pre_condition(device);
	return vulkan_physical_device_get_properties(device)->deviceType == required_properties->deviceType; // for now, just check for the device type
}

bool vulkan_physical_device_is_extension_supported(vulkan_physical_device_t* device, const char* extension)
{
	check_pre_condition(device);
	for(u32 i = 0; i < device->extension_count; i++)
		if(strcmp(device->extension_properties[i].extensionName, extension) == 0)
			return true;
	return false;
}


// to string (s)
void vulkan_physical_device_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	vulkan_physical_device_properties_to_string(device, string_buffer);
	vulkan_physical_device_limits_to_string(device, string_buffer);
	vulkan_physical_device_extensions_to_string(device, string_buffer);
}

void vulkan_physical_device_limits_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	check_pre_condition(device);
	vk_physical_device_limits_to_string("Limits:\n", vulkan_physical_device_get_limits(device), string_buffer);
}

void vulkan_physical_device_properties_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	vk_physical_device_properties_to_string("Properties:\n", vulkan_physical_device_get_properties(device), string_buffer);
}

void vulkan_physical_device_extensions_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer)
{
	check_pre_condition(device);
	buf_push_string(string_buffer, "Extensions: \n");
	for(u32 i = 0; i < device->extension_count; i++)
	{
		buf_push_char(string_buffer, '\t');
		buf_push_char(string_buffer, '\t');
		buf_push_string(string_buffer, device->extension_properties[i].extensionName);
		buf_push_char(string_buffer, '\n');
	}
}

#ifdef GLOBAL_DEBUG
static void check_pre_condition(vulkan_physical_device_t* device)
{
	assert(device != NULL);
	assert(device->handle != VK_NULL_HANDLE);
}
#endif /* GLOBAL_DEBUG */
