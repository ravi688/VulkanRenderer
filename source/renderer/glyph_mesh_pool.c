
#include <renderer/glyph_mesh_pool.h>
#include <renderer/mesh3d.h>	// mesh3d_t
#include <memory_allocator/memory_allocator.h>
#include <dictionary.h>			// dictionary_t
#include <renderer/assert.h> 	// assert
#include <ctype.h> 				// isspace

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(pool)
#else
	static void check_pre_condition(glyph_mesh_pool_t* pool);
#endif /*GLOBAL_DEBUG*/


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
glyph_mesh_pool_t* glyph_mesh_pool_new()
{
	glyph_mesh_pool_t* pool = heap_new(glyph_mesh_pool_t);
	memset(pool, 0, sizeof(glyph_mesh_pool_t));
	return pool;
}

glyph_mesh_pool_t* glyph_mesh_pool_create(renderer_t* renderer, font_t* font)
{
	assert(font != NULL);
	glyph_mesh_pool_t* pool = glyph_mesh_pool_new();
	pool->glyph_meshes = dictionary_create(u16, glyph_mesh_t, 0, dictionary_key_comparer_u16);
	pool->renderer = renderer;
	pool->font = font;
	return pool;
}

void glyph_mesh_pool_destroy(glyph_mesh_pool_t* pool)
{
	check_pre_condition(pool);
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
	{
		glyph_mesh_t* ptr = dictionary_get_value_ptr_at(&pool->glyph_meshes, i);
		mesh_destroy(ptr->mesh, pool->renderer);
		mesh3d_destroy(ptr->mesh3d);
	}
}

void glyph_mesh_pool_release_resources(glyph_mesh_pool_t* pool)
{
	check_pre_condition(pool);
	for(u64 i = 0; i < dictionary_get_count(&pool->glyph_meshes); i++)
	{
		glyph_mesh_t* ptr = dictionary_get_value_ptr_at(&pool->glyph_meshes, i);
		mesh_release_resources(ptr->mesh);
	}
	dictionary_clear(&pool->glyph_meshes);
	heap_free(pool);
}

// getters

font_t* glyph_mesh_pool_get_font(glyph_mesh_pool_t* pool)
{
	check_pre_condition(pool);
	return pool->font;
}

mesh_t* glyph_mesh_pool_get_mesh(glyph_mesh_pool_t* pool, u16 glyph)
{
	check_pre_condition(pool);
	if(isspace(glyph)) return NULL;

	mesh_t* mesh;
	if(!dictionary_contains(&pool->glyph_meshes, &glyph))
	{
		mesh3d_t* mesh3d = mesh3d_new();
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


#ifdef GLOBAL_DEBUG
static void check_pre_condition(glyph_mesh_pool_t* pool)
{
	assert(pool != NULL);
	assert(pool->renderer != NULL);
	assert(pool->font != NULL);
}
#endif /*GLOBAL_DEBUG*/
