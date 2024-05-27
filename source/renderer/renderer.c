/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: renderer.c is a part of VulkanRenderer

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


#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/alloc.h>

#include <renderer/shader_library.h>
#include <renderer/material_library.h>
#include <renderer/render_pass_pool.h>
#include <renderer/string_builder.h>

#include <freetype/freetype.h>

RENDERER_API renderer_t* renderer_init(memory_allocator_t* allocator, renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	renderer_t* renderer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDERER, renderer_t);
	memzero(renderer, renderer_t);

	renderer->allocator = allocator;
	IF_DEBUG( renderer->debug_log_builder = string_builder_create(allocator, 2048) );

	/* create and initialize the free type library */
	AUTO error = FT_Init_FreeType(&renderer->ft_library);
	if(error != 0)
		debug_log_error("Failed to initialize free type library");

	// create the renderer
	vulkan_renderer_create_info_t create_info = 
	{
		.renderer = renderer,
		.prefered_gpu_type = CAST_TO(vulkan_renderer_gpu_type_t, gpu_type), 
		.width = width, 
		.height = height, 
		.title = title, 
		.full_screen = full_screen, 
		.resizable = resizable
	};
	renderer->vulkan_handle = vulkan_renderer_create(&create_info);

	return renderer;
}

RENDERER_API void renderer_terminate(renderer_t* renderer)
{
	// terminate the renderer
	vulkan_renderer_destroy(renderer->vulkan_handle);

	/* destroy free type library context */
	FT_Done_FreeType(renderer->ft_library);

	IF_DEBUG( string_builder_destroy(renderer->debug_log_builder) );
	memory_allocator_dealloc(renderer->allocator, renderer);
}

RENDERER_API void renderer_update(renderer_t* renderer)
{
	vulkan_renderer_update(renderer->vulkan_handle);
}

RENDERER_API bool renderer_is_running(renderer_t* renderer)
{
	return vulkan_renderer_is_running(renderer->vulkan_handle);
}


RENDERER_API void renderer_begin_frame(renderer_t* renderer)
{
	vulkan_renderer_begin_frame(renderer->vulkan_handle);
}

RENDERER_API void renderer_end_frame(renderer_t* renderer)
{
	vulkan_renderer_end_frame(renderer->vulkan_handle);
}

RENDERER_API void renderer_wait_idle(renderer_t* renderer)
{
	vulkan_renderer_wait_idle(renderer->vulkan_handle);
}

/* getters */
RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer)
{
	return vulkan_renderer_get_window(renderer->vulkan_handle);
}

RENDERER_API shader_library_t* renderer_get_shader_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->shader_library;
}

RENDERER_API material_library_t* renderer_get_material_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->material_library;
}

RENDERER_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer)
{
	return renderer->vulkan_handle->render_pass_pool;
}

RENDERER_API camera_system_t* renderer_get_camera_system(renderer_t* renderer)
{
	return renderer->vulkan_handle->camera_system;
}
