#pragma once

#include <shader_compiler/defines.h>

/* allocation callbacks made during the compilation process
 * one can initialize them with his/her user-defined allocators */
typedef struct sc_allocation_callbacks_t
{
	/* pointer to the user data which might be needed by 'allocate', 'reallocate', or 'deallocate' functions */
	void* user_data;
	/* called when allocation of certain size is needed */
	void* (*allocate)(void* user_data, u32 size, u32 align);
	/* called when reallocation of certain size is needed */
	void* (*reallocate)(void* user_data, void* old_ptr, u32 size, u32 align);	
	/* called when deallocation is needed for a memory block pointed by 'ptr' */
	void (*deallocate)(void* user_data, void* ptr);
} sc_allocation_callbacks_t;

static FORCE_INLINE void* sc_call_allocate_aligned(sc_allocation_callbacks_t* callbacks, u32 size, u32 align) { return callbacks->allocate(callbacks->user_data, size, align); }
static FORCE_INLINE void* sc_call_reallocate_aligned(sc_allocation_callbacks_t* callbacks, void* old_ptr, u32 size, u32 align) { return callbacks->reallocate(callbacks->user_data, old_ptr, size, align); }
static FORCE_INLINE void* sc_call_allocate(sc_allocation_callbacks_t* callbacks, u32 size) { return sc_call_allocate_aligned(callbacks, size, U32_MAX); }
static FORCE_INLINE void* sc_call_reallocate(sc_allocation_callbacks_t* callbacks, void* old_ptr, u32 size) { return  sc_call_reallocate_aligned(callbacks, old_ptr, size, U32_MAX); }
static FORCE_INLINE void sc_call_deallocate(sc_allocation_callbacks_t* callbacks, void* ptr) { return callbacks->deallocate(callbacks->user_data, ptr); }

#include <stdlib.h> /* for malloc, realloc and free */

static void* allocate(void* user_data, u32 size, u32 align) { return malloc(size); }
static void* reallocate(void* user_data, void* old_ptr, u32 size, u32 align) { return realloc(old_ptr, size); }
static void deallocate(void* user_data, void* ptr) { free(ptr); }

CAN_BE_UNUSED_FUNCTION static sc_allocation_callbacks_t sc_allocation_callbacks_get_std()
{
	return (sc_allocation_callbacks_t)
	{
		.allocate = allocate,
		.reallocate = reallocate,
		.deallocate = deallocate
	};
}

#include <bufferlib/buffer.h>
SC_API buffer_t buf_create_with_callbacks(sc_allocation_callbacks_t* callbacks, u32 element_size, u32 capacity, u32 offset);
SC_API BUFFER* BUFcreate_with_callbacks(sc_allocation_callbacks_t* callbacks, void* buffer, u32 element_size, u32 capacity, u32 offset);
