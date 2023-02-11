#include <renderer/glyph_mesh_pool.h>
#include <renderer/mesh3d.h>	// mesh3d_t
#include <renderer/renderer.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/dictionary.h> // dictionary_t
#include <renderer/assert.h> 	// assert
#include <ctype.h> 				// isspace

typedef	struct glyph_mesh_t
{
	mesh_t* mesh;
	mesh3d_t* mesh3d;
} glyph_mesh_t;

typedef	struct glyph_mesh_pool_t
{
	renderer_t* renderer;
	font_t* font;
	dictionary_t /*(u16, glyph_mesh_t*)*/ glyph_meshes;
} glyph_mesh_pool_t;

// constructors and destructors
RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_new(memory_allocator_t* allocator)
{
	glyph_mesh_pool_t* pool = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_GLYPH_MESH_POOL, glyph_mesh_pool_t);
	memzero(pool, glyph_mesh_pool_t);
	return pool;
}

RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_create(renderer_t* renderer, font_t* font)
{
	_debug_assert__(font != NULL);
	glyph_mesh_pool_t* pool = glyph_mesh_pool_new(renderer->allocator);
	pool->glyph_meshes = dictionary_create(u16, glyph_mesh_t, 0, dictionary_key_comparer_u16);
	pool->renderer = renderer;
	pool->font = font;
	return pool;
}

RENDERER_API void glyph_mesh_pool_destroy(glyph_mesh_pool_t* pool)
{
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
	{
		glyph_mesh_t* ptr = dictionary_get_value_ptr_at(&pool->glyph_meshes, i);
		mesh_destroy(ptr->mesh);
		mesh3d_destroy(ptr->mesh3d);
	}
}

RENDERER_API void glyph_mesh_pool_release_resources(glyph_mesh_pool_t* pool)
{
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
	{
		glyph_mesh_t* ptr = dictionary_get_value_ptr_at(&pool->glyph_meshes, i);
		mesh_release_resources(ptr->mesh);
	}
	dictionary_clear(&pool->glyph_meshes);
	memory_allocator_dealloc(pool->renderer->allocator, pool);
}

// getters

RENDERER_API font_t* glyph_mesh_pool_get_font(glyph_mesh_pool_t* pool)
{
	return pool->font;
}

RENDERER_API mesh_t* glyph_mesh_pool_get_mesh(glyph_mesh_pool_t* pool, u16 glyph)
{
	if(isspace(glyph)) return NULL;

	mesh_t* mesh;
	if(!dictionary_contains(&pool->glyph_meshes, &glyph))
	{
		mesh3d_t* mesh3d = mesh3d_new(pool->renderer->allocator);
		font_get_glyph_mesh(pool->font, glyph, FONT_GLYPH_MESH_QUALITY_LOW, mesh3d);
		mesh = mesh_create(pool->renderer, mesh3d);
		glyph_mesh_t glyph_mesh = { .mesh = mesh, .mesh3d = mesh3d };
		dictionary_add(&pool->glyph_meshes, &glyph, &glyph_mesh);
	}
	else
	{
		glyph_mesh_t* glyph_mesh = dictionary_get_value_ptr(&pool->glyph_meshes, &glyph);
		mesh = glyph_mesh->mesh;
	}
	return mesh;
}
