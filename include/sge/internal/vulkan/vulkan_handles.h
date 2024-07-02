/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_handles.h is a part of VulkanRenderer

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

#include <bufferlib/buffer.h>

typedef buf_ucount_t vulkan_material_handle_t;
#define VULKAN_MATERIAL_HANDLE_INVALID (~0ULL)

typedef buf_ucount_t vulkan_render_pass_handle_t;
#define VULKAN_RENDER_PASS_HANDLE_INVALID (~0ULL)

typedef buf_ucount_t vulkan_shader_handle_t;
#define VULKAN_SHADER_HANDLE_INVALID (~0ULL)

typedef struct vulkan_render_object_t vulkan_render_object_t;
typedef struct vulkan_render_queue_t vulkan_render_queue_t;

typedef struct vulkan_render_scene_object_handle_t
{
	vulkan_render_object_t* object;
	vulkan_render_queue_t* queue;
} vulkan_render_scene_object_handle_t;

#define VULKAN_RENDER_SCENE_OBJECT_HANDLE_INVALID (vulkan_render_scene_object_handle_t) { NULL, NULL }

#include <sge/library.h>
typedef library_slot_handle_t vulkan_camera_handle_t;
#define VULKAN_CAMERA_HANDLE_INVALID LIBRARY_SLOT_HANDLE_INVALID
