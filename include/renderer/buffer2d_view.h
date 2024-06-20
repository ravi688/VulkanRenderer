/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: buffer2d_view.h is a part of VulkanRenderer

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
#include <bufferlib/buffer.h>
#include <renderer/rect.h> // iextent2d_t
#include <renderer/object.h>

typedef struct buffer2d_view_create_info_t
{
	/* size of the view */
	iextent2d_t size;
	/* linear buffer (it should be a valid pointer to a buffer_t object) */
	buffer_t* buffer;
} buffer2d_view_create_info_t;

typedef struct buffer2d_view_t
{
	__OBJECT__;
	memory_allocator_t* allocator;
	buffer_t* backed_buffer;
	iextent2d_t size;
} buffer2d_view_t;

typedef buffer2d_view_t* buffer2d_view_ptr_t;

#define BUFFER2D_VIEW(ptr) OBJECT_UP_CAST(buffer2d_view_t*, OBJECT_TYPE_BUFFER2D_VIEW, ptr)
#define BUFFER2D_VIEW_CONST(ptr) OBJECT_UP_CAST_CONST(const buffer2d_view_t*, OBJECT_TYPE_BUFFER2D_VIEW, ptr)


BEGIN_CPP_COMPATIBLE

/* constructor and destructures */
RENDERER_API buffer2d_view_t* buffer2d_view_new(memory_allocator_t* allocator);
RENDERER_API buffer2d_view_t* buffer2d_view_create(memory_allocator_t* allocator, buffer2d_view_create_info_t* create_info);
RENDERER_API void buffer2d_view_create_no_alloc(memory_allocator_t* allocator, buffer2d_view_create_info_t* create_info, buffer2d_view_t OUT view);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void buffer2d_view_create_no_alloc_ext(memory_allocator_t* allocator, buffer2d_view_create_info_t* create_info, buffer2d_view_t OUT view)
{
	OBJECT_INIT(view, OBJECT_TYPE_BUFFER2D_VIEW, OBJECT_NATIONALITY_EXTERNAL);
	buffer2d_view_create_no_alloc(allocator, create_info, view);
}
RENDERER_API void buffer2d_view_destroy(buffer2d_view_t* view);
RENDERER_API void buffer2d_view_release_resources(buffer2d_view_t* view);

RENDERER_API void buffer2d_view_set_buffer(buffer2d_view_t* view, buffer_t* buffer);
RENDERER_API void buffer2d_view_resize(buffer2d_view_t* view, u32 width, u32 height);
RENDERER_API void buffer2d_view_clear(buffer2d_view_t* view, void* clear_value);

/* getters */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer_t* buffer2d_view_get_backed_buffer(buffer2d_view_t* view) { return view->backed_buffer; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE iextent2d_t buffer2d_view_get_size(buffer2d_view_t* view) { return view->size; }
RENDERER_API void buffer2d_view_get_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* out_data);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void buffer2d_view_get_rect(buffer2d_view_t* view, irect2d_t rect, void* out_data)
{
	buffer2d_view_get_at(view, rect.offset.x, rect.offset.y, rect.extent.x, rect.extent.y, out_data);
}

/* setters */
RENDERER_API void buffer2d_view_set_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* in_data);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void buffer2d_view_set_rect(buffer2d_view_t* view, irect2d_t rect, void* in_data)
{
	buffer2d_view_set_at(view, rect.offset.x, rect.offset.y, rect.extent.x, rect.extent.y, in_data);
}
RENDERER_API void buffer2d_view_broadcast_rect(buffer2d_view_t* view, irect2d_t rect, void* in_value, u32 in_value_size);

END_CPP_COMPATIBLE
