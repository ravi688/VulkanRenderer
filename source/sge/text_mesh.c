/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: text_mesh.c is a part of VulkanRenderer

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

#include <sge/text_mesh.h>
#include <sge/internal/vulkan/vulkan_text_mesh.h>
#include <sge/sge.h>

// constructors and destructors
SGE_API text_mesh_t* text_mesh_new(memory_allocator_t* allocator)
{
    return vulkan_text_mesh_new(allocator);
}

SGE_API text_mesh_t* text_mesh_create(renderer_t* renderer, glyph_mesh_pool_t* pool)
{
    return vulkan_text_mesh_create(VULKAN_RENDERER(renderer), pool);
}

SGE_API void text_mesh_create_no_alloc(renderer_t* renderer, glyph_mesh_pool_t* pool, text_mesh_t OUT text)
{
    vulkan_text_mesh_create_no_alloc(VULKAN_RENDERER(renderer), pool, text);
}

SGE_API void text_mesh_destroy(text_mesh_t* text)
{
    vulkan_text_mesh_destroy(text);
}

SGE_API void text_mesh_release_resources(text_mesh_t* text)
{
    vulkan_text_mesh_release_resources(text);
}


// logic functions
SGE_API void text_mesh_draw(text_mesh_t* text)
{
    vulkan_text_mesh_draw(text);
}

// constructors and destructors
SGE_API text_mesh_string_handle_t text_mesh_string_create(text_mesh_t* text_mesh)
{
    return vulkan_text_mesh_string_create(text_mesh);
}

SGE_API void text_mesh_string_destroyH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    vulkan_text_mesh_string_destroyH(text_mesh, handle);
}


// setters
SGE_API void text_mesh_set_point_size(text_mesh_t* text, u32 point_size)
{
    vulkan_text_mesh_set_point_size(text, point_size);
}

SGE_API void text_mesh_set_material(text_mesh_t* text, material_t* material)
{
    vulkan_text_mesh_set_material(text, material);
}

SGE_API void text_mesh_set_render_space_type(text_mesh_t* text, text_mesh_render_space_type_t space_type)
{
    vulkan_text_mesh_set_render_space_type(text, space_type);
}

SGE_API void text_mesh_set_render_surface_type(text_mesh_t* text, text_mesh_render_surface_type_t surface_type)
{
    vulkan_text_mesh_set_render_surface_type(text, surface_type);
}

SGE_API void text_mesh_string_setH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, const char* string)
{
    vulkan_text_mesh_string_setH(text_mesh, handle, string);
}

SGE_API void text_mesh_string_set_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle, u32 point_size)
{
    vulkan_text_mesh_string_set_point_sizeH(text, handle, point_size);
}

SGE_API void text_mesh_string_set_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, mat4_t transform)
{
    vulkan_text_mesh_string_set_transformH(text_mesh, handle, transform);
}


// getters
SGE_API u32 text_mesh_get_point_size(text_mesh_t* text)
{
    return vulkan_text_mesh_get_point_size(text);
}

SGE_API const char* text_mesh_string_getH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_getH(text_mesh, handle);
}

SGE_API u32 text_mesh_string_get_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_get_point_sizeH(text, handle);
}

SGE_API mat4_t text_mesh_string_get_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
    return vulkan_text_mesh_string_get_transformH(text_mesh, handle);
}
