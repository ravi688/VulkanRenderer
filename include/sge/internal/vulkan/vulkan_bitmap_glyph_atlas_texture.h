/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_bitmap_glyph_atlas_texture.h is a part of VulkanRenderer

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
#include <sge/internal/vulkan/vulkan_host_buffered_texture.h>
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <sge/bitmap_glyph_pool.h>
#include <hpml/vec2.h> /* vec2_t */
#include <sge/dictionary.h>
#include <glslcommon/glsl_types.h>

typedef struct vulkan_bitmap_glyph_atlas_texture_create_info_t
{
	/* initial width of the texture */
	u32 width;
	/* initial height of the texture */
	u32 height;
	/* font used to rasterize glyphs */
	font_t* font;
} vulkan_bitmap_glyph_atlas_texture_create_info_t;

/* element of Glyph Texture Coordinate (GTC) buffer */
typedef struct vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t
{
	/* per_instance [BTM_TXT_TLTC_BND, BTM_TXT_TLTC_LOC, BTM_TXT_TLTC_COMP] in vec2 tltc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t tltc;
	/* per_instance [BTM_TXT_TRTC_BND, BTM_TXT_TRTC_LOC, BTM_TXT_TRTC_COMP] in vec2 trtc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t trtc;
	/* per_instance [BTM_TXT_BRTC_BND, BTM_TXT_BRTC_LOC, BTM_TXT_BRTC_COMP] in vec2 brtc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t brtc;
	/* per_instance [BTM_TXT_BLTC_BND, BTM_TXT_BLTC_LOC, BTM_TXT_BLTC_COMP] in vec2 bltc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t bltc;
} vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t ALIGN_AS(U32_NEXT_MULTIPLE(GLSL_STD140_VEC2_ALIGN, 16));

/* deterministic and minimum (portable) size of vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t
 * the C compiler may add extra padding resulting in non-portable code or undefined behaviour */
#define SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T (4 * COM_GET_STRIDE_IN_ARRAY(8, GLSL_STD140_VEC2_ALIGN))
/* deterministic (portable) size of vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t in a homogeneous array, i.e. stride to the next element.
 * same as reason as above */
#define STRIDE_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T, ALIGN_OF(vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t))

typedef struct event_t event_t;

/* Glyph Texture Coordinate (GTC) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_glyph_texcoord_buffer_t;
typedef dictionary_t glyph_texcoord_index_table_t;

typedef struct vulkan_bitmap_glyph_atlas_texture_t
{
	__VULKAN_OBJECT__;
	/* holds the actual host side pixel buffer and device side pixel buffer (VkImage) */
	DERIVE_FROM(vulkan_host_buffered_texture_t);
	vulkan_renderer_t* renderer;
	/* facilitates pooling of rasterized glyphs, works on the host side pixel buffer */
	bitmap_glyph_pool_t pool;
	/* GTC buffer */
	vulkan_bitmap_glyph_texcoord_buffer_t glyph_texcoord_buffer;
	glyph_texcoord_index_table_t glyph_texcoord_index_table;
	/* this event will be published (just after) whenever BGA texture's device side objects are resized/recreated */
	event_t* on_resize_event;
	/* this event will be published (just after) whenever GTC buffer's device side objects are resized/recreated */
	event_t* on_gtc_buffer_resize_event;
} vulkan_bitmap_glyph_atlas_texture_t;

#define VULKAN_BITMAP_GLYPH_ATLAS_TEXTURE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_bitmap_glyph_atlas_texture_t*, VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE, ptr)
#define VULKAN_BITMAP_GLYPH_ATLAS_TEXTURE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_bitmap_glyph_atlas_texture_t*, VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_BITMAP_GLYPH_ATLAS_TEXTURE_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_bitmap_glyph_atlas_texture_t*, VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE, ptr)
#define VULKAN_BITMAP_GLYPH_ATLAS_TEXTURE_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_bitmap_glyph_atlas_texture_t*, VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator);
SGE_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_create(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info);
SGE_API void vulkan_bitmap_glyph_atlas_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info, vulkan_bitmap_glyph_atlas_texture_t OUT texture);
SGE_API void vulkan_bitmap_glyph_atlas_texture_destroy(vulkan_bitmap_glyph_atlas_texture_t* texture);
SGE_API void vulkan_bitmap_glyph_atlas_texture_release_resources(vulkan_bitmap_glyph_atlas_texture_t* texture);

/* rasterizes the glyph if not already and returns index to it in the BGA texture's texcoord table 
 * this function may mark the underlying vulkan API objects as dirty (out of date) */
SGE_API u32 vulkan_bitmap_glyph_atlas_texture_get_or_create_glyph(vulkan_bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode);
/* returns actual texture coordinates from the index returned by vulkan_bitmap_glyph_atlas_texture_get_or_create_glyph 
 * this function doesn't mark any underlying vulkan API objects as dirty */
SGE_API vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t vulkan_bitmap_glyph_atlas_texture_get_texcoord_from_index(vulkan_bitmap_glyph_atlas_texture_t* texture, u32 index);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE event_t* vulkan_bitmap_glyph_atlas_texture_get_on_resize_event(vulkan_bitmap_glyph_atlas_texture_t*  texture)
{
	return texture->on_resize_event;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE event_t* vulkan_bitmap_glyph_atlas_texture_get_on_gtc_buffer_resize_event(vulkan_bitmap_glyph_atlas_texture_t*  texture)
{
	return texture->on_gtc_buffer_resize_event;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE iextent2d_t vulkan_bitmap_glyph_atlas_texture_get_size(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return buffer2d_view_get_size(vulkan_host_buffered_texture_get_view_readonly(BASE(texture)));
}
/* returns pointer to the bitmap_glyph_pool_t object */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bitmap_glyph_pool_t* vulkan_bitmap_glyph_atlas_texture_get_pool(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return &texture->pool;
}
/* see: bitmap_glyph_pool_set_font for what it does. */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_bitmap_glyph_atlas_texture_set_font(vulkan_bitmap_glyph_atlas_texture_t* texture, font_t* font)
{
	bitmap_glyph_pool_set_font(&texture->pool, font);
}
/* returns the font used by this BGA texture */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE font_t* vulkan_bitmap_glyph_atlas_texture_get_font(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return bitmap_glyph_pool_get_font(&texture->pool);
}
/* returns a pointer to the underlying vulkan_texture_t object */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_texture_t* vulkan_bitmap_glyph_atlas_texture_get_texture(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_host_buffered_texture_get_texture(BASE(texture));
}
/* flushes the host side font bitmap to the GPU side memory (it might also destroy and create VkDeviceMemory)
 * returns true if either the internal buffers, images and image views has been updated or recreated */
SGE_API bool vulkan_bitmap_glyph_atlas_texture_commit(vulkan_bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized);
/* quality: quality of the rasterized glyph, ranges from 0 to 255 inclusive
 * unicode: glyph's unicode value to rasterize
 * texcoords: the texture coordinates (list of 4 vec2(s)), filled by this function if the glyph has graphical representation
 * returns: true if the glyph has graphical representation and there are no errors */
SGE_API bool vulkan_bitmap_glyph_atlas_texture_get_texcoord(vulkan_bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer_t* vulkan_bitmap_glyph_atlas_texture_get_gtc_host_buffer_readonly(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_host_buffered_buffer_get_host_buffer_readonly(&texture->glyph_texcoord_buffer);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_bitmap_glyph_atlas_texture_contains_texcoord(vulkan_bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode)
{
	return bitmap_glyph_pool_contains_texcoord(&texture->pool, unicode);
}
#ifdef GLOBAL_DEBUG
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_bitmap_glyph_atlas_texture_dump(vulkan_bitmap_glyph_atlas_texture_t* texture, const char* file_path)
{
	bitmap_glyph_pool_dump(&texture->pool, file_path);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_bitmap_glyph_atlas_texture_dump_bb(vulkan_bitmap_glyph_atlas_texture_t* texture, const char* file_path)
{
	bitmap_glyph_pool_dump_bb(&texture->pool, file_path);
}
#endif /* GLOBAL_DEBUG */

END_CPP_COMPATIBLE
