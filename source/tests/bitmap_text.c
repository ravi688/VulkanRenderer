/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_text.c is a part of VulkanRenderer

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

/*
	test id: BITMAP_TEXT
	Test to check if bitmap text is working fine
 */

#include <renderer/tests/bitmap_text.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_DATA_STRUCTURES
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <renderer/hash_table.h>
#include <renderer/bmp.h>
#include <renderer/color.h>
#include <renderer/buffer2d.h>
#include <renderer/system/display.h>
#include <renderer/bitmap_glyph_atlas_texture.h>
#include <renderer/bitmap_text.h>
#include <renderer/conio.h>

#include <freetype/ft2build.h>
#include <freetype/freetype/freetype.h>
#include <freetype/freetype/ftimage.h>

#include <disk_manager/file_reader.h>

TEST_DATA(BITMAP_TEXT)
{
	render_scene_t* scene;

	camera_t* camera;

	font_t* font;

	bitmap_glyph_atlas_texture_t* texture;
	bitmap_text_t* text;
	bitmap_text_string_handle_t text_string_handle;
	bitmap_text_string_handle_t another_string_handle;

	render_object_t* text_object;
	material_t* text_material;

	int isScreenSpace; /* bools are not support currently in V3DShader (see: TID64)*/
};


SETUP_TEST(BITMAP_TEXT);

TEST_ON_RENDERER_INITIALIZE(BITMAP_TEXT)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "BITMAP_TEXT (press any key to switch from Screen space to World space and vice versa",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

static void print_key_value_pair(void* key, void* value, void* user_data)
{
	debug_log_info("Pair: %s, %lu", *(char**)key, DREF_TO(u32, value));
}

static void print_key_value_pair_i(void* key, void* value, void* user_data)
{
	debug_log_info("Pair: %lu, %lu", *(u32*)key, DREF_TO(u32, value));
}

static void test_hash_table()
{
	/* hash table test */

	hash_table_t _table = hash_table_create(NULL, u64, u32, 5, u64_equal_to, u64_hash);
	u64 key = 40;
	u32 _value = 500;
	hash_table_add(&_table, &key, &_value);
	key = 41; _value = 600;
	hash_table_add(&_table, &key, &_value);
	key = 46; _value = 800;
	hash_table_add(&_table, &key, &_value);
	key = 52; _value = 700;
	hash_table_add(&_table, &key, &_value);
	key = 62; _value = 800;
	hash_table_add(&_table, &key, &_value);
	key = 33; _value = 900;
	hash_table_add(&_table, &key, &_value);
	hash_table_foreach(&_table, print_key_value_pair_i, NULL);
	hash_table_free(&_table);

	hash_table_t table = hash_table_create(NULL, const char*, u32, 5, string_equal_to, string_hash);
	const char* str = "Hello World";
	u32 value = 100;
	hash_table_add(&table, &str, &value); value = 200;
	str = "Hello World 2";
	hash_table_add(&table, &str, &value); value = 300;
	str = "ABCD";
	hash_table_add(&table, &str, &value); value = 500;
	str = "3434";
	hash_table_add(&table, &str, &value); value = 5430;
	const char* _str = "343654";
	hash_table_add(&table, &_str, &value); value = 505;
	str = "3436";
	hash_table_add(&table, &str, &value); value = 5023;
	str = "3437";

	str = "Hello World";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));
	str = "ABCD";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));
	str = "Hello World 2";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));

	str = "ABCD";
	hash_table_remove(&table, &str);

	hash_table_foreach(&table, print_key_value_pair, NULL);

	hash_table_free(&table);
}

static void test_bitmap()
{
	/*f32 half_diag = sqrt(2) * 256;
	s32 wave_number = 1;*/
	s32 num_divisons = 8;
	u8 data[512 * 512];
	for(s32 i = 0; i < 512; i++)
	{
		for(s32 j = 0; j < 512; j++)
		{
			/* ** Not sure why this is not working and giving completely unexpected results */
			/*
			f32 t = vec2_magnitude(vec2_sub(vec2(i, j), vec2(256, 256))) / half_diag;
			t = t * wave_number * 3.14159f;
			AUTO color = color3(fmaxf(sin(t), 0), fmaxf(sin(t - 1.57f), 0), t);
			data[i * 512 + j] = icolor3(color.r * 255, color.g * 255, color.b * 255);
			*/
			s32 size = 512 / num_divisons;
			s32 y = (i / size) * size;
			s32 x = (j / size) * size;

			if(((i >= x) && (i < (size + x))) && ((j >= y) < (size + y)))
				data[i * 512 + j] = 255 /*ICOLOR3_YELLOW*/;
			else
				data[i * 512 + j] = 0 /*ICOLOR3_BLUE*/;

			// if(i < 100)
			// 	data[i * 512 + j] = ICOLOR3_YELLOW;
			// else
			// 	data[i * 512 + j] = ICOLOR3_BLUE;
		}
	}
	bmp_write(data, 512, 512, 1, "test_bitmap.bmp");
}

static void test_buffer2d_view(renderer_t* renderer)
{
	iextent2d_t size = { 512, 512 };
	buffer_t pixels = memory_allocator_buf_create(renderer->allocator, sizeof(icolor3_t), IEXTENT2D_AREA(size), 0);
	AUTO default_color = ICOLOR3_WHITE;
	buf_clear_buffer(&pixels, &default_color);
	buf_set_element_size(&pixels, 1); // as buffer2d_view only works with element_size = 1 buffers
	buf_set_capacity(&pixels, sizeof(icolor3_t) * IEXTENT2D_AREA(size));

	buffer2d_view_create_info_t view_create_info =
	{
		.size = { size.x * sizeof(icolor3_t), size.y },
		.buffer = &pixels
	};
	buffer2d_view_t view;
	buffer2d_view_create_no_alloc_ext(renderer->allocator, &view_create_info, &view);

	icolor3_t colors[IEXTENT2D_AREA(size)];
	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_RED;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(32 * 3, 0), iextent2d(3 * 480, 32)), colors);

	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_BLUE;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(0, 32), iextent2d(32 * 3, 480)), colors);

	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_GREEN;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(32 * 3, 32), iextent2d(3 * 480, 480)), colors);

	bmp_write(buf_get_ptr(&pixels), size.x, size.y, 3, "buffer2d_view.bmp");

	buffer2d_view_destroy(&view);
	buffer2d_view_release_resources(&view);
	buf_free(&pixels);
}

typedef struct input_t
{
	u32 key;
	u32 width;
	u32 height;
} input_t;

CAN_BE_UNUSED_VARIABLE static icolor3_t colors[] =
{
	{ 255, 255, 255 },
	{ 0, 0, 0 },
	{ 0, 255, 0 },
	{ 0, 0, 255 },
	{ 255, 0, 0 },
	{ 0, 255, 255 },
	{ 128, 255, 255 },
	{ 150, 50, 50 },
	{ 255, 170, 170 }
};

#define KEY __COUNTER__

static input_t inputs[] =
{
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 128, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 5, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 64, 128 },
	{ KEY, 480, 32 },
	{ KEY, 32, 448 },
	{ KEY, 5, 128 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
	{ KEY, 32, 32 },
};

static void test_buffer2d(renderer_t* renderer)
{
	buffer2d_create_info_t create_info =
	{
		.size = { 512, 512 },
		.buffer = NULL,
		.resize_mode = BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT,
		.key_size = sizeof(u32),
		.key_comparer = u32_equal_to,
		.key_hash_function = u32_hash
	};
	buffer2d_t buffer;
	buffer2d_create_no_alloc_ext(renderer->allocator, &create_info, &buffer);

	u32 input_count = SIZEOF_ARRAY(inputs);
	for(u32 i = 0; i < input_count; i++)
	{
		IF_DEBUG( AUTO color = colors[i % SIZEOF_ARRAY(colors)] );
		buffer2d_push(&buffer, &inputs[i].key, NULL, inputs[i].width, inputs[i].height PARAM_IF_DEBUG(color));
	}

	IF_DEBUG( buffer2d_dump(&buffer, "buffer2d_dump.bmp") );

	debug_log_info("Packing Efficientcy: %f", buffer2d_get_packing_efficiency(&buffer));

	buffer2d_destroy(&buffer);
	buffer2d_release_resources(&buffer);
}

static void test_glyph_rasterization(renderer_t* renderer)
{
	BUFFER* data = load_binary_from_file("showcase/resource/fonts/arial.ttf");

	FT_Face face;
	AUTO error = FT_New_Memory_Face(renderer->ft_library, data->bytes, data->element_count, 0, &face);

	if(error)
	{
		debug_log_error("An error occurred while loading font file");
		return;
	}

	debug_log_info("Num glyphs: %lu", face->num_glyphs);

	AUTO dpi = display_get_dpi();
	debug_log_info("DPI: width = %lu dpi, height = %lu dpi", dpi.x, dpi.y);

	error = FT_Set_Char_Size(face, 0, 11*64, dpi.x, dpi.y);

	if(error)
	{
		debug_log_error("Couldn't set the char size");
		return;
	}

	AUTO glyph_index = FT_Get_Char_Index(face, 'A');

	_debug_assert__(glyph_index != 65);

	error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

	if(error)
	{
		debug_log_error("Couldn't load the glyph");
		return;
	}

	error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

	if(error)
	{
		debug_log_error("Couldn't render the glyph");
		return;
	}

	AUTO glyph_bitmap = face->glyph->bitmap;

	_debug_assert__(glyph_bitmap.pixel_mode == 2);
	bmp_write(glyph_bitmap.buffer, glyph_bitmap.width, glyph_bitmap.rows, 1, "test_glyph.bmp");

	FT_Done_Face(face);
	buf_free(data);
}

static void test_buffer2d_backed_buffer_dump(renderer_t* renderer)
{
	buffer2d_create_info_t create_info =
	{
		.size = { sizeof(icolor3_t) * 1024, 1024 },
		.buffer = NULL,
		.resize_mode = BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT,
		.key_size = sizeof(u32), // UTF-32
		.key_comparer = u32_equal_to,
		.key_hash_function = u32_hash
	};
	buffer2d_t bitmap;
	buffer2d_create_no_alloc_ext(renderer->allocator, &create_info, &bitmap);
	u8 default_color = 255;
	buffer2d_clear(&bitmap, (void*)&default_color);

	u32 input_count = SIZEOF_ARRAY(inputs);
	for(u32 i = 0; i < input_count; i++)
	{
		IF_DEBUG( AUTO color = colors[i % SIZEOF_ARRAY(colors)] );
		buffer2d_push_broadcast(&bitmap, &inputs[i].key, IF_DEBUG((void*)&color) ELSE(NULL), sizeof(icolor3_t), inputs[i].width * sizeof(icolor3_t), inputs[i].height PARAM_IF_DEBUG(color));
	}

	void* ptr = buf_get_ptr(buffer2d_get_backed_buffer(&bitmap));
	bmp_write(ptr, bitmap.view->size.x / sizeof(icolor3_t), bitmap.view->size.y, 3, "test_buffer2d_backed_buffer_dump.bmp");

	buffer2d_destroy(&bitmap);
	buffer2d_release_resources(&bitmap);
}

static void test_glyph_packing(renderer_t* renderer)
{
	/* create font */
	font_t font;
	font_load_and_create_no_alloc(renderer, "showcase/resource/fonts/arial.ttf", &font);

	/* create bitmap glyph pool */
	bitmap_glyph_pool_create_info_t create_info =
	{
		.size = { 128, 128 },
		.buffer = NULL,
		.font = &font
	};
	bitmap_glyph_pool_t* pool = bitmap_glyph_pool_create(renderer, &create_info);

	for(char i = 32; i < 86; ++i)
	{
		glyph_texcoord_t texcoord;
		font_set_char_size(&font, i / 2);
		bitmap_glyph_pool_get_texcoord(pool, make_pair(utf32_t, u32) { i, i / 2 }, &texcoord, NULL);
		font_set_char_size(&font, i / 4);
		bitmap_glyph_pool_get_texcoord(pool, make_pair(utf32_t, u32) { i, i / 4 }, &texcoord, NULL);
	}

	IF_DEBUG( bitmap_glyph_pool_dump(pool, "test_bitmap_glyph_pool_dump.bmp") );
	IF_DEBUG( bitmap_glyph_pool_dump_bb(pool, "test_bitmap_glyph_pool_dump_bb.bmp") );

	bitmap_glyph_pool_destroy(pool);
	bitmap_glyph_pool_release_resources(pool);

	font_destroy(&font);
}

static void test_bufferlib()
{
	buffer_t buffer = buf_new(u32);
	buf_push_u32(&buffer, 100);
	buf_push_u32(&buffer, 300);
	buf_push_u32(&buffer, 400);
	buf_push_u32(&buffer, 500);
	buf_push_u32(&buffer, 700);
	buf_traverse_elements(&buffer, 0, buf_get_element_count(&buffer) - 1, buf_u32_print, NULL); puts("");
	buf_insert_pseudo(&buffer, 3, 4);
	buf_traverse_elements(&buffer, 0, buf_get_element_count(&buffer) - 1, buf_u32_print, NULL); puts("");
	buf_free(&buffer);
}

#if DBG_ENABLED(BUFFER2D_RESIZE)

	static glyph_texcoord_t get_glyph_texcoord(irect2d_t rect, iextent2d_t tex_size)
	{
		iextent2d_t size = tex_size;
		return (glyph_texcoord_t)
		{
			.tltc = { (f32)rect.offset.x / size.width, (f32)rect.offset.y / size.height },
			.trtc = { (f32)(rect.offset.x + rect.extent.x) / size.width, (f32)rect.offset.y / size.height },
			.brtc = { (f32)(rect.offset.x + rect.extent.x) / size.width, (f32)(rect.offset.y + rect.extent.y) / size.height },
			.bltc = { (f32)rect.offset.x / size.width, (f32)(rect.offset.y + rect.extent.y) / size.height }
		};
	}

	typedef bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_ptr_t;
	typedef string_builder_t* string_builder_ptr_t;
	typedef_pair_t(bitmap_glyph_atlas_texture_ptr_t, string_builder_ptr_t);

	static void packed_rect_relocate_handler(const rect2d_info_t* before, const rect2d_info_t* after, void* user_data)
	{
		AUTO pair = CAST_TO(pair_t(bitmap_glyph_atlas_texture_ptr_t, string_builder_ptr_t)*, user_data);
		AUTO builder = pair->second;
		iextent2d_t tex_size = buffer2d_view_get_size(pair->first->pool.pixels.view);
		AUTO btc = get_glyph_texcoord(before->rect, iextent2d(tex_size.x >> 1, tex_size.y >> 1));
		AUTO atc = get_glyph_texcoord(after->rect, tex_size);
		string_builder_append(builder, "\n|(%.3f, %.3f), (%.3f, %.3f)|  -->  |(%.3f, %.3f) (%.3f, %.3f)|\n",
										btc.tltc.x, btc.tltc.y, btc.trtc.x, btc.trtc.y,
										atc.tltc.x, atc.tltc.y, atc.trtc.x, atc.trtc.y);
		string_builder_append(builder, "|(%.3f, %.3f), (%.3f, %.3f)|  -->  |(%.3f, %.3f) (%.3f, %.3f)|\n",
										btc.bltc.x, btc.bltc.y, btc.brtc.x, btc.brtc.y,
										atc.bltc.x, atc.bltc.y, atc.brtc.x, atc.brtc.y);
	}
#endif /* DBG_BUFFER2D_RESIZE */

TEST_ON_INITIALIZE(BITMAP_TEXT)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_BLACK, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_transform(this->camera, mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));

	this->text_material = material_library_getH(mlib,
						 	material_library_create_materialH(mlib,
						 	shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT),
						 	"BitmapTextShaderTest"));

	this->font = font_load_and_create(renderer, "showcase/resource/fonts/arial.ttf");
	font_set_char_size(this->font, 24);

	/* bitmap text */
	bitmap_glyph_atlas_texture_create_info_t texture_create_info = { 512, 512, this->font };
	this->texture = bitmap_glyph_atlas_texture_create(renderer, &texture_create_info);

	#if DBG_ENABLED(BUFFER2D_RESIZE)
		/* setup the callback handler for debugging */
		AUTO user_data = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, pair_t(bitmap_glyph_atlas_texture_ptr_t, string_builder_ptr_t));
		user_data->first = this->texture;
		user_data->second = renderer->debug_log_builder;
		buffer2d_set_packed_rect_relocate_callback_handler(&this->texture->pool.pixels, packed_rect_relocate_handler, user_data);
	#endif /* DBG_BUFFER2D_RESIZE */

	material_set_float(this->text_material, "parameters.color.r", 1.0f);
	material_set_float(this->text_material, "parameters.color.g", 1.0f);
	material_set_float(this->text_material, "parameters.color.b", 1.0f);
	this->text = bitmap_text_create(renderer, this->texture);
	this->text_string_handle = bitmap_text_string_create(this->text);
	this->another_string_handle = bitmap_text_string_create(this->text);
	bitmap_text_string_set_point_sizeH(this->text, this->another_string_handle, 24);
	bitmap_text_string_set_point_sizeH(this->text, this->text_string_handle, 35);
	bitmap_text_string_setH(this->text, this->another_string_handle, "Hardwork with dedication suffices to c");
	bitmap_text_string_setH(this->text, this->text_string_handle, "Hardwork with dedication suffices to c: 1234324");

	#ifdef GLOBAL_DEBUG
	bitmap_glyph_atlas_texture_dump(this->texture, "bitmap_glyph_atlas_texture.dump.bmp");
	#endif

	this->text_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_TEXT, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->text_object, this->text_material);
	render_object_attach(this->text_object, this->text);
	bitmap_text_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 400.0f, -400.0f));

	render_scene_build_queues(this->scene);

	test_hash_table();
	test_bitmap();
	test_buffer2d_view(renderer);
	test_buffer2d(renderer);
	test_glyph_rasterization(renderer);
	test_buffer2d_backed_buffer_dump(renderer);
	test_glyph_packing(renderer);
	test_bufferlib();

	#if DBG_ENABLED(BUFFER2D_RESIZE)
		memory_allocator_dealloc(renderer->allocator, user_data);
	#endif
}

TEST_ON_TERMINATE(BITMAP_TEXT)
{
	bitmap_text_destroy(this->text);
	bitmap_text_release_resources(this->text);
	bitmap_glyph_atlas_texture_destroy(this->texture);
	bitmap_glyph_atlas_texture_release_resources(this->texture);

	font_destroy(this->font);
	font_release_resources(this->font);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(BITMAP_TEXT)
{
	static bool isScreenSpace = true;
	if(kbhit())
	{
		char ch = getch();
		if((ch != 'p') && (ch != 'q') && (ch != 'd') && (ch != 'u'))
		{
			isScreenSpace = !isScreenSpace;
			if(isScreenSpace)
			{
				bitmap_text_set_render_space_type(this->text, BITMAP_TEXT_RENDER_SPACE_TYPE_2D);
				debug_log_info("BITMAP_TEXT_RENDER_SPACE_TYPE_2D");
				render_object_set_transform(this->text_object, mat4_identity());
				bitmap_text_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 400.0f, -400.0f));
			}
			else
			{
				bitmap_text_set_render_space_type(this->text, BITMAP_TEXT_RENDER_SPACE_TYPE_3D);
				debug_log_info("BITMAP_TEXT_RENDER_SPACE_TYPE_3D");
				render_object_set_transform(this->text_object, mat4_mul(3, mat4_rotation_y(45 DEG), mat4_scale(0.005f, 0.005f, 0.005f), mat4_translation(0.0f, 0.0f, -0.5f)));
				bitmap_text_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 30.0f, 0.0f));
			}
		}
		else if(ch == 'p')
		{
			bitmap_text_string_set_point_sizeH(this->text, this->another_string_handle, bitmap_text_string_get_point_sizeH(this->text, this->another_string_handle) * 2);
			bitmap_text_string_set_point_sizeH(this->text, this->text_string_handle, bitmap_text_string_get_point_sizeH(this->text, this->text_string_handle) * 2);
		}
		else if(ch == 'q')
		{
			bitmap_text_string_set_point_sizeH(this->text, this->another_string_handle, bitmap_text_string_get_point_sizeH(this->text, this->another_string_handle) / 2);
			bitmap_text_string_set_point_sizeH(this->text, this->text_string_handle, bitmap_text_string_get_point_sizeH(this->text, this->text_string_handle) / 2);
		}
		else if(ch == 'd')
		{
			#ifdef GLOBAL_DEBUG
			bitmap_glyph_atlas_texture_dump_bb(this->texture, "bitmap_glyph_atlas_texture_bb.dump.bmp");
			bitmap_glyph_atlas_texture_dump(this->texture, "bitmap_glyph_atlas_texture.dump.bmp");
			#endif
		}
		else if(ch == 'b')
		{
			static char ch = 33;
			bitmap_glyph_atlas_texture_get_texcoord(this->texture, make_pair(utf32_t, u32) { ch++, 34 }, NULL);
		}
	}

	static int counter = 0;
	counter++;
	if(counter == 66000)
	counter = 0;
	char buffer[128] =  { };
	sprintf(buffer, "%d", counter);
	bitmap_text_string_setH(this->text, this->text_string_handle, buffer);
	sprintf(buffer, "%d", counter);
	bitmap_text_string_setH(this->text, this->another_string_handle, buffer);

	bitmap_glyph_atlas_texture_commit(this->texture, NULL);
}

TEST_ON_RENDER(BITMAP_TEXT)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}
