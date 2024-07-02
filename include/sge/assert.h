/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: assert.h is a part of VulkanRenderer

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

#include <common/assert.h>

#define release_assert__(condition, ...) ASSERT(DESCRIPTION((condition) == true), __VA_ARGS__)
#define release_assert_wrn__(condition, ...) ASSERT_WRN(DESCRIPTION((condition) == true), __VA_ARGS__)
#define release_assert_not_null__(ptr) _ASSERT((ptr) != NULL)
#define release_assert_called_once__() static int __FUNCTION__##call_counter = 0; ++__FUNCTION__##call_counter; release_assert(__FUNCTION__##call_counter == 1, "%s has been called more than once\n", __FUNCTION__)
#define _release_assert__(condition) _ASSERT(condition)
#define _release_assert_wrn__(condition) _ASSERT_WRN(condition)

#if defined(GLOBAL_DEBUG)
#	define debug_assert__(...) release_assert__(__VA_ARGS__)
#	define debug_assert_wrn__(...) release_assert_wrn__(__VA_ARGS__)
#	define debug_assert_not_null__(ptr) release_assert_not_null__(ptr)
#	define debug_assert_called_once__() release_assert_called_once__()
#	define _debug_assert__(condition) _release_assert__(condition)
#	define _debug_assert_wrn__(condition) _release_assert_wrn__(condition)
#else
#	define debug_assert__(...)
#	define debug_assert_wrn__(...)
#	define debug_assert_not_null__(ptr)
#	define debug_assert_called_once__()
#	define _debug_assert__(condition)
#	define _debug_assert_wrn__(condition)
#endif
