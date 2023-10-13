#pragma once

#include <renderer/defines.h>
#include <renderer/dictionary.h> // dictionary_t

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
	vulkan_renderer_t* renderer;
	font_t* font;
	dictionary_t /*(u16, vulkan_glyph_mesh_t*)*/ glyph_meshes;
} vulkan_glyph_mesh_pool_t;


BEGIN_CPP_COMPATIBLE

// contructors and destructors
RENDERER_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_new(memory_allocator_t* allocator);
RENDERER_API vulkan_glyph_mesh_pool_t* vulkan_glyph_mesh_pool_create(vulkan_renderer_t* renderer, font_t* font);
RENDERER_API void vulkan_glyph_mesh_pool_destroy(vulkan_glyph_mesh_pool_t* pool);
RENDERER_API void vulkan_glyph_mesh_pool_release_resources(vulkan_glyph_mesh_pool_t* pool);

// getters
RENDERER_API font_t* vulkan_glyph_mesh_pool_get_font(vulkan_glyph_mesh_pool_t* pool);
RENDERER_API vulkan_mesh_t* vulkan_glyph_mesh_pool_get_mesh(vulkan_glyph_mesh_pool_t* pool, u16 glyph);

END_CPP_COMPATIBLE
