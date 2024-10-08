/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: text_mesh.h is a part of VulkanRenderer

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

#if   defined(SGE_VULKAN_DRIVER)
	// we need to include this header here because we don't know the implementation of vulkan_text_mesh_string_handle_t
	#include <sge/internal/vulkan/vulkan_text_mesh.h>
	typedef vulkan_text_mesh_t text_mesh_t;
	typedef vulkan_text_mesh_string_handle_t text_mesh_string_handle_t;
    typedef vulkan_material_t material_t;
	typedef vulkan_glyph_mesh_pool_t glyph_mesh_pool_t;
#elif defined(SGE_OPENGL_DRIVER)
	#include <sge/internal/opengl/opengl_text_mesh.h>
	typedef opengl_text_mesh_t text_mesh_t;
	typedef opengl_text_mesh_string_handle_t text_mesh_string_handle_t;
    typedef opengl_material_t material_t;
	typedef opengl_glyph_mesh_pool_t glyph_mesh_pool_t;
#elif defined(SGE_DIRECTX_DRIVER)
	#include <sge/internal/directx/directx_text_mesh.h>
	typedef directx_text_mesh_t text_mesh_t;
	typedef directx_text_mesh_string_handle_t text_mesh_string_handle_t;
    typedef directx_material_t material_t;
	typedef directx_glyph_mesh_pool_t glyph_mesh_pool_t;
#elif defined(SGE_METAL_DRIVER)
	#include <sge/internal/metal/metal_text_mesh.h>
	typedef metal_text_mesh_t text_mesh_t;
	typedef metal_text_mesh_string_handle_t text_mesh_string_handle_t;
    typedef metal_material_t material_t;
	typedef metal_glyph_mesh_pool_t glyph_mesh_pool_t;
#endif

typedef enum text_mesh_render_space_type_t
{
	TEXT_MESH_RENDER_SPACE_TYPE_2D,
	TEXT_MESH_RENDER_SPACE_TYPE_3D
} text_mesh_render_space_type_t;

typedef enum text_mesh_render_surface_type_t
{
	TEXT_MESH_RENDER_SURFACE_TYPE_CAMERA,
	TEXT_MESH_RENDER_SURFACE_TYPE_SCREEN
} text_mesh_render_surface_type_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API text_mesh_t* text_mesh_new(memory_allocator_t* allocator);
SGE_API text_mesh_t* text_mesh_create(renderer_t* renderer, glyph_mesh_pool_t* pool);
SGE_API void text_mesh_create_no_alloc(renderer_t* renderer, glyph_mesh_pool_t* pool, text_mesh_t OUT text);
SGE_API void text_mesh_destroy(text_mesh_t* text);
SGE_API void text_mesh_release_resources(text_mesh_t* text);

// logic functions
SGE_API void text_mesh_draw(text_mesh_t* text);

// constructors and destructors
SGE_API text_mesh_string_handle_t text_mesh_string_create(text_mesh_t* text_mesh);
SGE_API void text_mesh_string_destroyH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);

// setters
SGE_API void text_mesh_set_point_size(text_mesh_t* text, u32 point_size);
SGE_API void text_mesh_set_material(text_mesh_t* text, material_t* material);
SGE_API void text_mesh_set_render_space_type(text_mesh_t* text, text_mesh_render_space_type_t space_type);
SGE_API void text_mesh_set_render_surface_type(text_mesh_t* text, text_mesh_render_surface_type_t surface_type);
SGE_API void text_mesh_string_setH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, const char* string);
SGE_API void text_mesh_string_set_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle, u32 point_size);
SGE_API void text_mesh_string_set_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, mat4_t transform);

// getters
SGE_API u32 text_mesh_get_point_size(text_mesh_t* text);
SGE_API const char* text_mesh_string_getH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
SGE_API u32 text_mesh_string_get_point_sizeH(text_mesh_t* text, text_mesh_string_handle_t handle);
SGE_API mat4_t text_mesh_string_get_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);

END_CPP_COMPATIBLE
