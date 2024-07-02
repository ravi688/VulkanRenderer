/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_glyph_pool.c is a part of VulkanRenderer

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

#include <sge/bitmap_glyph_pool.h>
#include <sge/sge.h>
#include <sge/bmp.h> // bmp_write
#include <ctype.h> // isgraph

SGE_API bitmap_glyph_pool_t* bitmap_glyph_pool_new(memory_allocator_t* allocator)
{
	bitmap_glyph_pool_t* pool = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_BITMAP_GLYPH_POOL, bitmap_glyph_pool_t);
	memzero(pool, bitmap_glyph_pool_t);
	OBJECT_INIT(pool, OBJECT_TYPE_BITMAP_GLYPH_POOL, OBJECT_NATIONALITY_INTERNAL);
	return pool;
}

SGE_API bitmap_glyph_pool_t* bitmap_glyph_pool_create(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info)
{
	bitmap_glyph_pool_t* pool = bitmap_glyph_pool_new(renderer->allocator);
	bitmap_glyph_pool_create_no_alloc(renderer, create_info, pool);
	return pool;
}

SGE_API void bitmap_glyph_pool_create_no_alloc(renderer_t* renderer, bitmap_glyph_pool_create_info_t* create_info, bitmap_glyph_pool_t OUT pool)
{
	OBJECT_MEMZERO(pool, bitmap_glyph_pool_t);
	pool->renderer = renderer;
	pool->font = create_info->font;
	buffer2d_create_info_t _create_info =
	{
		.size = { create_info->size.x * sizeof(u8), create_info->size.y },
		.buffer = create_info->buffer,
		.view = create_info->view,
		.resize_mode = BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT,
		.key_size = sizeof(pair_t(utf32_t, u32)),
		.key_hash_function = utf32_u32_hash,
		.key_comparer = utf32_u32_equal_to
	};
	buffer2d_create_no_alloc_ext(renderer->allocator, &_create_info, &pool->pixels);
	buffer2d_clear(&pool->pixels, NULL);
}

SGE_API void bitmap_glyph_pool_destroy(bitmap_glyph_pool_t* pool)
{
	buffer2d_destroy(&pool->pixels);
}

SGE_API void bitmap_glyph_pool_release_resources(bitmap_glyph_pool_t* pool)
{
	buffer2d_release_resources(&pool->pixels);
	if(OBJECT_IS_INTERNAL(pool))
		memory_allocator_dealloc(pool->renderer->allocator, pool);
}

SGE_API bool bitmap_glyph_pool_get_texcoord(bitmap_glyph_pool_t* pool, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord, bool OUT is_resized)
{
	AUTO info = buffer2d_get_rect(&pool->pixels, &unicode);
	/* if no rect exists confiding/holding the rasterized glyph's pixels */
	if(info == NULL)
	{
		/* then rasterze it and put it into the pool */

		/* set the point size */
		u32 saved_char_size = U32_MAX;
		if(unicode.second != font_get_char_size(pool->font))
		{
			saved_char_size = font_get_char_size(pool->font);
			_debug_assert__(saved_char_size != U32_MAX);
			font_set_char_size(pool->font, unicode.second);
		}

		/* load the glyph */
		if(!font_load_glyph(pool->font, unicode.first, NULL))
		{
			if(isgraph(unicode.first))
				debug_log_fetal_error("Unable to load the glyph: %u, point size %lu", unicode.first, unicode.second);
			else
				debug_log_warning("Unable to load a whitespace glyph: %u, point size %lu", unicode.first, unicode.second);
			return false;
		}

		/* rasterize the glyph */
		glyph_bitmap_t bitmap;
		if(!font_get_glyph_bitmap(pool->font, unicode.first, &bitmap))
		{
			if(isgraph(unicode.first))
				debug_log_fetal_error("Unable to rasterize the glyph with unicode %u and point size %u", unicode.first, unicode.second);
			else
				debug_log_warning("Unable to rasterize a whitespace glyph %u", unicode.first);
			return false;
		}

		/* restore the point size */
		if(saved_char_size != U32_MAX)
			font_set_char_size(pool->font, saved_char_size);

		/* currently we are only working with single channel font bitmaps */
		_debug_assert__(bitmap.channel_count == 1);

		/* pack the glyph pixels into the font bitmap  */
		bool _is_resized = IF_DEBUG( buffer2d_push_debug(&pool->pixels, &unicode, bitmap.pixels, bitmap.width, bitmap.height) )
						   ELSE( buffer2d_push(&pool->pixels, &unicode, bitmap.pixels, bitmap.width, bitmap.height) );
		if(is_resized != NULL)
			OUT is_resized = _is_resized;

		/* now get the reference to the packed pixel information */
		info = buffer2d_get_rect(&pool->pixels, &unicode);
		_debug_assert__(info != NULL);
	}

	if(texcoord != NULL)
	{
		irect2d_t rect = info->rect_info.rect;
		iextent2d_t size = pool->pixels.view->size;
		OUT texcoord = (glyph_texcoord_t)
		{
			.tltc = { (f32)rect.offset.x / size.width, (f32)rect.offset.y / size.height },
			.trtc = { (f32)(rect.offset.x + rect.extent.x) / size.width, (f32)rect.offset.y / size.height },
			.brtc = { (f32)(rect.offset.x + rect.extent.x) / size.width, (f32)(rect.offset.y + rect.extent.y) / size.height },
			.bltc = { (f32)rect.offset.x / size.width, (f32)(rect.offset.y + rect.extent.y) / size.height }
		};
	}
	return true;
}

SGE_API bool bitmap_glyph_pool_contains_texcoord(bitmap_glyph_pool_t* pool, pair_t(utf32_t, u32) unicode)
{
	return buffer2d_get_rect(&pool->pixels, &unicode) != NULL;
}

SGE_API void bitmap_glyph_pool_dump(bitmap_glyph_pool_t* pool, const char* file_path)
{
	void* ptr = buf_get_ptr(buffer2d_get_backed_buffer(&pool->pixels));
	bmp_write(ptr, pool->pixels.view->size.x / sizeof(u8), pool->pixels.view->size.y, 1, file_path);
}

#ifdef GLOBAL_DEBUG
SGE_API void bitmap_glyph_pool_dump_bb(bitmap_glyph_pool_t* pool, const char* file_path)
{
	buffer2d_dump(&pool->pixels, file_path);
}
#endif
