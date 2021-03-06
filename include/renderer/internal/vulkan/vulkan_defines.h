
#pragma once

#ifdef DEBUG
#	include <renderer/internal/vulkan/vulkan_result.h>
#	define vkCall(result) vulkan_result_assert_success(result)
#else
#	define vkCall(call) call
#endif
