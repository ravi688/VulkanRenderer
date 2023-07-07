#pragma once

#include <renderer/defines.h>
#include <renderer/font.h> 	  // font_t, utf32_t
#include <renderer/buffer2d.h> 	// buffer2d_t
#include <hpml/vec2.h>

/* stores texture coordinates of a glyph */
typedef struct glyph_texcoord_t
{
	/* top left texcoord of the glyph */
	vec2_t tltc;
	/* top right texcoord of the glyph */
	vec2_t trtc;
	/* bottom right texcoord of the glyph */
	vec2_t brtc;
	/* bottom left texcoord of the glyph */
	vec2_t bltc;
} glyph_texcoord_t;

typedef struct bitmap_glyph_pool_create_info_t
{
	/* size of the bitmap (in pixels) */
	iextent2d_t size;
	/* font to be used by the bitmap_glyph_pool_t */
	font_t* font;
	/* optional buffer to be used as a pixel buffer inside the bitmap_glyph_pool_t */
	buffer_t* buffer;
} bitmap_glyph_pool_create_info_t;

typedef struct bitmap_glyph_pool_t
{
	renderer_t* renderer;
	font_t* font;
	buffer2d_t pixels;
} bitmap_glyph_pool_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API bitmap_glyph_pool_t* bitmap_glyph_pool_new(memory_allocator_t* allocator);
RENDERER_API bitmap_glyph_pool_t* bitmap_glyph_pool_create(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info);
RENDERER_API void bitmap_glyph_pool_create_no_alloc(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info, bitmap_glyph_pool_t OUT pool);
RENDERER_API void bitmap_glyph_pool_destroy(bitmap_glyph_pool_t* pool);
RENDERER_API void bitmap_glyph_pool_release_resources(bitmap_glyph_pool_t* pool);

/* clears the pool for reuse */
static INLINE_IF_RELEASE_MODE void bitmap_glyph_pool_clear(bitmap_glyph_pool_t* pool) { buffer2d_clear(&pool->pixels, NULL); }
/* returns the font used by this bitmap glyph pool */
static INLINE_IF_RELEASE_MODE font_t* bitmap_glyph_pool_get_font(bitmap_glyph_pool_t* pool) { return pool->font; }
/* returns the texture coordinates of the glyph represented by the unicode encoding 'unicode' */
RENDERER_API bool bitmap_glyph_pool_get_texcoord(bitmap_glyph_pool_t* pool, utf32_t unicode, glyph_texcoord_t OUT texcoord);

#ifdef GLOBAL_DEBUG
/* creates a file and dumps the colors values in the bitmap to the file (.bmp) */
RENDERER_API void bitmap_glyph_pool_dump(bitmap_glyph_pool_t* pool, const char* file_path);
/* creates a file and dumps the bounding box color values of the packed rects to the file (.bmp) */
RENDERER_API void bitmap_glyph_pool_dump_bb(bitmap_glyph_pool_t* pool, const char* file_path);
#endif // GLOBAL_DEBUG

END_CPP_COMPATIBLE
