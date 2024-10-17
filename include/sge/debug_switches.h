/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: debug_switches.h is a part of VulkanRenderer

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

/* debug switches */
#define ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH 0
#define ENABLE_DBG_BUFFER2D_RESIZE 1
#define ENABLE_DBG_EVENT_PUBLISH 0

/* switches */
/* if enabled, then objects with shared materials and shaders will be drawn in the same render pass.
 * otherwise, each object will be rendered in separated (though inefficient) render passes. */
#define ENABLE_OBJECT_GROUPING_IN_TRANSPARENT_QUEUE 1

#define ENABLED(x) (ENABLE_##x == 1)

#ifdef GLOBAL_DEBUG	
#	define DBG_ENABLED(x) (ENABLE_DBG_##x == 1)
#else
#	define DBG_ENABLED(x) 0
#endif

#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
#	define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) x
#	define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) , x
#	define NOT_DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#	define NOT_DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#else
#	define DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#	define DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) /* nothing */
#	define NOT_DBG_VULKAN_BITMAP_TEXT_STRING_SETH(x) x
#	define NOT_DBG_ARG_VULKAN_BITMAP_TEXT_STRING_SETH(x) , x
#endif /* ENABLE_DBG_VULKAN_BITMAP_TEXT_STRING_SETH */
#if DBG_ENABLED(BUFFER2D_RESIZE)
#	define DBG_BUFFER2D_RESIZE(x) x
#	define DBG_ARG_BUFFER2D_RESIZE(x) , x
#	define NOT_DBG_BUFFER2D_RESIZE(x) /* nothing */
#	define NOT_DBG_ARG_BUFFER2D_RESIZE(x) /* nothing */
#else
#	define DBG_BUFFER2D_RESIZE(x) /* nothing */
#	define DBG_ARG_BUFFER2D_RESIZE(x) /* nothing */
#	define NOT_DBG_BUFFER2D_RESIZE(x) x
#	define NOT_DBG_ARG_BUFFER2D_RESIZE(x) , x
#endif /* ENABLE_DBG_BUFFER2D_RESIZE */
#if DBG_ENABLED(EVENT_PUBLISH)
#	define DBG_EVENT_PUBLISH(x) x
#	define DBG_ARG_EVENT_PUBLISH(x) , x
#	define NOT_DBG_EVENT_PUBLISH(x)
#	define NOT_DBG_ARG_EVENT_PUBLISH(x)
#else
#	define DBG_EVENT_PUBLISH(x)
#	define DBG_ARG_EVENT_PUBLISH(x)
#	define NOT_DBG_EVENT_PUBLISH(x) x
#	define NOT_DBG_ARG_EVENT_PUBLISH(x) , x
#endif
