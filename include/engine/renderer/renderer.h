
#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <tuple/header_config.h>
#include <tuple/tuple.h>

#include <stdint.h>
#include <no_compile_header.h>

typedef VkPhysicalDevice* pVkPhysicalDevice_t; 
instantiate_tuple_t(uint32_t, pVkPhysicalDevice_t);

VkInstance vk_create_instance(VkInstanceCreateInfo* create_info, VkAllocationCallbacks* host_memory_allocator); 
tuple_t(uint32_t, pVkPhysicalDevice_t) vk_get_physical_devices(VkInstance instance);

VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice physical_device);
VkPhysicalDeviceFeatures vk_get_physical_device_features(VkPhysicalDevice physical_device);
NOT_IMPLEMENTED VkPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties(VkPhysicalDevice physical_device);
//NOT_IMPLEMENTED VkPhysicalDeviceQueueFamilyProperites vk_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);