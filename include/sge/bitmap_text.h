/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_text.h is a part of VulkanRenderer

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
	#include <sge/internal/vulkan/vulkan_bitmap_text.h>
	typedef vulkan_bitmap_text_t bitmap_text_t;
	typedef vulkan_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef vulkan_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef vulkan_material_t material_t;
#elif defined(SGE_OPENGL_DRIVER)
	#include <sge/internal/opengl/opengl_bitmap_text.h>
	typedef opengl_bitmap_text_t bitmap_text_t;
	typedef opengl_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef opengl_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef opengl_material_t material_t;
#elif defined(SGE_DIRECTX_DRIVER)
	#include <sge/internal/directx/directx_bitmap_text.h>
	typedef directx_bitmap_text_t bitmap_text_t;
	typedef directx_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef directx_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef directx_material_t material_t;
#elif defined(SGE_METAL_DRIVER)
	#include <sge/internal/metal/metal_bitmap_text.h>
	typedef metal_bitmap_text_t bitmap_text_t;
	typedef metal_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef metal_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef metal_material_t material_t;
#endif

#include <sge/rect.h> // rect2d_t and rect3d_t
#include <hpml/mat4.h> 	// mat4_t
#include <hpml/vec3.h> 	// vec3_t

typedef enum bitmap_text_render_space_type_t
{
	BITMAP_TEXT_RENDER_SPACE_TYPE_2D,
	BITMAP_TEXT_RENDER_SPACE_TYPE_3D
} bitmap_text_render_space_type_t;

typedef enum bitmap_text_render_surface_type_t
{
	BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA,
	BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN
} bitmap_text_render_surface_type_t;

BEGIN_CPP_COMPATIBLE

SGE_API bitmap_text_t* bitmap_text_new(memory_allocator_t* allocator);
SGE_API bitmap_text_t* bitmap_text_create(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture);
SGE_API void bitmap_text_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture, bitmap_text_t OUT text);
SGE_API void bitmap_text_destroy(bitmap_text_t* text);
SGE_API void bitmap_text_release_resources(bitmap_text_t* text);

SGE_API void bitmap_text_draw(bitmap_text_t* text);

SGE_API bitmap_text_string_handle_t bitmap_text_string_create(bitmap_text_t* text);
SGE_API void bitmap_text_string_destroyH(bitmap_text_t* text, bitmap_text_string_handle_t handle);

/* setters */
SGE_API void bitmap_text_set_point_size(bitmap_text_t* text, u32 point_size);
SGE_API void bitmap_text_set_material(bitmap_text_t* text, material_t* material);
SGE_API void bitmap_text_set_render_space_type(bitmap_text_t* text, bitmap_text_render_space_type_t space_type);
SGE_API void bitmap_text_set_render_surface_type(bitmap_text_t* text, bitmap_text_render_surface_type_t surface_type);
SGE_API void bitmap_text_string_setH(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char* string);
SGE_API void bitmap_text_string_set_point_sizeH(bitmap_text_t* text, bitmap_text_string_handle_t handle, u32 point_size);
SGE_API void bitmap_text_string_set_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle, mat4_t transform);

/* getters */
SGE_API u32 bitmap_text_get_point_size(bitmap_text_t* text);
SGE_API const char* bitmap_text_string_getH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
SGE_API u32 bitmap_text_string_get_point_sizeH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
SGE_API mat4_t bitmap_text_string_get_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle);

END_CPP_COMPATIBLE
