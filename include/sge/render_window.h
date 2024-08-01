/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.h is a part of VulkanRenderer

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

#include <sge/defines.h>
#include <sge/event.h>
#include <sge/type_system.h>
#include <sge/object.h>

static const type_t TYPE_ID(render_window_t) = TYPE_ID_CREATE(1);

typedef struct render_window_t
{
	__OBJECT__;
	memory_allocator_t* allocator;

	/* handle to the internal GLFWwindow object */
	void* handle;

	/* width of the window (in pixels) */
	u32 width;
	/* height of the window (in pixels) */
	u32 height;

	/* event triggered whenever the window resizes */
	event_t* on_resize_event;
} render_window_t;

#define RENDER_WINDOW(ptr) OBJECT_UP_CAST(render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CONST(ptr) OBJECT_UP_CAST(const render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CAST(ptr) OBJECT_TYPE_CAST(render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CAST_CONST(ptr) OBJECT_TYPE_CAST_CONST(const render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)

BEGIN_CPP_COMPATIBLE

SGE_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
SGE_API bool render_window_should_close(render_window_t* window);
SGE_API void render_window_poll_events(render_window_t* window);
SGE_API void render_window_destroy(render_window_t* window);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE event_t* render_window_get_on_resize_event(render_window_t* window) { return window->on_resize_event; }

// getters
SGE_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height);

//API specific extensions
SGE_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface);

// Window System specific extension
// out_ptr: will be a double pointer (&(GLFWwindow*))
SGE_API void render_window_get_glfw_window_ptr(render_window_t* window, void* out_ptr);
END_CPP_COMPATIBLE
