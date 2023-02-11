
#pragma once

#include <renderer/defines.h>

#include <ttf2mesh.h>

enum
{
	FONT_GLYPH_MESH_QUALITY_LOW,
	FONT_GLYPH_MESH_QUALITY_NORMAL,
	FONT_GLYPH_MESH_QUALITY_HIGH
};

typedef struct font_t font_t;
typedef struct mesh3d_t mesh3d_t;

typedef struct font_glyph_info_t
{
	u32 index;

	float min_x;
	float max_x;
	float min_y;
	float max_y;

	float advance_width;

	float left_side_bearing;
	float right_side_bearing;
} font_glyph_info_t;

typedef struct font_t
{
	ttf_t* handle;
	memory_allocator_t* allocator;
} font_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API font_t* font_create(memory_allocator_t* allocator, void* bytes, u64 length);
RENDERER_API font_t* font_load_and_create(memory_allocator_t* allocator, const char* file_name);
RENDERER_API void font_destroy(font_t* font);
RENDERER_API void font_release_resources(font_t* font);

RENDERER_API void font_get_glyph_mesh(font_t* font, u16 wide_char, u8 mesh_quality, mesh3d_t* out_mesh);
RENDERER_API void font_get_glyph_info(font_t* font, u16 wide_char, font_glyph_info_t* out_info);
// void font_get_glyph_bitmap(font_t* font, void* out_bytes);

END_CPP_COMPATIBLE
