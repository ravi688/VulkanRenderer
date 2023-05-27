/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_result.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/debug.h>
#include <stdlib.h>
#include <stdio.h>

RENDERER_API void __vulkan_result_assert_success(VkResult result, u32 line, const char* fnname, const char* flname)
{
	if(result == VK_SUCCESS)
		return;
	BUFFER buffer = buf_create(sizeof(char), 0, 0);
	vulkan_result_to_string(result, &buffer);
	debug_assert(line, fnname, flname, DESCRIPTION(false), " assertion was result == VK_SUCCESS, but result equals to \"%s\"", vulkan_result_to_string_literal(result));
	buf_free(&buffer);
}

// RENDERER_API void vulkan_result_assert_success(VkResult result)
// {
// 	if(result == VK_SUCCESS)
// 		return;
// 	BUFFER buffer = buf_create(sizeof(char), 0, 0);
// 	vulkan_result_to_string(result, &buffer);
// 	LOG_ERR("[Assertion Failed]: assertion was result == VK_SUCCESS, but result equals to \"%s\"\n", vulkan_result_to_string_literal(result));
// 	buf_free(&buffer);
// 	exit(0);
// }

RENDERER_API void __vulkan_result_assert_on_time(VkResult result, u32 line, const char* fnname, const char* flname)
{
	if(result != VK_TIMEOUT)
		return;
	BUFFER buffer = buf_create(sizeof(char), 0, 0);
	vulkan_result_to_string(result, &buffer);
	debug_assert(line, fnname, flname, DESCRIPTION(false), "assertion was result == VK_TIMEOUT, but result equals to \"%s\"", vulkan_result_to_string_literal(result));
	buf_free(&buffer);
}

RENDERER_API void __vulkan_result_assert_complete(VkResult result, u32 line, const char* fnname, const char* flname)
{
	if(result != VK_INCOMPLETE)
		return;
	BUFFER buffer = buf_create(sizeof(char), 0, 0);
	vulkan_result_to_string(result, &buffer);
	debug_assert(line, fnname, flname, DESCRIPTION(false), "assertion was result == VK_INCOMPLETE, but result equals to \"%s\"", vulkan_result_to_string_literal(result));
	buf_free(&buffer);
}

RENDERER_API void __vulkan_result_assert_no_error(VkResult result, u32 line, const char* fnname, const char* flname)
{
	if(result <= 5)
		return;
	BUFFER buffer = buf_create(sizeof(char), 0, 0);
	vulkan_result_to_string(result, &buffer);
	debug_assert(line, fnname, flname, DESCRIPTION(false), "assertion was result is not an error, but result equals to \"%s\"", vulkan_result_to_string_literal(result));
	buf_free(&buffer);
}

RENDERER_API void vulkan_result_to_string(VkResult result, BUFFER* string_buffer)
{
	_debug_assert__(buf_get_element_size(string_buffer) == sizeof(char));
	buf_push_string(string_buffer, (char*)vulkan_result_to_string_literal(result));
}

RENDERER_API const char* vulkan_result_to_string_literal(VkResult result)
{
	switch(result)
	{
		case VK_SUCCESS: return "VK_SUCCESS";
   		case VK_NOT_READY: return "VK_NOT_READY";
   		case VK_TIMEOUT: return "VK_TIMEOUT";
   		case VK_EVENT_SET: return "VK_EVENT_SET";
   		case VK_EVENT_RESET: return "VK_EVENT_RESET";
   		case VK_INCOMPLETE: return "VK_INCOMPLETE";
   		case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
   		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
   		case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
   		case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
   		case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
   		case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
   		case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
   		case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
   		case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
   		case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
   		case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
   		case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
   		case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
   		case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
   		case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
   		case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
   		case VK_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
   		case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
   		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
   		case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
   		case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
   		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
   		case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
   		case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
   		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
   		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
   		case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
   		case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
   		case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
   		case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
   		case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
   		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR";
	}
	return "Unkown VkResult";
}
