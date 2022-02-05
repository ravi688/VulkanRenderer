
#pragma once

#include <renderer/defines.h>
#include <buffer.h>
#include <vulkan/vulkan.h>

typedef struct vulkan_physical_device_t vulkan_physical_device_t;

typedef struct vulkan_instance_t
{
	VkInstance handle;

	// physical devices
	vulkan_physical_device_t** physical_devices;
	u32 physical_device_count;

	// extensions
	VkExtensionProperties* extension_properties;
	u32 extension_count;
} vulkan_instance_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_instance_t* vulkan_instance_new();
RENDERER_API vulkan_instance_t* vulkan_instance_create(const char* const* extensions, u32 extension_count);
RENDERER_API void vulkan_instance_destroy(vulkan_instance_t* instance);
RENDERER_API void vulkan_instance_release_resources(vulkan_instance_t* instance);

// getters
RENDERER_API u32 vulkan_instance_get_physical_device_count(vulkan_instance_t* instance);
RENDERER_API u32 vulkan_instance_get_extension_count(vulkan_instance_t* instance, const char* layer_name);
RENDERER_API vulkan_physical_device_t** vulkan_instance_get_physical_devices(vulkan_instance_t* instance);
RENDERER_API VkExtensionProperties* vulkan_instance_get_extension_properties(vulkan_instance_t* instance, const char* layer_name);

// returning bools
RENDERER_API bool vulkan_instance_is_extension_supported(vulkan_instance_t* instance, const char* extension, const char* layer_name);

// to string (s)
RENDERER_API void vulkan_instance_to_string(vulkan_instance_t* instance, BUFFER* string_buffer);

END_CPP_COMPATIBLE
