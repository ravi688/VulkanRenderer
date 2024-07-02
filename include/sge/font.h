/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: font.h is a part of VulkanRenderer

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
#include <sge/rect.h> 	// iextent2d_t
#include <sge/comparer.h> // u32_equal_to
#include <sge/hash_function.h> // u32_hash
#include <sge/hash_table.h>

typedef u32 utf32_t;
#define utf32_equal_to u32_equal_to
#define utf32_hash u32_hash

typedef_pair_t(utf32_t, u32);
#define utf32_u32_equal_to u64_equal_to
#define utf32_u32_hash u64_hash

enum
{
	FONT_GLYPH_MESH_QUALITY_LOW,
	FONT_GLYPH_MESH_QUALITY_NORMAL,
	FONT_GLYPH_MESH_QUALITY_HIGH
};

typedef struct mesh3d_t mesh3d_t;

typedef struct FT_FaceRec_*  FT_Face;
typedef struct ttf_file          ttf_t;

/* **NOTE: for now we are only dealing with horizonal fonts** */

/* stores common glyph metrices and information */
typedef struct font_glyph_info_t
{
	/* index of the glyph in the type face */
	u32 index;

	f32 min_x;
	f32 max_x;
	f32 min_y;
	f32 max_y;

	/* advance */
	f32 advance_width;

	/* width of the glyph bounding box in point size (72 points equals to 1 inch)
	 * it can be calculated as: max_x - max_y */
	f32 width;
	/* height of the glyph bounding box in point size (72 points equals to 1 inch)
	 * it can be calculated as: max_y - min_y */
	f32 height;

	/* horizontal bearing (distance of the left most edge of the bounding box of the glyph from the origin of the glyph */
	union
	{
		f32 bearing_x;
		f32 left_side_bearing;
	};
	/* vertical bearing (distance of the top most edge of the bouding box of the glyph from the baseline of the font */
	f32 bearing_y;
	/* right side bearing (distance of the right most edge of the bouding box of the glyph from the advance width
	 * it can be calculated as: advance_width - left_side_bearing - width */
	f32 right_side_bearing;

	f32 bitmap_top;
	f32 bitmap_left;

	/* true if this glyph has a grpahical representation, otherwise false */
	bool is_graph;
} font_glyph_info_t;

typedef struct glyph_bitmap_t
{
	/* in memory buffer holding pixel values */
	void* pixels;
	/* width in pixels */
	u32 width;
	/* height in pixels */
	u32 height;
	/* number of channels */
	u32 channel_count;
} glyph_bitmap_t;

typedef struct font_t
{
	renderer_t* renderer;
	ttf_t* ttf_handle;

	struct
	{
		FT_Face ft_handle;
		/* the font data must not be deallocated in case of free type library, hence we have to keep a copy of it */
		void* ft_data;
		u32 ft_data_size;
	};
	/* display resolution in dpi */
	iextent2d_t dpi;
	u32 point_size;
	u32 loaded_point_size;
	/* cached glyph infos as we are not sure how does the FT_Load_Glyph works and how much expensive it is */
	hash_table_t glyph_info_table;
} font_t;

BEGIN_CPP_COMPATIBLE

SGE_API font_t* font_new(memory_allocator_t* allocator);
SGE_API font_t* font_create(renderer_t* renderer, void* bytes, u64 length);
SGE_API void font_create_no_alloc(renderer_t* renderer, void* bytes, u64 length, font_t OUT font);
SGE_API font_t* font_load_and_create(renderer_t* renderer, const char* file_name);
SGE_API void font_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, font_t OUT font);
SGE_API void font_destroy(font_t* font);
SGE_API void font_release_resources(font_t* font);

/* sets the point size of the glyphs in this font */
SGE_API void font_set_char_size(font_t* font, u32 point_size);

/* loads 'info' with information of the loaded glyph represented by unicode encoding 'unicode' */
SGE_API bool font_load_glyph(font_t* font, utf32_t unicode, font_glyph_info_t OUT glyph_info);

/* returns true if glyph has graphical representation and there are no errors, and fills buffer 'buffer' with antialiased bitmap values (GREY) */
SGE_API bool font_get_glyph_bitmap(font_t* font, utf32_t unicode, glyph_bitmap_t OUT bitmap);

/* fills 'mesh' with 3d polygon data of the glyph represented by unicode encoding 'unicode' */
SGE_API void font_get_glyph_mesh(font_t* font, utf32_t unicode, u8 mesh_quality, mesh3d_t OUT mesh);

/* fills 'info' with information of the glyph represented by unicode encoding 'unicode' */
SGE_API void font_get_glyph_info(font_t* font, utf32_t unicode, font_glyph_info_t OUT info);
/* fills 'info' with information of the glyph only but calls font_load_glyph internally if required (be careful) */
SGE_API void font_get_glyph_info2(font_t* font, utf32_t unicode, font_glyph_info_t OUT info);

/* returns the ascender of the face (the ascender of the glyph having heighest value in the face) */
SGE_API f32 font_get_ascender(font_t* font);

/* returns the units (in point size) per EM */
SGE_API f32 font_get_units_per_em(font_t* font);

/* returns the point size of the characters in the font */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 font_get_char_size(font_t* font) { return font->point_size; }

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE f32 get_inches_from_point_size(u32 point_size)
{
	/* 72 points make 1 inch */
	return point_size * 1.0 / 72;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 get_pixels_from_point_size(u32 point_size, u32 dpi)
{
	/* convert inches to pixels by multiplying with 'dots per inch' */
	return CAST_TO(u32, (get_inches_from_point_size(point_size) * dpi));
}

END_CPP_COMPATIBLE
