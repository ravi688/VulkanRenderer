#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdbool.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>
#include <exception/exception.h>

#ifdef DEBUG
#	define vkCall(call)\
{\
	VkResult result =  call;\
	EXCEPTION_BLOCK\
	(\
		if(result != VK_SUCCESS)\
			throw_exception(VULKAN_ABORTED);\
	)\
}

#else
#	define vkCall(call) call
#endif


declare_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
declare_exception(VULKAN_LAYER_NOT_SUPPORTED);
declare_exception(VULKAN_ABORTED);
declare_exception(VULKAN_DEVICE_NOT_FOUND);


typedef VkPhysicalDevice* pVkPhysicalDevice_t; 
instantiate_tuple_t(uint32_t, pVkPhysicalDevice_t);

typedef VkExtensionProperties* pVkExtensionProperties_t; 
instantiate_tuple_t(uint32_t, pVkExtensionProperties_t);

typedef VkLayerProperties* pVkLayerProperties_t;
instantiate_tuple_t(uint32_t, pVkLayerProperties_t);

typedef const char* const* ppVkChar_t;
instantiate_tuple_t(uint32_t, ppVkChar_t);

typedef VkQueueFamilyProperties* pVkQueueFamilyProperties_t; 
instantiate_tuple_t(uint32_t, pVkQueueFamilyProperties_t);

typedef VkSurfaceFormatKHR* pVkSurfaceFormatKHR_t;
instantiate_tuple_t(uint32_t, pVkSurfaceFormatKHR_t);

typedef VkPresentModeKHR* pVkPresentModeKHR_t; 
instantiate_tuple_t(uint32_t, pVkPresentModeKHR_t);

void vk_setup_validation_layers();
VkInstance vk_create_instance(); 
tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance);
tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_instance_extension_properties();
tuple_t(uint32_t, pVkExtensionProperties_t) vk_get_physical_device_extension_properties(VkPhysicalDevice physical_device);
tuple_t(uint32_t, pVkLayerProperties_t) vk_get_instance_layer_properties();
tuple_t(uint32_t, pVkQueueFamilyProperties_t) vk_get_queue_family_properties(VkPhysicalDevice physical_device);
tuple_t(uint32_t, pVkSurfaceFormatKHR_t) vk_get_physical_device_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
tuple_t(uint32_t, pVkPresentModeKHR_t) vk_get_physical_device_surface_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

VkSurfaceCapabilitiesKHR vk_get_physical_device_surface_capabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice physical_device);
VkPhysicalDeviceFeatures vk_get_physical_device_features(VkPhysicalDevice physical_device);
VkPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties(VkPhysicalDevice physical_device);
bool vk_check_layer_support(tuple_t(uint32_t, ppVkChar_t) layers);
bool vk_check_instance_extension_support(tuple_t(uint32_t, ppVkChar_t) extensions);
bool vk_check_physical_device_extension_support(VkPhysicalDevice device, tuple_t(uint32_t, ppVkChar_t) extensions);
VkPhysicalDevice vk_select_suitable_device(tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices);
//NOT_IMPLEMENTED VkPhysicalDeviceQueueFamilyProperites vk_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);

void vk_dump_instance_extensions(); 
void vk_dump_instance_layers();
void vk_dump_physical_devices(tuple_t(uint32_t, pVkPhysicalDevice_t)* physical_devices);
void vk_dump_queue_families(tuple_t(uint32_t, pVkQueueFamilyProperties_t)* queue_families);


const char* vk_physical_device_memory_properties_to_string(VkPhysicalDeviceMemoryProperties *memory_properties);
const char* vk_physical_device_properties_to_string(VkPhysicalDeviceProperties* properties);
const char* vk_physical_device_type_to_string(VkPhysicalDeviceType* deviceType);
const char* vk_physical_device_features_to_string(VkPhysicalDeviceFeatures* device_features);
const char* vk_physical_device_queue_family_to_string(VkQueueFamilyProperties properties);