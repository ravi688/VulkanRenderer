/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: renderer.h is a part of VulkanRenderer

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

/* API level object selection */
#ifdef SGE_VULKAN_DRIVER
#	include <sge/internal/vulkan/vulkan_renderer.h>
	typedef vulkan_renderer_t driver_t;
	typedef vulkan_render_pass_pool_t render_pass_pool_t;
	typedef vulkan_shader_library_t shader_library_t;
	typedef vulkan_material_library_t material_library_t;
	typedef vulkan_camera_system_t camera_system_t;
#	define RENDERER(vk_ptr) ((vk_ptr)->renderer)
#	define VULKAN_RENDERER(ptr) ((ptr)->vulkan_handle)
#elif SGE_OPENGL_DRIVER
#	include <sge/internal/opengl/opengl_renderer.h>
	typedef opengl_renderer_t driver_t;
	typedef opengl_render_pass_pool_t render_pass_pool_t;
	typedef opengl_shader_library_t shader_library_t;
	typedef opengl_material_library_t material_library_t;
	typedef opengl_camera_system_t camera_system_t;
#	define RENDERER(gl_ptr) ((gl_ptr)->renderer)
#	define OPENGL_RENDERER(ptr) ((ptr)->opengl_handle)
#elif SGE_DIRECTX_DRIVER
#	include <sge/internal/directx/directx_renderer.h>
	typedef directx_renderer_t driver_t;
	typedef directx_render_pass_pool_t render_pass_pool_t;
	typedef directx_shader_library_t shader_library_t;
	typedef directx_material_library_t material_library_t;
	typedef directx_camera_system_t camera_system_t;
#	define RENDERER(dx_ptr) ((dx_ptr)->renderer)
#	define DIRECTX_RENDERER(ptr) ((ptr)->directx_handle)
#elif SGE_METAL_DRIVER
#	include <sge/internal/metal/metal_renderer.h>
	typedef metal_renderer_t driver_t;
	typedef metal_render_pass_pool_t render_pass_pool_t;
	typedef metal_shader_library_t shader_library_t;
	typedef metal_material_library_t material_library_t;
	typedef metal_camera_system_t camera_system_t;
#	define RENDERER(mt_ptr) ((mt_ptr)->renderer)
#	define METAL_RENDERER(ptr) ((ptr)->metal_handle)
#endif

typedef struct render_window_t render_window_t;
typedef struct memory_allocator_t memory_allocator_t;

typedef struct FT_LibraryRec_  *FT_Library;

typedef enum sge_gpu_type_t
{
	SGE_GPU_TYPE_AUTO = 0,
	SGE_GPU_TYPE_INTEGRATED,
	SGE_GPU_TYPE_DISCRETE,
	SGE_GPU_TYPE_CPU
} sge_gpu_type_t;


typedef struct sge_driver_create_info_t
{
	sge_gpu_type_t gpu_type;
	u32 width;
	u32 height;
	const char* title;
	bool full_screen;
	bool resizable;
	bool require_bitmap_text;
	u32 max_point_lights;
	u32 max_spot_lights;
	u32 max_far_lights;
} sge_driver_create_info_t;

typedef struct renderer_t
{
	union
	{
		driver_t*
#ifdef SGE_VULKAN_DRIVER
		vulkan_driver, *vulkan_handle;
#elif SGE_OPENGL_DRIVER
		opengl_driver, *opengl_handle;
#elif SGE_DIRECTX_DRIVER
		directx_driver, directx_handle;
#elif SGE_METAL_DRIVER
		metal_driver, *metal_handle;
#endif
		driver_t* handle;
		driver_t* driver;
	};

	memory_allocator_t* allocator;
	IF_DEBUG(string_builder_t* debug_log_builder;)
	FT_Library ft_library;
	// shader_library_t* shader_library;
	// material_library_t* material_library;
	// render_pass_pool_t* render_pass_pool;
} renderer_t;

BEGIN_CPP_COMPATIBLE

SGE_API renderer_t* renderer_init(memory_allocator_t* allocator, sge_driver_create_info_t* create_info);
SGE_API void renderer_terminate(renderer_t* renderer);
SGE_API void renderer_update(renderer_t* renderer);
SGE_API bool renderer_is_running(renderer_t* renderer);

/*
	description:
		begins command buffer recording
	params:
		renderer: ptr to the renderer_t object
	returns:
		nothing
 */
SGE_API void renderer_begin_frame(renderer_t* renderer);

/*
	description:
		ends command buffer recording
 */
SGE_API void renderer_end_frame(renderer_t* renderer);

SGE_API void renderer_wait_idle(renderer_t* renderer);

/* getters */
SGE_API const char* renderer_get_builtin_file_data(renderer_t* renderer, const char* virtual_file_path, u64 OUT data_size);
SGE_API render_window_t* renderer_get_window(renderer_t* renderer);
SGE_API shader_library_t* renderer_get_shader_library(renderer_t* renderer);
SGE_API material_library_t* renderer_get_material_library(renderer_t* renderer);
SGE_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer);
SGE_API camera_system_t* renderer_get_camera_system(renderer_t* renderer);

END_CPP_COMPATIBLE

#include <sge/include_helper.h>
