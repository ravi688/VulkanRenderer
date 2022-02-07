
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
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

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_physical_device_t* vulkan_physical_device_new();
RENDERER_API vulkan_physical_device_t* vulkan_physical_device_create(VkPhysicalDevice vk_device);
RENDERER_API void vulkan_physical_device_destroy(vulkan_physical_device_t* device);
RENDERER_API void vulkan_physical_device_release_resources(vulkan_physical_device_t* device);

// getters
RENDERER_API VkPresentModeKHR* vulkan_physical_device_get_present_modes(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count);
RENDERER_API VkSurfaceFormatKHR* vulkan_physical_device_get_surface_formats(vulkan_physical_device_t* device, VkSurfaceKHR surface, u32* out_count);
RENDERER_API VkSurfaceCapabilitiesKHR vulkan_physical_device_get_surface_capabilities(vulkan_physical_device_t* device, VkSurfaceKHR surface);
RENDERER_API VkPhysicalDeviceLimits* vulkan_physical_device_get_limits(vulkan_physical_device_t* device);
RENDERER_API VkPhysicalDeviceProperties* vulkan_physical_device_get_properties(vulkan_physical_device_t* device);
RENDERER_API VkPhysicalDeviceFeatures* vulkan_physical_device_get_features(vulkan_physical_device_t* device);
RENDERER_API VkQueueFamilyProperties* vulkan_physical_device_get_queue_family_properties(vulkan_physical_device_t* device);
RENDERER_API u32 vulkan_physical_device_get_queue_family_count(vulkan_physical_device_t* device);
// returns U32_MAX if not found
RENDERER_API u32 vulkan_physical_device_find_queue_family_index(vulkan_physical_device_t* device, VkQueueFlags queue_flags);
RENDERER_API u32 vulkan_physical_device_find_queue_family_index_for_surface(vulkan_physical_device_t* device, VkSurfaceKHR surface);
RENDERER_API u32 vulkan_physical_device_find_memory_type(vulkan_physical_device_t* device, u32 required_memory_type_bits, VkMemoryPropertyFlags required_memory_properties);
RENDERER_API VkFormat vulkan_physical_device_find_supported_format(vulkan_physical_device_t* device, const VkFormat* const formats, u32 format_count, VkImageTiling tiling, VkFormatFeatureFlags format_features);

// returning bools
RENDERER_API bool vulkan_physical_device_meets_feature_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceFeatures* required_features);
RENDERER_API bool vulkan_physical_device_meets_property_requirements(vulkan_physical_device_t* device, VkPhysicalDeviceProperties* required_properties);
RENDERER_API bool vulkan_physical_device_is_extension_supported(vulkan_physical_device_t* device, const char* extension);

// to string (s)
RENDERER_API void vulkan_physical_device_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_limits_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_properties_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);
RENDERER_API void vulkan_physical_device_extensions_to_string(vulkan_physical_device_t* device, BUFFER* string_buffer);

END_CPP_COMPATIBLE
