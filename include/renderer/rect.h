/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: rect.h is a part of VulkanRenderer

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

#include <renderer/defines.h>

typedef union extent2d_t
{
	struct 
	{
		f32 x;
		f32 y;
	};

	struct
	{
		f32 width;
		f32 height;
	};
} extent2d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE extent2d_t extent2d(f32 x, f32 y) { return (extent2d_t) { x, y }; }

typedef extent2d_t offset2d_t;
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE offset2d_t offset2d(f32 x, f32 y) { return (offset2d_t) { x, y }; }

typedef struct rect2d_t
{
	offset2d_t offset;
	extent2d_t extent;
} rect2d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE rect2d_t rect2d(offset2d_t offset, extent2d_t extent) { return (rect2d_t) { offset, extent }; }

typedef union extent3d_t
{
	struct
	{
		f32 x;
		f32 y;
		f32 z;
	};

	struct
	{
		f32 width;
		f32 height;
		f32 depth;
	};
} extent3d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE extent3d_t extent3d(f32 x, f32 y, f32 z) { return (extent3d_t) { x, y, z }; }

typedef extent3d_t offset3d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE offset3d_t offset3d(f32 x, f32 y, f32 z) { return (offset3d_t) { x, y, z }; }

typedef struct rect3d_t
{
	offset3d_t offset;
	extent3d_t extent;
} rect3d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE rect3d_t rect3d(offset3d_t offset, extent3d_t extent) { return (rect3d_t) { offset, extent }; }

typedef union iextent2d_t
{
	struct
	{
		u32 x;
		u32 y;
	};

	struct
	{
		u32 width;
		u32 height;
	};
} iextent2d_t;

typedef iextent2d_t isize2d_t;
typedef iextent2d_t ioffset2d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE iextent2d_t iextent2d(u32 width, u32 height) { return (iextent2d_t) { width, height }; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE ioffset2d_t ioffset2d(u32 x, u32 y) { return iextent2d(x, y); }

#define IEXTENT2D_NULL (iextent2d_t) { U32_MAX, U32_MAX }
#define IOFFSET2D_NULL IEXTENT2D_NULL

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool iextent2d_is_null(iextent2d_t extent)
{
	return (extent.width == U32_MAX) && (extent.height == U32_MAX);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool ioffset2d_is_null(ioffset2d_t offset) { return iextent2d_is_null((iextent2d_t)offset); }

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 IEXTENT2D_AREA(iextent2d_t extent) { return extent.width * extent.height; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE f32 IEXTENT2D_ASPECT_RATIO(iextent2d_t extent) { return (f32)extent.width / extent.height; }

typedef struct irect2d_t
{
	ioffset2d_t offset;
	iextent2d_t extent;
} irect2d_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE irect2d_t irect2d(ioffset2d_t offset, iextent2d_t extent) { return (irect2d_t) { offset, extent }; }

#define IRECT2D_AREA(rect) IEXTENT2D_AREA((rect).extent)
#define IRECT2D_ASPECT_RATIO(rect) IEXTENT2D_ASPECT_RATIO((rect).extent)
#define IRECT2D_NULL (irect2d_t) { IOFFSET2D_NULL, IEXTENT2D_NULL }

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool irect2d_is_null(irect2d_t rect)
{
	return ioffset2d_is_null(rect.offset) && iextent2d_is_null(rect.extent);
}

