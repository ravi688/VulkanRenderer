/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_glyph_mesh_pool.h is a part of VulkanRenderer

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
#include <renderer/dictionary.h> // dictionary_t
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct mesh3d_t mesh3d_t;
typedef struct font_t font_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef	struct vulkan_glyph_mesh_t
{
	vulkan_mesh_t* mesh;
	mesh3d_t* mesh3d;
} vulkan_glyph_mesh_t;

typedef	struct vulkan_glyph_mesh_pool_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	font_t* font;
	dictionary_t /*(u16, vulkan_glyph_mesh_t*)*/ glyph_meshes;
} vulkan_glyph_mesh_pool_t;

#define VULKAN_GLYPH_MESH_POOL(ptr) VULKAN_OBJECT_UP_CAST(vulkan_glyph_mesh_pool_t*, VULKAN_OBJECT_TYPE_GLYPH_MESH_POOL, ptr)
#define VULKAN_GLYPH_MESH_POOL_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_glyph_mesh_pool_t*, VULKAN_OBJECT_TYPE_GLYPH_MESH_POOL, ptr)


BEGIN_CPP_COMPATIBLE

// contructors and destructors
SGE_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_new(memory_allocator_t* allocator);
SGE_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_create(vulkan_renderer_t* renderer, font_t* font);
SGE_API void vulkan_glyph_mesh_pool_destroy(vulkan_glyph_mesh_pool_t* pool);
SGE_API void vulkan_glyph_mesh_pool_release_resources(vulkan_glyph_mesh_pool_t* pool);

// getters
SGE_API font_t* vulkan_glyph_mesh_pool_get_font(vulkan_glyph_mesh_pool_t* pool);
SGE_API vulkan_mesh_t* vulkan_glyph_mesh_pool_get_mesh(vulkan_glyph_mesh_pool_t* pool, u16 glyph);

END_CPP_COMPATIBLE
