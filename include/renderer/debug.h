/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: debug.h is a part of VulkanRenderer

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

#include <common/debug.h>
#include <calltrace/calltrace.h>

#define log_u32(var) log_msg("%s: %u\n", #var, var)
#define log_u64(var) log_msg("%s: %llu\n", #var, var)
#define log_ptr(var) log_msg("%s: %p\n", #var, var)

#if defined(GLOBAL_DEBUG)
#	define LOG_MSG(...) log_msg(__VA_ARGS__)
#	define LOG_ERR(...) log_err(__VA_ARGS__)
#	define LOG_WRN(...) log_wrn(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) log_fetal_err(__VA_ARGS__)
#else
#	define LOG_WRN(...) DEBUG_LOG_WARNING( __VA_ARGS__)
#	define LOG_MSG(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define LOG_ERR(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#	define LOG_FETAL_ERR(...) DEBUG_LOG_FETAL_ERROR(__VA_ARGS__)
#endif


#include <renderer/defines.h>

#if defined(debug_break)
#undef debug_break
#endif

typedef enum debug_break_reason_t
{
	DEBUG_BREAK_REASON_NONE,
	DEBUG_BREAK_REASON_INVALID_MEMORY_ACCESS,
	DEBUG_BREAK_REASON_ASSERTION_FAILED,
	DEBUG_BREAK_REASON_UNKNOWN
} debug_break_reason_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API NO_RETURN_FUNCTION void debug_break__(debug_break_reason_t reason);

END_CPP_COMPATIBLE
