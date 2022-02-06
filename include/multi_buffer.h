
#pragma once

#include <bufferlib/buffer.h>
#include <renderer/defines.h>

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

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API void multi_buffer_create(u32 element_size, u32 capacity, multi_buffer_t* out_buffer);
RENDERER_API void multi_buffer_free(multi_buffer_t* buffer);

// getters
RENDERER_API buf_ucount_t multi_buffer_get_count(multi_buffer_t* buffer);
RENDERER_API buf_ucount_t multi_buffer_get_capacity(multi_buffer_t* buffer);
RENDERER_API buf_ucount_t multi_buffer_get_sub_buffer_count(multi_buffer_t* buffer);


// logic functions
RENDERER_API void multi_buffer_clear(multi_buffer_t* buffer);

// sub buffer

// constructors and destructors
RENDERER_API sub_buffer_handle_t multi_buffer_sub_buffer_create(multi_buffer_t* buffer, buf_ucount_t capacity);
RENDERER_API void multi_buffer_sub_buffer_destroy(multi_buffer_t* buffer, sub_buffer_handle_t handle); /*TODO*/
#define sub_buffer_create multi_buffer_sub_buffer_create
#define sub_buffer_destroy multi_buffer_sub_buffer_destroy

// logic functions
RENDERER_API void multi_buffer_sub_buffer_push(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
RENDERER_API void multi_buffer_sub_buffer_push_back(multi_buffer_t* buffer, sub_buffer_handle_t handle, void* in_value);
RENDERER_API void multi_buffer_sub_buffer_clear(multi_buffer_t* buffer, sub_buffer_handle_t handle);
#define sub_buffer_push multi_buffer_sub_buffer_push
#define sub_buffer_push_back multi_buffer_sub_buffer_push_back
#define sub_buffer_clear multi_buffer_sub_buffer_clear

// setters
RENDERER_API void multi_buffer_sub_buffer_set_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value);
#define sub_buffer_set_at multi_buffer_sub_buffer_set_at

// getters
RENDERER_API buf_ucount_t multi_buffer_sub_buffer_get_count(multi_buffer_t* buffer, sub_buffer_handle_t handle);
RENDERER_API buf_ucount_t multi_buffer_sub_buffer_get_capacity(multi_buffer_t* buffer, sub_buffer_handle_t handle);
RENDERER_API void multi_buffer_sub_buffer_get_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* out_value);
RENDERER_API void* multi_buffer_sub_buffer_get_ptr_at(multi_buffer_t* buffer, sub_buffer_handle_t handle, buf_ucount_t index);
#define sub_buffer_get_count multi_buffer_sub_buffer_get_count
#define sub_buffer_get_capacity multi_buffer_sub_buffer_get_capacity
#define sub_buffer_get_at multi_buffer_sub_buffer_get_at
#define sub_buffer_get_ptr_at multi_buffer_sub_buffer_get_ptr_at
#define sub_buffer_get_ptr(buffer, handle) multi_buffer_sub_buffer_get_ptr_at(buffer, handle, 0)

END_CPP_COMPATIBLE
