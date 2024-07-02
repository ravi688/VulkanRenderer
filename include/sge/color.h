/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: color.h is a part of VulkanRenderer

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
#include <hpml/vec3.h>

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

	union
	{
		vec3_t xyz;
		vec3_t rgb;
	};
} color_t;

typedef u32 color_hex_t;

typedef color_t color4_t;
typedef union color3_t
{
	struct
	{
		float r;
		float g;
		float b;
	};

	struct
	{
		float x;
		float y;
		float z;
	};
} color3_t;

typedef union icolor4_t
{
	struct
	{
		u8 r;
		u8 g;
		u8 b;
		u8 a;
	};

	struct
	{
		u8 x;
		u8 y;
		u8 z;
		u8 w;
	};
} icolor4_t;

typedef union icolor3_t
{
	struct
	{
		u8 r;
		u8 g;
		u8 b;
	};

	struct
	{
		u8 x;
		u8 y;
		u8 z;
	};
} icolor3_t;

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

#define COLOR4_WHITE 	COLOR_WHITE
#define COLOR4_BLACK 	COLOR_BLACK
#define COLOR4_GREEN 	COLOR_GREEN
#define COLOR4_BLUE 	COLOR_BLUE 
#define COLOR4_RED 		COLOR_RED 
#define COLOR4_GREY 	COLOR_GREY 
#define COLOR4_YELLOW 	COLOR_YELLOW
#define COLOR4_ORANGE 	COLOR_ORANGE
#define COLOR4_VIOLET 	COLOR_VIOLET
#define COLOR4_BROWN 	COLOR_BROWN
#define COLOR4_PINK 	COLOR_PINK 

#define COLOR3_WHITE 	color3_hex(COLOR_WHITE_HEX)
#define COLOR3_BLACK 	color3_hex(COLOR_BLACK_HEX)
#define COLOR3_GREEN 	color3_hex(COLOR_GREEN_HEX)
#define COLOR3_BLUE 	color3_hex(COLOR_BLUE_HEX)
#define COLOR3_RED 		color3_hex(COLOR_RED_HEX)
#define COLOR3_GREY 	color3_hex(COLOR_GREY_HEX)
#define COLOR3_YELLOW 	color3_hex(COLOR_YELLOW_HEX)
#define COLOR3_ORANGE 	color3_hex(COLOR_ORANGE_HEX)
#define COLOR3_VIOLET 	color3_hex(COLOR_VIOLET_HEX)
#define COLOR3_BROWN 	color3_hex(COLOR_BROWN_HEX)
#define COLOR3_PINK 	color3_hex(COLOR_PINK_HEX)

#define ICOLOR4_WHITE 	icolor4_hex(COLOR_WHITE_HEX)
#define ICOLOR4_BLACK 	icolor4_hex(COLOR_BLACK_HEX)
#define ICOLOR4_GREEN 	icolor4_hex(COLOR_GREEN_HEX)
#define ICOLOR4_BLUE 	icolor4_hex(COLOR_BLUE_HEX)
#define ICOLOR4_RED 	icolor4_hex(COLOR_RED_HEX)
#define ICOLOR4_GREY 	icolor4_hex(COLOR_GREY_HEX)
#define ICOLOR4_YELLOW 	icolor4_hex(COLOR_YELLOW_HEX)
#define ICOLOR4_ORANGE 	icolor4_hex(COLOR_ORANGE_HEX)
#define ICOLOR4_VIOLET 	icolor4_hex(COLOR_VIOLET_HEX)
#define ICOLOR4_BROWN 	icolor4_hex(COLOR_BROWN_HEX)
#define ICOLOR4_PINK 	icolor4_hex(COLOR_PINK_HEX)

#define ICOLOR3_WHITE 	icolor3_hex(COLOR_WHITE_HEX)
#define ICOLOR3_BLACK 	icolor3_hex(COLOR_BLACK_HEX)
#define ICOLOR3_GREEN 	icolor3_hex(COLOR_GREEN_HEX)
#define ICOLOR3_BLUE 	icolor3_hex(COLOR_BLUE_HEX)
#define ICOLOR3_RED 	icolor3_hex(COLOR_RED_HEX)
#define ICOLOR3_GREY 	icolor3_hex(COLOR_GREY_HEX)
#define ICOLOR3_YELLOW 	icolor3_hex(COLOR_YELLOW_HEX)
#define ICOLOR3_ORANGE 	icolor3_hex(COLOR_ORANGE_HEX)
#define ICOLOR3_VIOLET 	icolor3_hex(COLOR_VIOLET_HEX)
#define ICOLOR3_BROWN 	icolor3_hex(COLOR_BROWN_HEX)
#define ICOLOR3_PINK 	icolor3_hex(COLOR_PINK_HEX)

static FORCE_INLINE color_t color(float r, float g, float b, float a) { return (color_t) { r, g, b, a }; }
static FORCE_INLINE icolor4_t icolor4(u8 r, u8 g, u8 b, u8 a) { return (icolor4_t) { r, g, b, a }; }
static FORCE_INLINE color3_t color3(float r, float g, float b) { return (color3_t) { r, g, b }; }
static FORCE_INLINE icolor3_t icolor3(u8 r, u8 g, u8 b) { return (icolor3_t) { r, g, b }; }

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
static FORCE_INLINE icolor4_t icolor4_hex(color_hex_t hex)
{
	return icolor4(color_hex_r(hex), color_hex_g(hex), color_hex_b(hex), color_hex_a(hex));
}
static FORCE_INLINE color3_t color3_hex(color_hex_t hex)
{
	float t = 1 / (float)0xFF;
	return color3(color_hex_r(hex) * t, color_hex_g(hex) * t, color_hex_b(hex) * t);
}
static FORCE_INLINE icolor3_t icolor3_hex(color_hex_t hex)
{
	return icolor3(color_hex_r(hex), color_hex_g(hex), color_hex_b(hex));
}
