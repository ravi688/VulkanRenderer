/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_library.h is a part of VulkanRenderer

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
#include <renderer/string.h>
#include <bufferlib/buffer.h>

#include <renderer/internal/vulkan/vulkan_handles.h> 		// vulkan_shader_handle_t

typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_shader_create_info_t vulkan_shader_create_info_t;
typedef struct vulkan_shader_load_info_t vulkan_shader_load_info_t;

typedef struct vulkan_shader_library_slot_t
{
	string_t name;
	vulkan_shader_t* shader;
	vulkan_shader_handle_t handle;
} vulkan_shader_library_slot_t;

typedef struct vulkan_shader_library_t
{
	vulkan_renderer_t* renderer;
	BUFFER relocation_table; 	// list of buf_ucount_t
	BUFFER shaders; 			// list of vulkan_shader_library_slot_t
} vulkan_shader_library_t;


/* constructors & destructors */
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_new(memory_allocator_t* allocator);
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_shader_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t OUT library);
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_load_folder(vulkan_renderer_t* renderer, const char* folder_path);
RENDERER_API void vulkan_shader_library_destroy(vulkan_shader_library_t* library);
RENDERER_API void vulkan_shader_library_release_resources(vulkan_shader_library_t* library);

/* logic functions */

RENDERER_API vulkan_shader_handle_t vulkan_shader_library_create_shader(vulkan_shader_library_t* library, vulkan_shader_create_info_t* create_info, const char* shader_name);
RENDERER_API vulkan_shader_handle_t vulkan_shader_library_load_shader(vulkan_shader_library_t* library, vulkan_shader_load_info_t* load_info, const char* shader_name);
RENDERER_API bool vulkan_shader_library_destroy_shader(vulkan_shader_library_t* library, const char* shader_name);
RENDERER_API bool vulkan_shader_library_destroy_shaderH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);

/* getters */
RENDERER_API vulkan_shader_handle_t vulkan_shader_library_get_handle(vulkan_shader_library_t* library, const char* shader_name);
RENDERER_API const char* vulkan_shader_library_get_nameH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);
RENDERER_API vulkan_shader_t* vulkan_shader_library_getH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);
RENDERER_API vulkan_shader_t* vulkan_shader_library_get(vulkan_shader_library_t* library, const char* shader_name);
