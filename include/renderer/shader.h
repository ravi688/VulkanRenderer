/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: shader.h is a part of VulkanRenderer

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
#include <bufferlib/buffer.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_shader.h>
	typedef vulkan_shader_t shader_t;
	typedef vulkan_shader_create_info_t shader_create_info_t;
	typedef vulkan_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID VULKAN_SHADER_HANDLE_INVALID
	#define SHADER(ptr) VULKAN_SHADER(ptr)
	#define SHADER_CONST(ptr) VULKAN_SHADER_CONST(ptr)
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_shader.h>
	typedef opengl_shader_t shader_t;
	typedef opengl_shader_create_info_t shader_create_info_t;
	typedef opengl_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID OPENGL_SHADER_HANDLE_INVALID
	#define SHADER(ptr) OPENGL_SHADER(ptr)
	#define SHADER_CONST(ptr) OPENGL_SHADER_CONST(ptr)
#elif RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_shader.h>
	typedef directx_shader_t shader_t;
	typedef directx_shader_create_info_t shader_create_info_t;
	typedef directx_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID DIRECTX_SHADER_HANDLE_INVALID
	#define SHADER(ptr) DIRECTX_SHADER(ptr)
	#define SHADER_CONST(ptr) DIRECTX_SHADER_CONST(ptr)
#elif RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_shader.h>
	typedef metal_shader_t shader_t;
	typedef metal_shader_create_info_t shader_create_info_t;
	typedef metal_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID METAL_SHADER_HANDLE_INVALID
	#define SHADER(ptr) METAL_SHADER(ptr)
	#define SHADER_CONST(ptr) METAL_SHADER_CONST(ptr)
#endif

BEGIN_CPP_COMPATIBLE

RENDERER_API shader_t* shader_create(renderer_t* renderer, shader_create_info_t* create_info);
RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path);
RENDERER_API void shader_destroy(shader_t* shader);
RENDERER_API void shader_release_resources(shader_t* shader);

END_CPP_COMPATIBLE
