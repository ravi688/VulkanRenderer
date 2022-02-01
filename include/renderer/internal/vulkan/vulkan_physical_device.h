
#pragma once

#include <renderer/defines.h>
#include <buffer.h>
#include <vulkan/vulkan.h>

#include <renderer/internal/vulkan/vulkan_logical_device.h>

typedef struct vulkan_physical_device_t
{
	VkPhysicalDevice handle;
	VkPhysicalDeviceProperties properties;			// cached properties
	VkPhysicalDeviceFeatures features;				// cached features
	VkQueueFamilyProperties* queue_family_properties;
	u32 queue_family_count;
	VkExtensionProperties* extension_properties;
	u32 extension_count;
	//BUFFER /* vulkan_logical_device_t* */ logical_devices; 			// all the logical devices created with this physical device
} vulkan_physical_device_t;


// constructors and destructors
vulkan_physical_device_t* vulkan_physical_device_new();
vulkan_physical_device_t* vulkan_physical_device_create(VkPhysicalDevice vk_device);
void vulkan_physical_device_destroy(vulkan_physical_device_t* device);
void vulkan_physical_device_release_resources(vulkan_physical_device_t* device);

// getters
VkPhysicalDeviceLimits* vulkan_physical_device_get_limits(vulkan_physical_device_t* device);
VkPhysicalDeviceProperties* vulkan_physical_device_get_properties(vulkan_physical_device_t* device);
VkPhysicalDeviceFeatures* vulkan_physical_device_get_features(vulkan_physical_device_t* device);
VkQueueFamilyProperties* vulkan_physical_device_get_queue_family_properties(vulkan_physical_device_t* device);
u32 vulkan_physical_device_get_queue_family_count(vulkan_physical_device_t* device);
// returns U32_MAX if not found
u32 vulkan_physical_device_find_queue_family_index(vulkan_physical_device_t* device, VkQueueFlags queue_flags);
u32 vulkan_physical_device_find_queue_family_index_for_surface(vulkan_physical_device_t* device, VkSurfaceKHR surface);

// returning bools
bool vulkan_physical_device_meets_feature_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceFeatures* required_features);
bool vulkan_physical_device_meets_property_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceProperties* required_properties);
bool vulkan_physical_device_is_extension_supported(vulkan_physical_device_t* device, const char* extension);

// to string (s)
void vulkan_physical_device_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
void vulkan_physical_device_limits_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
void vulkan_physical_device_properties_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
void vulkan_physical_device_extensions_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
