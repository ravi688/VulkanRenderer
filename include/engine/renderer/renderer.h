
#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <tuple/header_config.h>
#include <tuple/tuple.h>

#include <stdint.h>
#include <stdbool.h>
#include <no_compile_header.h>

typedef VkPhysicalDevice* pVkPhysicalDevice_t; 
instantiate_tuple_t(uint32_t, pVkPhysicalDevice_t);

typedef VkExtensionProperties* pVkExtensionProperties_t; 
instantiate_tuple_t(uint32_t, pVkExtensionProperties_t);

typedef VkLayerProperties* pVkLayerProperties_t;
instantiate_tuple_t(uint32_t, pVkLayerProperties_t);

typedef const char* const* ppVkChar_t;
instantiate_tuple_t(uint32_t, ppVkChar_t);


typedef struct renderer_t
{
	VkInstance vk_instance;
} renderer_t;


renderer_t* renderer_init();
void renderer_terminate(renderer_t* renderer);

VkInstance vk_create_instance(); 
tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance);
tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_instance_extension_properties();
tuple_t(uint32_t, pVkLayerProperties_t) vk_get_instance_layer_properties();

VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice physical_device);
VkPhysicalDeviceFeatures vk_get_physical_device_features(VkPhysicalDevice physical_device);
VkPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties(VkPhysicalDevice physical_device);
bool vk_check_layer_support(tuple_t(uint32_t, ppVkChar_t) layers);
bool vk_check_extension_support(tuple_t(uint32_t, ppVkChar_t) extensions);
//NOT_IMPLEMENTED VkPhysicalDeviceQueueFamilyProperites vk_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);


void vk_dump_instance_extensions(); 
void vk_dump_instance_layers();
const char* vk_physical_device_memory_properties_to_string(VkPhysicalDeviceMemoryProperties *memory_properties);
const char* vk_physical_device_properties_to_string(VkPhysicalDeviceProperties* properties);
const char* vk_physical_device_type_to_string(VkPhysicalDeviceType* deviceType);
