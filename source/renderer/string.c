
#include <renderer/string.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

#include <string.h>

RENDERER_API string_t string_create(memory_allocator_t* allocator, const char* str)
{
	u32 len = strlen(str);
	string_t string = 
	{
		.data = CAST_TO(char*, memory_allocator_alloc(allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_CHAR_BUFFER, sizeof(char) * (len + 1))),
		.length = len,
		.allocator = allocator
	};
	strcpy(string.data, str);
	return string;
}

RENDERER_API void string_destroy(string_t* str)
{
	if(str->data == NULL) return;

	memory_allocator_dealloc(str->allocator, str->data);
	str->data = NULL;
	str->length = 0;
}
