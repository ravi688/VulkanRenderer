
#pragma once


#include <vulkan/vulkan.h>
#include <buffer.h>

// VkBool32
void vk_bool32_to_string(const char* description, VkBool32 value, BUFFER* string_buffer);

// VkSampleCountFlags
void vk_sample_count_flags_to_string(const char* description, VkSampleCountFlags flags, BUFFER* string_buffer);

// VkPhysicalDeviceType
void vk_physical_device_type_to_string(const char* description, VkPhysicalDeviceType type, BUFFER* string_buffer);

// VkPhysicalDeviceProperties
void vk_physical_device_properties_to_string(const char* description, VkPhysicalDeviceProperties* properties, BUFFER* string_buffer);

// VkPhysicalDeviceLimits
void vk_physical_device_limits_to_string(const char* description, VkPhysicalDeviceLimits* limits, BUFFER* string_buffer);
