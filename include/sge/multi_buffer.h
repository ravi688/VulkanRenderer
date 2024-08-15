/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: multi_buffer.h is a part of VulkanRenderer

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

#include <bufferlib/buffer.h>
#include <sge/defines.h>
#include <sge/comparer.h> // comparer_t

typedef struct sub_buffer_t
{
	buf_ucount_t ptr;
	buf_ucount_t capacity;
	buf_ucount_t count;
} sub_buffer_t;

typedef struct multi_buffer_t
{
	BUFFER buffer;			// contains contiguous memory for all the sub-buffers
	BUFFER /*sub_buffer_t*/ sub_buffers;
} multi_buffer_t;

typedef buf_ucount_t sub_buffer_handle_t;
#define SUB_BUFFER_HANDLE_INVALID BUF_INVALID_INDEX

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API void multi_buffer_create(u32 element_size, u32 capacity, multi_buffer_t* out_buffer);
SGE_API void multi_buffer_free(multi_buffer_t* buffer);

// getters
SGE_API buf_ucount_t multi_buffer_get_count(multi_buffer_t* buffer);
SGE_API buf_ucount_t multi_buffer_get_capacity(multi_buffer_t* buffer);
SGE_API buf_ucount_t multi_buffer_get_sub_buffer_count(multi_buffer_t* buffer);


// logic functions
SGE_API void multi_buffer_clear(multi_buffer_t* buffer);

// sub buffer

// constructors and destructors
SGE_API sub_buffer_handle_t multi_buffer_sub_buffer_create(multi_buffer_t* buffer, buf_ucount_t capacity);
SGE_API void multi_buffer_sub_buffer_destroy(multi_buffer_t* buffer, sub_buffer_handle_t handle); /*TODO*/
#define sub_buffer_create multi_buffer_sub_buffer_create
#define sub_buffer_destroy multi_buffer_sub_buffer_destroy

// logic functions
SGE_API void multi_buffer_sub_buffer_push(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
SGE_API void multi_buffer_sub_buffer_push_n(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value, u32 max_size);
SGE_API void multi_buffer_sub_buffer_push_back(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
SGE_API void multi_buffer_sub_buffer_clear(multi_buffer_t* buffer, sub_buffer_handle_t handle);
SGE_API buf_ucount_t multi_buffer_sub_buffer_find_index_of(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal);
SGE_API bool multi_buffer_sub_buffer_remove(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value, comparer_t is_equal);
#define sub_buffer_push multi_buffer_sub_buffer_push
#define sub_buffer_push_n multi_buffer_sub_buffer_push_n
#define sub_buffer_push_back multi_buffer_sub_buffer_push_back
#define sub_buffer_clear multi_buffer_sub_buffer_clear
#define sub_buffer_find_index_of multi_buffer_sub_buffer_find_index_of
#define sub_buffer_remove multi_buffer_sub_buffer_remove

// setters
SGE_API void multi_buffer_sub_buffer_set_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value);
SGE_API void multi_buffer_sub_buffer_set_at_n(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value, buf_ucount_t max_size);
#define sub_buffer_set_at multi_buffer_sub_buffer_set_at
#define sub_buffer_set_at_n multi_buffer_sub_buffer_set_at_n

// getters
SGE_API buf_ucount_t multi_buffer_sub_buffer_get_count(multi_buffer_t* buffer, sub_buffer_handle_t handle);
SGE_API buf_ucount_t multi_buffer_sub_buffer_get_capacity(multi_buffer_t* buffer, sub_buffer_handle_t handle);
SGE_API void multi_buffer_sub_buffer_get_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* out_value);
SGE_API void* multi_buffer_sub_buffer_get_ptr_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index);
#define sub_buffer_get_count multi_buffer_sub_buffer_get_count
#define sub_buffer_get_capacity multi_buffer_sub_buffer_get_capacity
#define sub_buffer_get_at multi_buffer_sub_buffer_get_at
#define sub_buffer_get_ptr_at multi_buffer_sub_buffer_get_ptr_at
#define sub_buffer_get_ptr(buffer, handle) sub_buffer_get_ptr_at(buffer, handle, 0)

END_CPP_COMPATIBLE
