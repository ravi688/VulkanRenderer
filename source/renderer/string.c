
#include <renderer/string.h>
#include <renderer/memory_allocator.h>

RENDERER_API string_t string_create(const char* str)
{
	u32 len = strlen(str);
	string_t string = 
	{
		.data = heap_newv(char, len + 1),
		.len = len
	};
	strcpy(string.data, str);
	return string;
}

RENDERER_API void string_destroy(string_t* str)
{
	if(str->data == NULL) return;

	heap_free(str->data)
	str->data = NULL;
	str->len = 0;
}
