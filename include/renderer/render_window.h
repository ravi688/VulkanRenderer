/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
#include <bufferlib/buffer.h>
#include <renderer/event.h>
#include <renderer/type_system.h>
#include <renderer/struct_descriptor.h>

static const type_t TYPE_ID(render_window_t) = TYPE_ID_CREATE(1);

typedef struct render_window_t
{
	memory_allocator_t* allocator;

	/* handle to the internal GLFWwindow object */
	void* handle;

	/* pointer to API specific GPU buffer object */
	void* api_buffer;
	/* pointer to the memory mapped GPU buffer data of the above object */
	void* api_buffer_data_ptr;

	/* global_set::screen_info
	 * {
	 * 		uvec2 resolution;  	// resolution of the display (in pixels)
	 * 		uvec2 dpi; 			// dpi of the display (in pixels per inch)
	 * 		uvec2 size; 		// window size (in pixels)
	 * 		mat4 matrix; 		// matrix to project onto the window/screen
	 * } */
	struct_descriptor_t 	screen_info_struct;
	struct_field_handle_t 		resolution_field;
	struct_field_handle_t 		dpi_field;
	struct_field_handle_t		size_field;
	struct_field_handle_t 		matrix_field;

	/* width of the window (in pixels) */
	u32 width;
	/* height of the window (in pixels) */
	u32 height;

	/* event triggered whenever the window resizes */
	event_t* on_resize_event;
} render_window_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
RENDERER_API bool render_window_should_close(render_window_t* window);
RENDERER_API void render_window_poll_events(render_window_t* window);
RENDERER_API void render_window_destroy(render_window_t* window);

RENDERER_API void render_window_initialize_api_buffer(render_window_t* window, void* driver);

// getters
RENDERER_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height);

//API specific extensions
RENDERER_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface);

END_CPP_COMPATIBLE
