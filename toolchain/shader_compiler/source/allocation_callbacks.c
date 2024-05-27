#include <shader_compiler/allocation_callbacks.h>

static void* buf_malloc_callback_handler(buf_ucount_t size, void* user_data)
{
	return sc_call_allocate(CAST_TO(sc_allocation_callbacks_t*, user_data), size);
}

static void buf_free_callback_handler(void* ptr, void* user_data)
{
	sc_call_deallocate(CAST_TO(sc_allocation_callbacks_t*, user_data), ptr);
}

static void* buf_realloc_callback_handler(void* old_ptr, buf_ucount_t size, void* user_data)
{
	return sc_call_reallocate(CAST_TO(sc_allocation_callbacks_t*, user_data), old_ptr, size);
}

SC_API buffer_t buf_create_with_callbacks(sc_allocation_callbacks_t* callbacks, u32 element_size, u32 capacity, u32 offset)
{
	return buf_create_a(element_size, capacity, offset, buf_malloc_callback_handler, buf_free_callback_handler, buf_realloc_callback_handler, callbacks);
}

SC_API BUFFER* BUFcreate_with_callbacks(sc_allocation_callbacks_t* callbacks, void* buffer, u32 element_size, u32 capacity, u32 offset)
{
	return BUFcreate_a(buffer, element_size, capacity, offset, buf_malloc_callback_handler, buf_free_callback_handler, buf_realloc_callback_handler, callbacks);
}
