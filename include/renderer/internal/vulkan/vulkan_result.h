
#pragma once

#include <vulkan/vulkan.h>
#include <buffer.h>

// doesn't do anything if result == VK_SUCCESS
void vulkan_result_assert_success(VkResult result);

// doesn't do anything if result != VK_TIMEOUT
void vulkan_result_assert_on_time(VkResult result);

// doesn't do anything if result != VK_INCOMPLETE
void vulkan_result_assert_complete(VkResult result);

// doesn't do anything if result != VK_ERROR_*
void vulkan_result_assert_no_error(VkResult result);

// to string (s)
void vulkan_result_to_string(VkResult result, BUFFER* string_buffer);
const char* vulkan_result_to_string_literal(VkResult result);
