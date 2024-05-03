/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_material_library.h is a part of VulkanRenderer

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
#include <renderer/string.h>

#include <renderer/internal/vulkan/vulkan_handles.h> // vulkan_material_handle_t, vulkan_shader_handle_t

typedef struct vulkan_shader_library_t vulkan_shader_library_t;
typedef struct vulkan_material_t vulkan_material_t;

typedef struct vulkan_material_library_slot_t
{
	string_t name;
	vulkan_material_t* material;
	vulkan_material_handle_t handle;
} vulkan_material_library_slot_t;

typedef struct vulkan_material_library_t
{
	vulkan_renderer_t* renderer;
	vulkan_shader_library_t* shader_library;
	BUFFER relocation_table;
	BUFFER materials;
} vulkan_material_library_t;

/* constructors & destructors */
RENDERER_API vulkan_material_library_t* vulkan_material_library_new(memory_allocator_t* allocator);
RENDERER_API vulkan_material_library_t* vulkan_material_library_create(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library);
RENDERER_API void vulkan_material_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library, vulkan_material_library_t OUT library);
RENDERER_API void vulkan_material_library_destroy(vulkan_material_library_t* library);
RENDERER_API void vulkan_material_library_release_resources(vulkan_material_library_t* library);

/* logic functions */
RENDERER_API vulkan_material_handle_t vulkan_material_library_create_material(vulkan_material_library_t* library, const char* shader_name, const char* material_name);
RENDERER_API vulkan_material_handle_t vulkan_material_library_create_materialH(vulkan_material_library_t* library, vulkan_shader_handle_t handle, const char* material_name);
RENDERER_API vulkan_material_handle_t vulkan_material_library_load_material(vulkan_material_library_t* library, const char* file_path);
RENDERER_API bool vulkan_material_library_destroy_material(vulkan_material_library_t* library, const char* material_name);
RENDERER_API bool vulkan_material_library_destroy_materialH(vulkan_material_library_t* library, vulkan_material_handle_t handle);
RENDERER_API BUFFER* vulkan_material_library_serialize(vulkan_material_library_t* library);
RENDERER_API vulkan_material_library_t* vulkan_material_library_deserialize(void* bytes, u64 length);
RENDERER_API bool vulkan_material_library_deserialize_no_alloc(void* bytes, u64 length, vulkan_material_library_t* out_library);

/* getters */
RENDERER_API vulkan_material_handle_t vulkan_material_library_get_handle(vulkan_material_library_t* library, const char* material_name);
RENDERER_API vulkan_material_t* vulkan_material_library_get(vulkan_material_library_t* library, const char* material_name);
RENDERER_API vulkan_material_t* vulkan_material_library_getH(vulkan_material_library_t* library, vulkan_material_handle_t handle);
