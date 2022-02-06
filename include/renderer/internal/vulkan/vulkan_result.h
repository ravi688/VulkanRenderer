
#pragma once

#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>
#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

// doesn't do anything if result == VK_SUCCESS
RENDERER_API void vulkan_result_assert_success(VkResult result);

// doesn't do anything if result != VK_TIMEOUT
RENDERER_API void vulkan_result_assert_on_time(VkResult result);

// doesn't do anything if result != VK_INCOMPLETE
RENDERER_API void vulkan_result_assert_complete(VkResult result);

// doesn't do anything if result != VK_ERROR_*
RENDERER_API void vulkan_result_assert_no_error(VkResult result);

// to string (s)
RENDERER_API void vulkan_result_to_string(VkResult result, BUFFER* string_buffer);
RENDERER_API const char* vulkan_result_to_string_literal(VkResult result);

END_CPP_COMPATIBLE
