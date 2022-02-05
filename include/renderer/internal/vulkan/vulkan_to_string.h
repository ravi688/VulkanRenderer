
#pragma once


#include <vulkan/vulkan.h>
#include <buffer.h>
#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

// VkFormat
RENDERER_API void vk_format_to_string(const char* description, VkFormat format, BUFFER* string_buffer);

// VkSurfaceFormatKHR
RENDERER_API void vk_present_mode_to_string(const char* description, VkPresentModeKHR present_mode, BUFFER* string_buffer);

// VkPresentModeKHR
RENDERER_API void vk_surface_format_to_string(const char* description, VkSurfaceFormatKHR surface_format, BUFFER* string_buffer);

// VkSurfaceTransformFlagBits
RENDERER_API void vk_surface_transform_flag_bits_to_string(const char* description, VkSurfaceTransformFlagBitsKHR transform, BUFFER* string_buffer);

// VkSurfaceTransformFlags
RENDERER_API void vk_surface_transform_flags_to_string(const char* description, VkSurfaceTransformFlagsKHR transform_flags, BUFFER* string_buffer);

// VkSurfaceCapabilitiesKHR
RENDERER_API void vk_surface_capabilities_to_string(const char* description, VkSurfaceCapabilitiesKHR* capabilities, BUFFER* string_buffer);

// VkBool32
RENDERER_API void vk_bool32_to_string(const char* description, VkBool32 value, BUFFER* string_buffer);

// VkSampleCountFlags
RENDERER_API void vk_sample_count_flags_to_string(const char* description, VkSampleCountFlags flags, BUFFER* string_buffer);

// VkPhysicalDeviceType
RENDERER_API void vk_physical_device_type_to_string(const char* description, VkPhysicalDeviceType type, BUFFER* string_buffer);

// VkPhysicalDeviceProperties
RENDERER_API void vk_physical_device_properties_to_string(const char* description, VkPhysicalDeviceProperties* properties, BUFFER* string_buffer);

// VkPhysicalDeviceLimits
RENDERER_API void vk_physical_device_limits_to_string(const char* description, VkPhysicalDeviceLimits* limits, BUFFER* string_buffer);

END_CPP_COMPATIBLE
