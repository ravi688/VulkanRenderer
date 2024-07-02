/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: font.c is a part of VulkanRenderer

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


#include <sge/font.h>
#include <sge/mesh3d.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>
#include <ttf2mesh.h>
#include <sge/assert.h>
#include <sge/system/display.h>
#include <sge/sge.h>

#include <freetype/freetype.h>
#include <freetype/ftimage.h>

#include <ctype.h> // isgraph

SGE_API font_t* font_new(memory_allocator_t* allocator)
{
	font_t* font = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_FONT, font_t);
	memzero(font, font_t);
	return font;
}

SGE_API font_t* font_create(renderer_t* renderer, void* bytes, u64 length)
{
	font_t* font = font_new(renderer->allocator);
	font_create_no_alloc(renderer, bytes, length, font);
	return font;
}

SGE_API void font_create_no_alloc(renderer_t* renderer, void* bytes, u64 length, font_t OUT font)
{
	_debug_assert__(bytes != NULL);
	_debug_assert__(length != 0);
	memzero(font, font_t);
	font->renderer = renderer;
	font->dpi = display_get_dpi();
	font->loaded_point_size = 0;

	/* save the font data */
	font->ft_data = memory_allocator_alloc(renderer->allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, length);
	memcpy(font->ft_data, bytes, length);
	font->ft_data_size = length;

	/* we need to create a temporary buffer as ttf_load_from_mem modifies the content of the buffer */
	void* temp_buffer = memory_allocator_alloc(renderer->allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, length);
	memcpy(temp_buffer, bytes, length);
	/* WARNING: ttf_load_from_mem modifies the buffer, which had c caused a bug in which FT_New_Memory_Face was returning error code 2*/
	int result = ttf_load_from_mem(temp_buffer, length, &font->ttf_handle, false);
	if((result != 0) || (font->ttf_handle == NULL))
	{
		debug_log_fetal_error("Failed to create ttf2mesh font, bytes = %p, length = %lu", bytes, length);
		font->ttf_handle = NULL;
	}
	memory_allocator_dealloc(renderer->allocator, temp_buffer);

	AUTO error = FT_New_Memory_Face(renderer->ft_library, font->ft_data, font->ft_data_size, 0, &font->ft_handle);
	if(error != 0)
	{
		debug_log_fetal_error("Failed to create free type face, bytes = %p, length = %lu", bytes, length);
		font->ft_handle = NULL;
		memory_allocator_dealloc(renderer->allocator, font->ft_data);
		font->ft_data = NULL;
		font->ft_data_size = 0;
	}
	else
		font_set_char_size(font, 11);

	font->glyph_info_table = hash_table_create(renderer->allocator, pair_t(utf32_t, u32), font_glyph_info_t, 128, utf32_u32_equal_to, utf32_u32_hash);
}

SGE_API font_t* font_load_and_create(renderer_t* renderer, const char* file_name)
{
	font_t* font = font_new(renderer->allocator);
	font_load_and_create_no_alloc(renderer, file_name, font);
	return font;
}

SGE_API void font_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, font_t OUT font)
{
	BUFFER* data = load_binary_from_file(file_name);
	font_create_no_alloc(renderer, data->bytes, data->element_count, font);
	buf_free(data);
}

SGE_API void font_destroy(font_t* font)
{
	if(font->ttf_handle != NULL)
		ttf_free(font->ttf_handle);
	if(font->ft_handle != NULL)
	{
		FT_Done_Face(font->ft_handle);
		memory_allocator_dealloc(font->renderer->allocator, font->ft_data);
	}
	hash_table_free(&font->glyph_info_table);
}

SGE_API void font_release_resources(font_t* font)
{
	memory_allocator_dealloc(font->renderer->allocator, font);
}

SGE_API void font_set_char_size(font_t* font, u32 point_size)
{
	if(font->point_size == point_size) return;

	if(font->ft_handle != NULL)
		if(FT_Set_Char_Size(font->ft_handle, 0, point_size << 6, font->dpi.x, font->dpi.y) != 0)
			debug_log_fetal_error("Failed to set char size");
	font->point_size = point_size;
}

SGE_API bool font_load_glyph(font_t* font, utf32_t unicode, font_glyph_info_t OUT glyph_info)
{
	if(font->ft_handle != NULL)
	{
		AUTO glyph_index = FT_Get_Char_Index(font->ft_handle, unicode);

		/* if a glyph with the same glyph index and the same point size is already loaded then we can skip */
		if((glyph_index == font->ft_handle->glyph->glyph_index) && (font->loaded_point_size == font->point_size))
		{
			#if GLOBAL_DEBUG
				/* NOTE: we can't use IF_DEBUG macro here, as the ',' will be treated as another parameter next */
				AUTO key = make_pair(utf32_t, u32) { unicode, font->point_size };
			#endif /* GLOBAL_DEBUG */
			_debug_assert__(hash_table_contains(&font->glyph_info_table, &key));
			return true;
		}

		AUTO error = FT_Load_Glyph(font->ft_handle, glyph_index, FT_LOAD_DEFAULT);
		if(error != 0)
		{
			debug_log_fetal_error("Failed to load glyph Unicode (%lu)", unicode);
			return false;
		}

		font->loaded_point_size = font->point_size;

		bool is_graph = isgraph(CAST_TO(s32, unicode)) != 0;
		AUTO key = make_pair(utf32_t, u32) { unicode, font->point_size };
		if(!hash_table_contains(&font->glyph_info_table, &key))
		{
			FT_GlyphSlot glyph_slot = font->ft_handle->glyph;

			font_glyph_info_t info = { };

			/* fill up the glyph information */
			info.index = glyph_slot->glyph_index;
			info.advance_width = CAST_TO(f32, glyph_slot->advance.x >> 6);
			_debug_assert__(glyph_slot->advance.x == glyph_slot->metrics.horiAdvance);
			info.width = CAST_TO(f32, glyph_slot->metrics.width >> 6);
			info.height = CAST_TO(f32, glyph_slot->metrics.height >> 6);
			info.bearing_x = CAST_TO(f32, glyph_slot->metrics.horiBearingX >> 6);
			info.bearing_y = CAST_TO(f32, glyph_slot->metrics.horiBearingY >> 6);
			info.bitmap_top = CAST_TO(f32, glyph_slot->bitmap_top >> 6);
			info.bitmap_left = CAST_TO(f32, glyph_slot->bitmap_left >> 6);
			info.right_side_bearing = info.advance_width - (info.left_side_bearing + info.width);
			info.min_x = info.bearing_x;
			info.min_y = info.bearing_y - info.height;
			info.max_x = info.bearing_x + info.width;
			info.max_y = info.bearing_y;
			info.is_graph = is_graph;

			hash_table_add(&font->glyph_info_table, &key, &info);

			if(glyph_info != NULL)
				OUT glyph_info = info;
		}
		return is_graph;
	}
	IF_DEBUG (else { debug_log_fetal_error("Freetype font is NULL, couldn't load glyph %lu", unicode); return false; })
	return false;
}

SGE_API bool font_get_glyph_bitmap(font_t* font, utf32_t unicode, glyph_bitmap_t OUT bitmap)
{
	if(font->ft_handle != NULL)
	{
		if(!isgraph(CAST_TO(s32, unicode)))
			return false;
		_debug_assert__(bitmap != NULL);
		AUTO error = FT_Render_Glyph(font->ft_handle->glyph, FT_RENDER_MODE_NORMAL);
		if(error != 0)
		{
			debug_log_fetal_error("Failed to render glyph Unicode (%lu), did you called font_load_glyph() first?", unicode);
			return false;
		}

		if(bitmap != NULL)
		{
			FT_Bitmap _bitmap = font->ft_handle->glyph->bitmap;
			_debug_assert__(_bitmap.pixel_mode == 2);
			bitmap->pixels = _bitmap.buffer;
			bitmap->width = _bitmap.width;
			bitmap->height = _bitmap.rows;
			bitmap->channel_count = 1;
		}
	}
	IF_DEBUG (else { debug_log_fetal_error("Freetype font is NULL, couldn't render glyph %lu", unicode); return false; })
	return true;
}

SGE_API void font_get_glyph_mesh(font_t* font, utf32_t unicode, u8 mesh_quality,  mesh3d_t OUT out_mesh)
{
	_debug_assert__(out_mesh != NULL);
	mesh3d_positions_new(out_mesh, 0);
	// mesh3d_colors_new(out_mesh, 0);
	mesh3d_triangles_new(out_mesh, 0);
	font_glyph_info_t info;
	font_get_glyph_info(font, unicode, &info);
	ttf_mesh_t* mesh;
	u8 quality;
	switch(mesh_quality)
	{
		case FONT_GLYPH_MESH_QUALITY_LOW: quality = TTF_QUALITY_LOW; break;
		case FONT_GLYPH_MESH_QUALITY_NORMAL: quality = TTF_QUALITY_NORMAL; break;
		case FONT_GLYPH_MESH_QUALITY_HIGH: quality = TTF_QUALITY_HIGH; break;
		default: LOG_FETAL_ERR("Invalid font mesh quality: %u\n", mesh_quality);
	}
	CAN_BE_UNUSED_VARIABLE int result = ttf_glyph2mesh(&font->ttf_handle->glyphs[info.index], &mesh, quality, TTF_FEATURES_DFLT);
	_debug_assert__(result == TTF_DONE);

	float max_y = 0, min_y = 0, max_x = 0, min_x = 0;
	for(int i = 0; i < mesh->nvert; i++)
	{
		if(mesh->vert[i].y > max_y)
			max_y = mesh->vert[i].y;
		if(mesh->vert[i].y < min_y)
			min_y = mesh->vert[i].y;

		if(mesh->vert[i].x > max_x)
			max_x = mesh->vert[i].x;
		if(mesh->vert[i].x < min_x)
			min_x = mesh->vert[i].x;

		mesh3d_position_add(out_mesh, 0, mesh->vert[i].y, mesh->vert[i].x);
		// mesh3d_color_add(out_mesh, 1, 1, 1);
	}
	for(int i = 0; i < mesh->nfaces; i++)
		mesh3d_triangle_add(out_mesh, mesh->faces[i].v3, mesh->faces[i].v2, mesh->faces[i].v1);
	mesh3d_optimize_buffer(out_mesh);
	ttf_free_mesh(mesh);
}


SGE_API void font_get_glyph_info(font_t* font, utf32_t unicode, font_glyph_info_t* out_info)
{
	int index = ttf_find_glyph(font->ttf_handle, unicode);
	out_info->is_graph = isgraph(CAST_TO(s32, unicode)) != 0;
	if(index < 0)
	{
		LOG_FETAL_ERR("Font error: couldn't find glyph \"%c\"\n", unicode);
	}
	ttf_glyph_t info = font->ttf_handle->glyphs[index];
	out_info->left_side_bearing =  info.lbearing;
	out_info->right_side_bearing = info.rbearing;
	out_info->advance_width = info.advance;
	out_info->min_x = info.xbounds[0];
	out_info->max_x = info.xbounds[1];
	out_info->min_y = info.ybounds[0];
	out_info->max_y = info.ybounds[1];
	out_info->index = index;
}

SGE_API void font_get_glyph_info2(font_t* font, utf32_t unicode, font_glyph_info_t OUT info)
{
	AUTO key = make_pair(utf32_t, u32) { unicode, font->point_size };
	void* ptr = hash_table_get_value(&font->glyph_info_table, &key);
	if(ptr != NULL)
		OUT info = DREF_TO(font_glyph_info_t, ptr);
	else
		font_load_glyph(font, unicode, info);
}

SGE_API f32 font_get_ascender(font_t* font)
{
	_release_assert__(font->ft_handle != NULL);
	return CAST_TO(f32, font->ft_handle->ascender);
}

SGE_API f32 font_get_units_per_em(font_t* font)
{
	_release_assert__(font->ft_handle != NULL);
	return CAST_TO(f32, font->ft_handle->units_per_EM);
}