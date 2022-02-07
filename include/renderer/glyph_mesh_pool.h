
#pragma once

#include <renderer/mesh.h>		// mesh_t
#include <renderer/font.h>		// font_t

typedef struct glyph_mesh_pool_t glyph_mesh_pool_t;

BEGIN_CPP_COMPATIBLE

// contructors and destructors
RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_new();
RENDERER_API glyph_mesh_pool_t* glyph_mesh_pool_create(renderer_t* renderer, font_t* font);
RENDERER_API void glyph_mesh_pool_destroy(glyph_mesh_pool_t* pool);
RENDERER_API void glyph_mesh_pool_release_resources(glyph_mesh_pool_t* pool);

// getters
RENDERER_API font_t* glyph_mesh_pool_get_font(glyph_mesh_pool_t* pool);
RENDERER_API mesh_t* glyph_mesh_pool_get_mesh(glyph_mesh_pool_t* pool, u16 glyph);


END_CPP_COMPATIBLE
