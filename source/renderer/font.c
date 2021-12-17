
#include <renderer/font.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <disk_manager/file_reader.h>


font_t* font_load_and_create(const char* file_name)
{
	BUFFER* file_data = load_binary_from_file(file_name);
	font_t* font = font_create(file_data->bytes, file_data->element_count);
	buf_free(file_data);
	return font;
}

font_t* font_create(void* bytes, u64 length)
{
	font_t* font = heap_new(font_t);
	memset(font, 0, sizeof(font_t));
	return font;
}


void font_destroy(font_t* font)
{

}


void font_release_resources(font_t* font)
{
	heap_free(font);
}

