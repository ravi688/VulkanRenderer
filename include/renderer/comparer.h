/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: comparer.h is a part of VulkanRenderer

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

#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

typedef bool (*comparer_t)(void* lhs, void* rhs);

RENDERER_API bool string_equal_to(void* lhs, void* rhs);
RENDERER_API bool ptr_equal_to(void* lhs, void* rhs);
RENDERER_API bool s8_equal_to(void* lhs, void* rhs);
RENDERER_API bool s16_equal_to(void* lhs, void* rhs);
RENDERER_API bool s32_equal_to(void* lhs, void* rhs);
RENDERER_API bool s64_equal_to(void* lhs, void* rhs);
RENDERER_API bool u8_equal_to(void* lhs, void* rhs);
RENDERER_API bool u16_equal_to(void* lhs, void* rhs);
RENDERER_API bool u32_equal_to(void* lhs, void* rhs);
RENDERER_API bool u64_equal_to(void* lhs, void* rhs);
RENDERER_API bool float_equal_to(void* lhs, void* rhs);
RENDERER_API bool double_equal_to(void* lhs, void* rhs);

RENDERER_API bool string_greater_than(void* lhs, void* rhs);
RENDERER_API bool ptr_greater_than(void* lhs, void* rhs);
RENDERER_API bool s8_greater_than(void* lhs, void* rhs);
RENDERER_API bool s16_greater_than(void* lhs, void* rhs);
RENDERER_API bool s32_greater_than(void* lhs, void* rhs);
RENDERER_API bool s64_greater_than(void* lhs, void* rhs);
RENDERER_API bool u8_greater_than(void* lhs, void* rhs);
RENDERER_API bool u16_greater_than(void* lhs, void* rhs);
RENDERER_API bool u32_greater_than(void* lhs, void* rhs);
RENDERER_API bool u64_greater_than(void* lhs, void* rhs);
RENDERER_API bool float_greater_than(void* lhs, void* rhs);
RENDERER_API bool double_greater_than(void* lhs, void* rhs);

RENDERER_API bool string_less_than(void* lhs, void* rhs);
RENDERER_API bool ptr_less_than(void* lhs, void* rhs);
RENDERER_API bool s8_less_than(void* lhs, void* rhs);
RENDERER_API bool s16_less_than(void* lhs, void* rhs);
RENDERER_API bool s32_less_than(void* lhs, void* rhs);
RENDERER_API bool s64_less_than(void* lhs, void* rhs);
RENDERER_API bool u8_less_than(void* lhs, void* rhs);
RENDERER_API bool u16_less_than(void* lhs, void* rhs);
RENDERER_API bool u32_less_than(void* lhs, void* rhs);
RENDERER_API bool u64_less_than(void* lhs, void* rhs);
RENDERER_API bool float_less_than(void* lhs, void* rhs);
RENDERER_API bool double_less_than(void* lhs, void* rhs);

END_CPP_COMPATIBLE
