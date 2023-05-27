/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: color.h is a part of VulkanRenderer

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

typedef union color_t
{
	struct
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
} color_t;

typedef u32 color_hex_t;

#define COLOR_WHITE_HEX 	0xFFFFFFFFUL
#define COLOR_BLACK_HEX 	0x000000FFUL
#define COLOR_GREEN_HEX		0x00FF00FFUL
#define COLOR_BLUE_HEX 		0x0000FFFFUL
#define COLOR_RED_HEX 		0xFF0000FFUL
#define COLOR_GREY_HEX 		0x7F7F7FFFUL
#define COLOR_YELLOW_HEX 	0xFFFF00FFUL
#define COLOR_ORANGE_HEX 	0xFFA500FFUL
#define COLOR_VIOLET_HEX 	0x8A2BE2FFUL
#define COLOR_BROWN_HEX 	0x794044FFUL
#define COLOR_PINK_HEX 		0xFF1493FFUL

#define COLOR_WHITE 	color_hex(COLOR_WHITE_HEX)
#define COLOR_BLACK 	color_hex(COLOR_BLACK_HEX)
#define COLOR_GREEN 	color_hex(COLOR_GREEN_HEX)
#define COLOR_BLUE 		color_hex(COLOR_BLUE_HEX)
#define COLOR_RED 		color_hex(COLOR_RED_HEX)
#define COLOR_GREY 		color_hex(COLOR_GREY_HEX)
#define COLOR_YELLOW 	color_hex(COLOR_YELLOW_HEX)
#define COLOR_ORANGE 	color_hex(COLOR_ORANGE_HEX)
#define COLOR_VIOLET 	color_hex(COLOR_VIOLET_HEX)
#define COLOR_BROWN 	color_hex(COLOR_BROWN_HEX)
#define COLOR_PINK 		color_hex(COLOR_PINK_HEX)

static FORCE_INLINE color_t color(float r, float g, float b, float a) { return (color_t) { r, g, b, a }; }

static FORCE_INLINE u8 color_hex_get(color_hex_t hex, u8 index) { return (hex & (0xFF << index)) >> index; }
static FORCE_INLINE u8 color_hex_r(color_hex_t hex) { return color_hex_get(hex, 24); }
static FORCE_INLINE u8 color_hex_g(color_hex_t hex) { return color_hex_get(hex, 16); }
static FORCE_INLINE u8 color_hex_b(color_hex_t hex) { return color_hex_get(hex, 8); }
static FORCE_INLINE u8 color_hex_a(color_hex_t hex) { return color_hex_get(hex, 0); }
static FORCE_INLINE color_t color_hex(color_hex_t hex)
{
	float t = 1 / (float)0xFF;
	return color(color_hex_r(hex) * t, color_hex_g(hex) * t, color_hex_b(hex) * t, color_hex_a(hex) * t);
}
