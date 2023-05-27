/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glyph_mesh_pool.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

#include <renderer/mesh.h>		// mesh_t
#include <renderer/font.h>		// font_t

typedef struct glyph_mesh_pool_t glyph_mesh_pool_t;

BEGIN_CPP_COMPATIBLE

// contructors and destructors
RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_new(memory_allocator_t* allocator);
RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_create(renderer_t* renderer, font_t* font);
RENDERER_API void glyph_mesh_pool_destroy(glyph_mesh_pool_t* pool);
RENDERER_API void glyph_mesh_pool_release_resources(glyph_mesh_pool_t* pool);

// getters
RENDERER_API font_t* glyph_mesh_pool_get_font(glyph_mesh_pool_t* pool);
RENDERER_API mesh_t* glyph_mesh_pool_get_mesh(glyph_mesh_pool_t* pool, u16 glyph);


END_CPP_COMPATIBLE
