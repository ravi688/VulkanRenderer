/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.h is a part of VulkanRenderer

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

#include <shader_compiler/defines.h>

#include <string.h>
#include <ctype.h>

SC_API const char* skip_whitespaces(const char* str, const char* const end);
SC_API u32 __get_word_length(const char* str, const char* const end, const char* delimiters);
SC_API u32 get_word_length(const char* string, const char delimiter);
SC_API int safe_strncmp(const char* str1, const char* const str2, u32 len);
SC_API bool is_empty(const char* start, const char* const end);
SC_API void remove_comments(char* start, const char* const end);
