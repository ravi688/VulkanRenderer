/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: time.h is a part of VulkanRenderer

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

#include <stdint.h>
#include <renderer/defines.h>

typedef u32 time_handle_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API time_handle_t time_get_handle();
RENDERER_API float time_get_seconds(time_handle_t handle);
RENDERER_API u32 time_get_milliseconds(time_handle_t handle);
RENDERER_API float time_get_delta_time(time_handle_t* handle);

END_CPP_COMPATIBLE
