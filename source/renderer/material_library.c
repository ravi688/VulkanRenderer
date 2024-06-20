/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: material_library.c is a part of VulkanRenderer

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


#include <renderer/material_library.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>


/* constructors & destructors */
RENDERER_API material_library_t* material_library_new(memory_allocator_t* allocator)
{
	return vulkan_material_library_new(allocator);
}

RENDERER_API material_library_t* material_library_create(renderer_t* renderer, shader_library_t* shader_library)
{
	return vulkan_material_library_create(renderer->vulkan_handle, shader_library);
}

RENDERER_API void material_library_destroy(material_library_t* library)
{
	vulkan_material_library_destroy(library);
}

RENDERER_API void material_library_release_resources(material_library_t* library)
{
	vulkan_material_library_release_resources(library);
}


/* logic functions */

RENDERER_API material_handle_t material_library_create_material(material_library_t* library, const char* shader_name, const char* material_name)
{
	return vulkan_material_library_create_material(library, shader_name, material_name);
}

RENDERER_API material_handle_t material_library_create_materialH(material_library_t* library, shader_handle_t handle, const char* material_name)
{
	return vulkan_material_library_create_materialH(library, handle, material_name);
}

RENDERER_API material_handle_t material_library_load_material(material_library_t* library, const char* file_path)
{
	return vulkan_material_library_load_material(library, file_path);
}

RENDERER_API bool material_library_destroy_material(material_library_t* library, const char* material_name)
{
	return vulkan_material_library_destroy_material(library, material_name);
}

RENDERER_API bool material_library_destroy_materialH(material_library_t* library, material_handle_t handle)
{
	return vulkan_material_library_destroy_materialH(library, handle);
}

RENDERER_API BUFFER* material_library_serialize(material_library_t* library)
{
	return vulkan_material_library_serialize(library);
}

RENDERER_API material_library_t* material_library_deserialize(void* bytes, u64 length)
{
	return vulkan_material_library_deserialize(bytes, length);
}

RENDERER_API bool material_library_deserialize_no_alloc(void* bytes, u64 length, material_library_t* out_library)
{
	return vulkan_material_library_deserialize_no_alloc(bytes, length, out_library);
}


/* getters */

RENDERER_API material_handle_t material_library_get_handle(material_library_t* library, const char* material_name)
{
	return vulkan_material_library_get_handle(library, material_name);
}

RENDERER_API material_t* material_library_get(material_library_t* library, const char* material_name)
{
	return vulkan_material_library_get(library, material_name);
}

RENDERER_API material_t* material_library_getH(material_library_t* library, material_handle_t handle)
{
	return vulkan_material_library_getH(library, handle);
}
