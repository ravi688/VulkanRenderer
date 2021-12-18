
#include <renderer/font.h>
#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <disk_manager/file_reader.h>

#include <ttf2mesh.h>


font_t* font_load_and_create(const char* file_name)
{
	assert(file_name != NULL);
	BUFFER* file_data = load_binary_from_file(file_name);
	font_t* font = font_create(file_data->bytes, file_data->element_count);
	buf_free(file_data);
	return font;
}

font_t* font_create(void* bytes, u64 length)
{
	assert(bytes != NULL);
	assert(length != 0);
	font_t* font = heap_new(font_t);
	memset(font, 0, sizeof(font_t));
	int result = ttf_load_from_mem(bytes, length, &font->handle, false);
	if((result != 0) || (font->handle == NULL))
	{
		LOG_FETAL_ERR("Failed to create font, bytes = %p, length = %u\n", bytes, length);
	}
	return font;
}

void font_destroy(font_t* font)
{
	assert(font != NULL);
	ttf_free(refp(font_t, font, 0)->handle);
}


void font_release_resources(font_t* font)
{
	heap_free(font);
}

