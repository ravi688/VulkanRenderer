/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: garbage_collector.h is a part of VulkanRenderer

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
#include <stdlib.h>

BEGIN_CPP_COMPATIBLE

// RENDERER_API extern tgc_t gc;

END_CPP_COMPATIBLE

#ifndef DEBUG
#	define GC_START(stack_address) //tgc_start(&gc, (void*)(stack_address))
#	define GC_STOP() //tgc_stop(&gc)
#	define GC_RUN() //tgc_run(&gc)
#	define GC_PAUSE() //tgc_pause(&gc)
#	define GC_RESUME() //tgc_resume(&gc)
#	define GC_ALLOC(size) malloc(size)
#	define GC_FREE(ptr) free(ptr)
#else
#	include <stdlib.h>
#	define GC_START(stack_address)
#	define GC_STOP()
#	define GC_RUN()
#	define GC_PAUSE()
#	define GC_RESUME()
#	define GC_ALLOC(size) malloc(size)
#	define GC_FREE(ptr) free(ptr)
#endif

#define GC_NEW(type) (type*)GCALLOC(sizeof(type))
#define GC_NEWV(type, count) (type*)GC_ALLOC(sizeof(type) * (count))
