
#pragma once

#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>
#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

// doesn't do anything if result == VK_SUCCESS
// RENDERER_API void vulkan_result_assert_success(VkResult result);
#define vulkan_result_assert_success(result) __vulkan_result_assert_success(result, __LINE__, __FUNCTION__, __FILE__)
RENDERER_API void __vulkan_result_assert_success(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_TIMEOUT
#define vulkan_result_assert_on_time(result) __vulkan_result_assert_on_time(result, __LINE__, __FUNCTION__, __FILE__)
RENDERER_API void __vulkan_result_assert_on_time(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_INCOMPLETE
#define vulkan_result_assert_complete(result) __vulkan_result_assert_complete(result, __LINE__, __FUNCTION__, __FILE__)
RENDERER_API void __vulkan_result_assert_complete(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_ERROR_*
#define vulkan_result_assert_no_error(result) __vulkan_result_assert_no_error(result, __LINE__, __FUNCTION__, __FILE__)
RENDERER_API void __vulkan_result_assert_no_error(VkResult result, u32 line, const char* fnname, const char* flname);

// to string (s)
RENDERER_API void vulkan_result_to_string(VkResult result, BUFFER* string_buffer);
RENDERER_API const char* vulkan_result_to_string_literal(VkResult result);

END_CPP_COMPATIBLE
