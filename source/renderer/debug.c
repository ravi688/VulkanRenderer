/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: debug.c is a part of VulkanRenderer

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

#include <renderer/debug.h>

RENDERER_API void debug_break__(debug_break_reason_t reason)
{
	switch(reason)
	{
		case DEBUG_BREAK_REASON_NONE:
		case DEBUG_BREAK_REASON_INVALID_MEMORY_ACCESS:
		case DEBUG_BREAK_REASON_ASSERTION_FAILED:
		case DEBUG_BREAK_REASON_UNKNOWN:
		default:
			__builtin_trap();
	}
}
