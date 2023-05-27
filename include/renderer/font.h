/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: font.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
