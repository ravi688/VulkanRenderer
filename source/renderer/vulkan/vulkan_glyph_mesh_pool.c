/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glyph_mesh_pool.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_glyph_mesh_pool.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/mesh3d.h>	// mesh3d_t
#include <renderer/mesh.h>		// for mesh_create_no_alloc
#include <renderer/font.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h> 	// assert
#include <ctype.h> 				// isspace

// constructors and destructors
RENDERER_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_new(memory_allocator_t* allocator)
{
	vulkan_glyph_mesh_pool_t* pool = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GLYPH_MESH_POOL, vulkan_glyph_mesh_pool_t);
	memzero(pool, vulkan_glyph_mesh_pool_t);
	return pool;
}

RENDERER_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_create(vulkan_renderer_t* renderer, font_t* font)
{
	_debug_assert__(font != NULL);
	vulkan_glyph_mesh_pool_t* pool = vulkan_glyph_mesh_pool_new(renderer->allocator);
	pool->glyph_meshes = dictionary_create(u16, vulkan_glyph_mesh_t, 0, dictionary_key_comparer_u16);
	pool->renderer = renderer;
	pool->font = font;
	return pool;
}

RENDERER_API void vulkan_glyph_mesh_pool_destroy(vulkan_glyph_mesh_pool_t* pool)
{
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
	{
        vulkan_glyph_mesh_t* ptr = dictionary_get_value_ptr_at(&pool->glyph_meshes, i);
		mesh_destroy(ptr->mesh);
		mesh3d_destroy(ptr->mesh3d);
	}
}

RENDERER_API void vulkan_glyph_mesh_pool_release_resources(vulkan_glyph_mesh_pool_t* pool)
{
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
		vulkan_mesh_release_resources(CAST_TO(vulkan_glyph_mesh_t*, dictionary_get_value_ptr_at(&pool->glyph_meshes, i))->mesh);

	dictionary_clear(&pool->glyph_meshes);
	memory_allocator_dealloc(pool->renderer->allocator, pool);
}

// getters

RENDERER_API font_t* vulkan_glyph_mesh_pool_get_font(vulkan_glyph_mesh_pool_t* pool)
{
	return pool->font;
}

RENDERER_API vulkan_mesh_t* vulkan_glyph_mesh_pool_get_mesh(vulkan_glyph_mesh_pool_t* pool, u16 glyph)
{
	if(isspace(glyph)) return NULL;

	vulkan_mesh_t* mesh;
	if(!dictionary_contains(&pool->glyph_meshes, &glyph))
	{
		mesh3d_t* mesh3d = mesh3d_new(pool->renderer->allocator);
		font_get_glyph_mesh(pool->font, glyph, FONT_GLYPH_MESH_QUALITY_LOW, mesh3d);

		mesh = vulkan_mesh_new(pool->renderer->allocator);
		/* This might not work as intended because the API backend will be compiled separately in future
		 * and will be decoupled from the renderer interface.
		 * So, if the renderer interface chooses an opengl implementation of this function then calling
		 * this function from the vulkan API backend will result in data corruption.
		 * However, to get the intended results one needs to link compatible API backend with the rendering interface. */
		mesh_create_no_alloc(pool->renderer->renderer, mesh3d, STATIC_CAST(vulkan_mesh_t*, mesh));


		vulkan_glyph_mesh_t glyph_mesh = { .mesh = mesh, .mesh3d = mesh3d };
		dictionary_add(&pool->glyph_meshes, &glyph, &glyph_mesh);
	}
	else
	{
		mesh = CAST_TO(vulkan_glyph_mesh_t*, dictionary_get_value_ptr(&pool->glyph_meshes, &glyph))->mesh;
	}
	return mesh;
}
