/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_glyph_atlas_texture.h is a part of VulkanRenderer

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

#if   defined(SGE_VULKAN_DRIVER)
	typedef struct vulkan_bitmap_glyph_atlas_texture_t vulkan_bitmap_glyph_atlas_texture_t;
	typedef vulkan_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef struct vulkan_texture_t vulkan_texture_t;
	typedef vulkan_texture_t texture_t;
#elif defined(SGE_OPENGL_DRIVER)
	typedef struct opengl_bitmap_glyph_atlas_texture_t opengl_bitmap_glyph_atlas_texture_t;
	typedef opengl_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef struct opengl_texture_t opengl_texture_t;
	typedef opengl_texture_t texture_t;
#elif defined(SGE_DIRECTX_DRIVER)
	typedef struct directx_bitmap_glyph_atlas_texture_t directx_bitmap_glyph_atlas_texture_t;
	typedef directx_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef struct directx_texture_t directx_texture_t;
	typedef directx_texture_t texture_t;
#elif defined(SGE_METAL_DRIVER)
	typedef struct metal_bitmap_glyph_atlas_texture_t metal_bitmap_glyph_atlas_texture_t;
	typedef metal_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
	typedef struct metal_texture_t metal_texture_t;
	typedef metal_texture_t texture_t;
#endif

#include <sge/font.h> // font_t and utf32_t
#include <sge/bitmap_glyph_pool.h> // bitmap_glyph_pool_t and glyph_texcoord_t

typedef struct bitmap_glyph_atlas_texture_create_info_t
{
	/* initial width of the texture */
	u32 width;
	/* initial height of the texture */
	u32 height;
	/* font used to rasterize glyphs */
	font_t* font;
} bitmap_glyph_atlas_texture_create_info_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator);
SGE_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_create(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info);
SGE_API void bitmap_glyph_atlas_texture_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info, bitmap_glyph_atlas_texture_t OUT texture);
SGE_API void bitmap_glyph_atlas_texture_destroy(bitmap_glyph_atlas_texture_t* texture);
SGE_API void bitmap_glyph_atlas_texture_release_resources(bitmap_glyph_atlas_texture_t* texture);

/* returns pointer to the bitmap_glyph_pool_t object */
SGE_API bitmap_glyph_pool_t* bitmap_glyph_atlas_texture_get_pool(bitmap_glyph_atlas_texture_t* texture);
/* returns the font used by this BGA texture */
SGE_API font_t* bitmap_glyph_atlas_texture_get_font(bitmap_glyph_atlas_texture_t* texture);
/* retursn a pointer to the underlying texture_t object */
SGE_API texture_t* bitmap_glyph_atlas_texture_get_texture(vulkan_bitmap_glyph_atlas_texture_t* texture);
/* flushes the host side font bitmap to the GPU side memory (it might also destroy and create VkDeviceMemory)
 * is_resized: true if internal GPU and host memory has been resized
 * returns: true if internal API objects recreated or GPU side memory has been updated */
SGE_API bool bitmap_glyph_atlas_texture_commit(bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized);
/* quality: quality of the rasterized glyph, ranges from 0 to 255 inclusive
 * unicode: glyph's unicode value to rasterize
 * texcoords: the texture coordinates (list of 4 vec2(s)), filled by this function if the glyph has graphical representation
 * returns: true if the glyph has graphical representation and there are no errors */
SGE_API bool bitmap_glyph_atlas_texture_get_texcoord(bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord);
#ifdef GLOBAL_DEBUG
SGE_API void bitmap_glyph_atlas_texture_dump(bitmap_glyph_atlas_texture_t* texture, const char* file_path);
SGE_API void bitmap_glyph_atlas_texture_dump_bb(bitmap_glyph_atlas_texture_t* texture, const char* file_path);
#endif /* GLOBAL_DEBUG */
END_CPP_COMPATIBLE
