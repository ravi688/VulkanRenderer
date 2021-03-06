
#include <renderer/multi_buffer.h>
#include <renderer/assert.h>
#include <memory.h>

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(multi_buffer)
#	define check_handle(multi_buffer, handle)
#	define check_index(sub_buffer, index)
#else
	static void check_pre_condition(multi_buffer_t* multi_buffer);
	static void check_handle(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle);
#	define check_index(sub_buffer, index) assert((index) < (sub_buffer)->count)
#endif /*GLOBAL_DEBUG*/

inline static sub_buffer_t* get_sub_buffer(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_handle(multi_buffer, handle);
	return (sub_buffer_t*)buf_get_ptr_at(&multi_buffer->sub_buffers, handle);
}

inline static buf_ucount_t get_master_index(sub_buffer_t* sub_buffer, buf_ucount_t index)
{
	check_index(sub_buffer, index);
	return sub_buffer->ptr + index;
}

// constructors and destructors
RENDERER_API void multi_buffer_create(u32 element_size, u32 capacity, multi_buffer_t* out_multi_buffer)
{
	assert(out_multi_buffer != NULL);
	out_multi_buffer->buffer = buf_create(element_size, capacity, 0);
	out_multi_buffer->sub_buffers = buf_create(sizeof(sub_buffer_t), 1, 0);
}

RENDERER_API void multi_buffer_free(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	buf_free(&multi_buffer->buffer);
	buf_free(&multi_buffer->sub_buffers);
}

// getters
RENDERER_API buf_ucount_t multi_buffer_get_count(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	return buf_get_element_count(&multi_buffer->buffer);
}

RENDERER_API buf_ucount_t multi_buffer_get_capacity(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	return buf_get_capacity(&multi_buffer->buffer);
}

RENDERER_API buf_ucount_t multi_buffer_get_sub_buffer_count(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	return buf_get_element_count(&multi_buffer->sub_buffers);
}

// logic functions
RENDERER_API void multi_buffer_clear(multi_buffer_t* multi_buffer)
{
	check_pre_condition(multi_buffer);
	buf_clear(&multi_buffer->buffer, NULL);
	buf_clear(&multi_buffer->sub_buffers, NULL);
}

// sub buffer

// constructors and destructors
RENDERER_API sub_buffer_handle_t multi_buffer_sub_buffer_create(multi_buffer_t* multi_buffer, buf_ucount_t capacity)
{
	check_pre_condition(multi_buffer);
	BUFFER* sub_buffers = &multi_buffer->sub_buffers;
	BUFFER* buffer = &multi_buffer->buffer;

	// create a new sub_buffer_t instance
	buf_push_pseudo(sub_buffers, 1);
	sub_buffer_t* sub_buffer = buf_peek_ptr(sub_buffers);

	// set the ptr to current element count in the master buffer
	sub_buffer->ptr = buf_get_element_count(buffer);

	if(capacity != 0)
		// extend the master buffer to fit the newly created sub_buffer's capacity
		buf_push_pseudo(buffer, capacity);
	sub_buffer->capacity = capacity;
	
	// set the element count in the newly created sub_buffer to zero
	sub_buffer->count = 0;

	// return the index of the newly created sub_buffer in the sub_buffers buffer as a handle to it
	return buf_get_element_count(sub_buffers) - 1;
}

RENDERER_API void multi_buffer_sub_buffer_destroy(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle); /*TODO*/

// logic functions
RENDERER_API void multi_buffer_sub_buffer_push(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, void* in_value)
{
	check_pre_condition(multi_buffer);
	BUFFER* sub_buffers = &multi_buffer->sub_buffers;
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	
	// resize the master buffer if needed
	buf_ucount_t new_count = sub_buffer->count + 1;
	buf_ucount_t prev_capacity = sub_buffer->capacity;
	sub_buffer->capacity = (sub_buffer->capacity == 0) ? 1 : sub_buffer->capacity;
	if(sub_buffer->capacity < new_count)
		sub_buffer->capacity <<= 1; 		// multiply by 2
	if(prev_capacity != sub_buffer->capacity)
	{
		buf_ucount_t diff = sub_buffer->capacity - prev_capacity;
		buf_insert_pseudo(&multi_buffer->buffer, sub_buffer->ptr + sub_buffer->count, diff);
		// update the pointers of the sub_buffers to the new location
		for(buf_ucount_t i = handle + 1; i < buf_get_element_count(sub_buffers); i++)
			((sub_buffer_t*)buf_get_ptr_at(sub_buffers, i))->ptr += diff;
	}

	// push the value
	buf_set_at(&multi_buffer->buffer, sub_buffer->ptr + sub_buffer->count, in_value);
	sub_buffer->count++;
}

RENDERER_API void multi_buffer_sub_buffer_clear(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	sub_buffer_t* sub_buffer = get_sub_buffer(multi_buffer, handle);
	void* bytes = buf_get_ptr(&multi_buffer->buffer);
	memset(bytes + sub_buffer->ptr * buf_get_element_size(&multi_buffer->buffer), 0, buf_get_element_size(&multi_buffer->buffer) * sub_buffer->count);
	sub_buffer->count = 0;
}

// getters
RENDERER_API buf_ucount_t multi_buffer_sub_buffer_get_count(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	return get_sub_buffer(multi_buffer, handle)->count;
}

RENDERER_API buf_ucount_t multi_buffer_sub_buffer_get_capacity(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	check_pre_condition(multi_buffer);
	return get_sub_buffer(multi_buffer, handle)->capacity;
}

RENDERER_API void multi_buffer_sub_buffer_get_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* out_value)
{
	assert(out_value != NULL);
	check_pre_condition(multi_buffer);
	buf_get_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index), out_value);
}

RENDERER_API void* multi_buffer_sub_buffer_get_ptr_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index)
{
	check_pre_condition(multi_buffer);
	return buf_get_ptr_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index));
}

// setters
RENDERER_API void multi_buffer_sub_buffer_set_at(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle, buf_ucount_t index, void* in_value)
{
	assert(in_value != NULL);
	check_pre_condition(multi_buffer);
	buf_set_at(&multi_buffer->buffer, get_master_index(get_sub_buffer(multi_buffer, handle), index), in_value);
}

#ifdef GLOBAL_DEBUG
static void check_pre_condition(multi_buffer_t* multi_buffer)
{
	assert(multi_buffer != NULL);
}

static void check_handle(multi_buffer_t* multi_buffer, sub_buffer_handle_t handle)
{
	assert(handle < buf_get_element_count(&multi_buffer->sub_buffers));
	assert(handle != BUF_INVALID_INDEX);
}
#endif /*GLOBAL_DEBUG*/
