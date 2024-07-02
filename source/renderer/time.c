/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: time.c is a part of VulkanRenderer

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


#include <renderer/time.h>


#include <common/platform.h>

#ifdef PLATFORM_WINDOWS
#	ifdef _USE_32BIT_TIME_T
#		undef _USE_32BIT_TIME_T
#	endif
#	include <sys/time.h>
#elif defined(PLATFORM_LINUX)
#	include <time.h>
#endif

#include <stdio.h>

SGE_API time_handle_t time_get_handle()
{
	return clock();
}

SGE_API float time_get_seconds(time_handle_t handle)
{
	return (float)(clock() - handle) / CLOCKS_PER_SEC;
}

SGE_API uint32_t time_get_milliseconds(time_handle_t handle)
{
	return (float)(clock() - handle) * (1000 / CLOCKS_PER_SEC);
}


SGE_API float time_get_delta_time(time_handle_t* handle)
{
	float delta_time = time_get_seconds(*handle);
	*handle = time_get_handle();
	return delta_time;
}