
#pragma once

#include <renderer/mesh.h>		// mesh_t
#include <renderer/font.h>		// font_t

typedef struct glyph_mesh_pool_t glyph_mesh_pool_t;

// contructors and destructors
glyph_mesh_pool_t* glyph_mesh_pool_new();
glyph_mesh_pool_t* glyph_mesh_pool_create(renderer_t* renderer, font_t* font);
void glyph_mesh_pool_destroy(glyph_mesh_pool_t* pool);
void glyph_mesh_pool_release_resources(glyph_mesh_pool_t* pool);

// getters
font_t* glyph_mesh_pool_get_font(glyph_mesh_pool_t* pool);
mesh_t* glyph_mesh_pool_get_mesh(glyph_mesh_pool_t* pool, u16 glyph);
