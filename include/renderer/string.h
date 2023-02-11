
#pragma once

#include <renderer/defines.h>


typedef struct string_t
{
	memory_allocator_t* allocator;
	char* data;			// ptr to the string data
	u32 length;			// length of the string (exclusive of the null character)
} string_t;

/*
	description: creates a string (memory allocation)
	params:
		str: constant string (ptr to the characters)
	returns:
		string_t, string_t object
 */
RENDERER_API string_t string_create(memory_allocator_t* allocator, const char* str);

static FORCE_INLINE string_t string_null() { return (string_t) { NULL, 0 }; }

/*
	description: destroys a string (deallocation)
	params:
		str: ptr to the string_t object to be destroyed
	returns:
		nothing
 */
RENDERER_API void string_destroy(string_t* str);
