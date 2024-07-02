/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: hash_function.h is a part of VulkanRenderer

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

typedef u64 hash_t;
typedef hash_t (*hash_function_t)(void* key);

BEGIN_CPP_COMPATIBLE

SGE_API hash_t string_hash(void* v);
SGE_API hash_t ptr_hash(void* v);
SGE_API hash_t s8_hash(void* v);
#define char_hash(void_ptr) s8_hash(void_ptr)
SGE_API hash_t s16_hash(void* v);
SGE_API hash_t s32_hash(void* v);
SGE_API hash_t s64_hash(void* v);
SGE_API hash_t u8_hash(void* v);
SGE_API hash_t u16_hash(void* v);
SGE_API hash_t u32_hash(void* v);
SGE_API hash_t u64_hash(void* v);
SGE_API hash_t float_hash(void* v);
SGE_API hash_t double_hash(void* v);

END_CPP_COMPATIBLE
