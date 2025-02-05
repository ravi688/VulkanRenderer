/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_glyph_pool.h is a part of VulkanRenderer

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
#include <sge/font.h> 	  // font_t, utf32_t
#include <sge/buffer2d.h> 	// buffer2d_t
#include <sge/object.h>
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
	/* font to be used by the bitmap_glyph_pool_t, it can be NULL at this time if the bitmap_glyph_pool_t will be used by vulkan_bitmap_text_t objects only.
	 * that's because, bitmap text objects have their own ptr to font objects and they override this 'font' each time they want to rasterize a glyph or get texcoord. 
	 * if you're using it as standalone then you must call bitmap_glyph_pool_set_font() to set the 'font' to a valid pointer before calling bitmap_glyph_pool_get_texcoord() */
	font_t* font;
	/* optional buffer to be used as a pixel buffer inside the bitmap_glyph_pool_t */
	buffer_t* buffer;
	/* optinal 2d view */
	buffer2d_view_t* view;
} bitmap_glyph_pool_create_info_t;

typedef struct bitmap_glyph_pool_t
{
	__OBJECT__;
	renderer_t* renderer;
	font_t* font;
	buffer2d_t pixels;
} bitmap_glyph_pool_t;

#define BITMAP_GLYPH_POOL(ptr) OBJECT_UP_CAST(bitmap_glyph_pool_t*, OBJECT_TYPE_BITMAP_GLYPH_POOL, ptr)
#define BITMAP_GLYPH_POOL_CONST(ptr) OBJECT_UP_CAST_CONST(const bitmap_glyph_pool_t*, OBJECT_TYPE_BITMAP_GLYPH_POOL, ptr)


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API bitmap_glyph_pool_t* bitmap_glyph_pool_new(memory_allocator_t* allocator);
SGE_API bitmap_glyph_pool_t* bitmap_glyph_pool_create(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info);
SGE_API void bitmap_glyph_pool_create_no_alloc(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info, bitmap_glyph_pool_t OUT pool);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void bitmap_glyph_pool_create_no_alloc_ext(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info, bitmap_glyph_pool_t OUT pool)
{
	OBJECT_INIT(pool, OBJECT_TYPE_BITMAP_GLYPH_POOL, OBJECT_NATIONALITY_EXTERNAL);
	bitmap_glyph_pool_create_no_alloc(renderer, create_info, pool);
}

SGE_API void bitmap_glyph_pool_destroy(bitmap_glyph_pool_t* pool);
SGE_API void bitmap_glyph_pool_release_resources(bitmap_glyph_pool_t* pool);

/* clears the pool for reuse */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void bitmap_glyph_pool_clear(bitmap_glyph_pool_t* pool) { buffer2d_clear(&pool->pixels, NULL); }
/* sets the current font which is used by the subsequent calls to bitmap_glyph_pool_get_texcoord() */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void bitmap_glyph_pool_set_font(bitmap_glyph_pool_t* pool, font_t* font) { pool->font = font; }
/* returns the font used by this bitmap glyph pool */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE font_t* bitmap_glyph_pool_get_font(bitmap_glyph_pool_t* pool) { return pool->font; }
/* checks if rasterized glyph corresponding to the supplied glyph instance id exists or not.
 * if it exists, then it returns true and populates the glyph_texcoord_t object 
 * NOTE: texcoord can be NULL, in that case populate step will not be done. */
SGE_API bool bitmap_glyph_pool_try_get_texcoord(bitmap_glyph_pool_t* pool, glyph_instance_id_t id, glyph_texcoord_t OUT texcoord);
/* rasterizes the glyph if it has graphical representation and not already rasterized.
 * returns true if the glyph has graphical representation and no errors,
 * outputs the texture coordinates of the glyph represented by the unicode encoding 'unicode' and with point size 'point_size' 
 * OUT is_resized : set to true if the 2D view has been resized (eventually the underlying linear buffer) 
 * OUT is_updated : set to true if more contents (or existing content updated) are added into the underlying linear buffer.
 * NOTE: is_updated may set to true even if there is no resize (i.e. is_resizeed equals false) */
SGE_API bool bitmap_glyph_pool_get_texcoord(bitmap_glyph_pool_t* pool, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord, bool OUT is_resized);
/* returns true if this pool has a rasterized graphical representation of a glyph represented by unicode encoding 'unicode' and with point size 'point_size' */
SGE_API bool bitmap_glyph_pool_contains_texcoord(bitmap_glyph_pool_t* pool, pair_t(utf32_t, u32) unicode);

#ifdef GLOBAL_DEBUG
/* creates a file and dumps the colors values in the bitmap to the file (.bmp) */
SGE_API void bitmap_glyph_pool_dump(bitmap_glyph_pool_t* pool, const char* file_path);
/* creates a file and dumps the bounding box color values of the packed rects to the file (.bmp) */
SGE_API void bitmap_glyph_pool_dump_bb(bitmap_glyph_pool_t* pool, const char* file_path);
#endif // GLOBAL_DEBUG

END_CPP_COMPATIBLE
