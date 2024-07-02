/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Time.hpp is a part of VulkanRenderer

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

#include <sge/time.h>

namespace Showcase
{
	class SHOWCASE_API Time
	{

		Time() = delete; 		// static class only

	public:

		typedef time_handle_t Handle;

		static inline Handle createHandle() { return time_get_handle(); }
		static inline float getSeconds(Handle handle) { return time_get_seconds(handle); }
		static inline float getMilliseconds(Handle handle) { return time_get_milliseconds(handle); }
		static inline float getDeltaTime(Handle& handle) { return time_get_delta_time(&handle); }
	};
}