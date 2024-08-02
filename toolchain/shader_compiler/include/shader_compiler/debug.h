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

#ifdef SC_VERBOSE
#	define sc_debug_log_info_verbose(...) debug_log_info(__VA_ARGS__)
#	define sc_debug_log_warning_verbose(...) debug_log_warning(__VA_ARGS__)
#	define sc_debug_log_error_verbose(...) debug_log_error(__VA_ARGS__)
#	define SC_DEBUG_LOG_INFO_VERBOSE(...) DEBUG_LOG_INFO(__VA_ARGS__)
#	define SC_DEBUG_LOG_WARNING_VERBOSE(...) DEBUG_LOG_WARNING(__VA_ARGS__)
#	define SC_DEBUG_LOG_ERROR_VERBOSE(...) DEBUG_LOG_ERROR(__VA_ARGS__)
#else
#	define sc_debug_log_info_verbose(...)
#	define sc_debug_log_warning_verbose(...)
#	define sc_debug_log_error_verbose(...)
#	define SC_DEBUG_LOG_INFO_VERBOSE(...)
#	define SC_DEBUG_LOG_WARNING_VERBOSE(...)
#	define SC_DEBUG_LOG_ERROR_VERBOSE(...)
#endif
