/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: buffer2d.h is a part of VulkanRenderer

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
#include <bufferlib/buffer.h> // buffer_t
#include <common/hash_table.h> // hash_table_t, comparer_t, hash_function_t
#include <sge/buffer2d_view.h> // buffer2d_view_t, isize2d_t, ioffset2d_t
#include <sge/rect.h> 			// irect2d_t
#include <sge/object.h>

#ifdef GLOBAL_DEBUG
#	include <sge/color.h> // color3_t
#endif

typedef enum buffer2d_resize_mode_t
{
	/* number of rows is allowed to increase */
	BUFFER2D_RESIZE_MODE_ROW_RELAXED,
	/* number of columns is allowed to increase */
	BUFFER2D_RESIZE_MODE_COLUMN_RELAXED,
	/* rows and columns both should be resized such that the aspect ratio remains constant */
	BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT,
	/* on average, the aspect ratio should remain same (or approx same) */
	BUFFER2D_RESIZE_MODE_ASPECT_RATIO_RELAXED
} buffer2d_resize_mode_t;

typedef struct buffer2d_create_info_t
{
	/* capacity of this buffer in each dimension, if either is zero then no allocation would be done initially */
	iextent2d_t size;
	/* optional user provided backed buffer, otherwise it will be created internally */
	buffer_t* buffer;
	/* optinal user provided 2d view */
	buffer2d_view_t* view;
	/* resize mode of this buffer */
	buffer2d_resize_mode_t resize_mode;
	/* key size */
	u32 key_size;
	/* key equality comparer */
	comparer_t key_comparer;
	/* key hash function */
	hash_function_t key_hash_function;
} buffer2d_create_info_t;

typedef u32 rect2d_info_key_t;
#define RECT2D_INFO_KEY_INVALID U32_MAX

typedef struct rect2d_info_t
{
	/* position, and extent */
	irect2d_t rect;
	/* pre-calculated area of this rectangle */
	u32 area;
	/* pre-calculated aspect ratio of this rectangle */
	f32 aspect_ratio;
	/* child rects resulted from subividing this rectangle */
	u32 child_count;
} rect2d_info_t;

typedef struct filled_rect_info_t
{
	/* position, extent, area, and aspect ratio of the rectangle */
	rect2d_info_t rect_info;
	/* debugging color of this element */
	IF_DEBUG(icolor3_t color);
} filled_rect_info_t;

/* list of filled_rect_info_t objects which can be looked up in constant time */
typedef hash_table_t filled_rect_info_table_t;

/* list of rect2d_info_t objects which can be looked up in constant time */
typedef hash_table_t vacant_rect_info_table_t;

typedef void (*packed_rect_relocate_callback_t)(const rect2d_info_t* before, const rect2d_info_t* after, void* user_data);

typedef struct buffer2d_t
{
	__OBJECT__;
	memory_allocator_t* allocator;

	#if DBG_ENABLED(BUFFER2D_RESIZE)
		packed_rect_relocate_callback_t packed_rect_relocate_callback;
		void* packed_rect_relocate_callback_handler_user_data;
	#endif /* DBG_BUFFER2D_RESIZE */

	/* linear buffer */
	buffer_t* buffer;
	/* 2d view of the linear buffer */
	buffer2d_view_t* view;
	/* true if the backed buffer is created by buffer2d itself, false if supplied by the user */
	bool is_backed_buffer_owner;
	/* true if the 2d view is created by buffer2d itself, false if supplied by the user */
	bool is_view_owner;
	/* resize mode of this buffer */
	buffer2d_resize_mode_t resize_mode;

	/* list of entries, which can be looked up in constant time with their keys */
	filled_rect_info_table_t filled_rects;
	/* list of rectangles representing empty spaces */
	vacant_rect_info_table_t vacant_rects;

	/* internal */
	u32 counter;
} buffer2d_t;

typedef buffer2d_t* buffer2d_ptr_t;

#define BUFFER2D(ptr) OBJECT_UP_CAST(buffer2d_t*, OBJECT_TYPE_BUFFER2D, ptr)
#define BUFFER2D_CONST(ptr) OBJECT_UP_CAST_CONST(const buffer2d_t*, OBJECT_TYPE_BUFFER2D, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors and destructurs */
SGE_API buffer2d_t* buffer2d_new(memory_allocator_t* allocator);
SGE_API buffer2d_t* buffer2d_create(memory_allocator_t* allocator, buffer2d_create_info_t* create_info);
SGE_API void buffer2d_create_no_alloc(memory_allocator_t* allocator, buffer2d_create_info_t* create_info, buffer2d_t OUT buffer);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void buffer2d_create_no_alloc_ext(memory_allocator_t* allocator, buffer2d_create_info_t* create_info, buffer2d_t OUT buffer)
{
	OBJECT_INIT(buffer, OBJECT_TYPE_BUFFER2D, OBJECT_NATIONALITY_EXTERNAL);
	buffer2d_create_no_alloc(allocator, create_info, buffer);
}
SGE_API void buffer2d_destroy(buffer2d_t* buffer);
SGE_API void buffer2d_release_resources(buffer2d_t* buffer);

/* returns pointer to the backed buffer */
SGE_API buffer_t* buffer2d_get_backed_buffer(buffer2d_t* buffer);
/* returns pointer to the backed buffer which must not be modified! */
SGE_API buffer_t* buffer2d_get_backed_buffer_readonly(buffer2d_t* buffer);
/* returns filled rectangle information with key 'key' */
SGE_API filled_rect_info_t* buffer2d_get_rect(buffer2d_t* buffer, void* key);
/* fills 'out_data' with the data contained by 'rect_info' */
SGE_API void buffer2d_get_rect_data(buffer2d_t* buffer, filled_rect_info_t* rect_info, void* out_data);
#ifdef GLOBAL_DEBUG
SGE_API bool buffer2d_push_debug(buffer2d_t* buffer, void* key, void* value, u32 width, u32 height);
#endif /* GLOBAL_DEBUG */
/* creates a buffer2d element and adds a value with a key and size (width, height)
 * returns true if the size has been resized, otherwise false */
SGE_API bool buffer2d_push(buffer2d_t* buffer, void* key, void* value, u32 width, u32 height PARAM_IF_DEBUG(icolor3_t color));
/* creates a buffer2d element and broadcasts a value of size 'size' to the entire rect with a key
 * returns true if the size has been resized, otherwise false */
SGE_API bool buffer2d_push_broadcast(buffer2d_t* buffer, void* key, void* value, u32 size, u32 width, u32 heigh PARAM_IF_DEBUG(icolor3_t color));
/* issues warning if the resize doesn't comply with the 'resize_mode' specified while creating this buffer */
SGE_API void buffer2d_resize(buffer2d_t* buffer, u32 num_rows, u32 num_columns);
/* clears the entire buffer */
SGE_API void buffer2d_clear(buffer2d_t* buffer, void* clear_value);
/* returns (filled area) / (total area) */
SGE_API f32 buffer2d_get_packing_efficiency(buffer2d_t* buffer);

#ifdef GLOBAL_DEBUG
SGE_API void buffer2d_dump(buffer2d_t* buffer, const char* file_name);
#endif /* GLOBAL_DEBUG */

#if DBG_ENABLED(BUFFER2D_RESIZE)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void buffer2d_set_packed_rect_relocate_callback_handler(buffer2d_t* buffer, packed_rect_relocate_callback_t handler, void* user_data)
{
	buffer->packed_rect_relocate_callback = handler;
	buffer->packed_rect_relocate_callback_handler_user_data = user_data;
}
#endif /* DBG_BUFFER_RESIZE */

END_CPP_COMPATIBLE
