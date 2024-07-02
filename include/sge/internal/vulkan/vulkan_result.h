/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_result.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

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


#pragma once

#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>
#include <sge/defines.h>

BEGIN_CPP_COMPATIBLE

// doesn't do anything if result == VK_SUCCESS
// SGE_API void vulkan_result_assert_success(VkResult result);
#define vulkan_result_assert_success(result) __vulkan_result_assert_success(result, __LINE__, __FUNCTION__, __FILE__)
SGE_API void __vulkan_result_assert_success(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_TIMEOUT
#define vulkan_result_assert_on_time(result) __vulkan_result_assert_on_time(result, __LINE__, __FUNCTION__, __FILE__)
SGE_API void __vulkan_result_assert_on_time(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_INCOMPLETE
#define vulkan_result_assert_complete(result) __vulkan_result_assert_complete(result, __LINE__, __FUNCTION__, __FILE__)
SGE_API void __vulkan_result_assert_complete(VkResult result, u32 line, const char* fnname, const char* flname);

// doesn't do anything if result != VK_ERROR_*
#define vulkan_result_assert_no_error(result) __vulkan_result_assert_no_error(result, __LINE__, __FUNCTION__, __FILE__)
SGE_API void __vulkan_result_assert_no_error(VkResult result, u32 line, const char* fnname, const char* flname);

// to string (s)
SGE_API void vulkan_result_to_string(VkResult result, BUFFER* string_buffer);
SGE_API const char* vulkan_result_to_string_literal(VkResult result);

END_CPP_COMPATIBLE
