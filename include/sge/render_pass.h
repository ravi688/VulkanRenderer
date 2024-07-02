/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_pass.h is a part of VulkanRenderer

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

#ifdef SGE_VULKAN_DRIVER
	#include <sge/internal/vulkan/vulkan_render_pass.h>
	typedef vulkan_render_pass_t render_pass_t;
	typedef vulkan_render_pass_create_info_t render_pass_create_info_t;
	typedef vulkan_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID VULKAN_RENDER_PASS_HANDLE_INVALID

	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;

#elif defined(SGE_OPENGL_DRIVER)
	#include <sge/internal/opengl/opengl_render_pass.h>
	typedef opengl_render_pass_t render_pass_t;
	typedef opengl_render_pass_create_info_t render_pass_create_info_t;
	typedef opengl_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID OPENGL_RENDER_PASS_HANDLE_INVALID

	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;

#elif defined(SGE_DIRECTX_DRIVER)
	#include <sge/internal/directx/directx_render_pass.h>
	typedef directx_render_pass_t render_pass_t;
	typedef directx_render_pass_create_info_t render_pass_create_info_t;
	typedef directx_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID DIRECTX_RENDER_PASS_HANDLE_INVALID

	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;

#elif defined(SGE_METAL_DRIVER)
	#include <sge/internal/metal/metal_render_pass.h>
	typedef metal_render_pass_t render_pass_t;
	typedef metal_render_pass_create_info_t render_pass_create_info_t;
	typedef metal_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID METAL_RENDER_PASS_HANDLE_INVALID

	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;

#endif


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API render_pass_t* render_pass_new(memory_allocator_t* allocator);
SGE_API render_pass_t* render_pass_create(renderer_t* renderer, render_pass_create_info_t* create_info);
SGE_API void render_pass_create_no_alloc(renderer_t* renderer, render_pass_create_info_t* create_info, render_pass_t OUT pass);
SGE_API void render_pass_destroy(render_pass_t* pass);
SGE_API void render_pass_release_resources(render_pass_t* pass);

/* logic functions */
SGE_API void render_pass_set_clear(render_pass_t* pass, color_t color, float depth);
SGE_API void render_pass_begin(render_pass_t* pass, void* api_specific, camera_t* camera);
SGE_API void render_pass_end(render_pass_t* pass);

END_CPP_COMPATIBLE
