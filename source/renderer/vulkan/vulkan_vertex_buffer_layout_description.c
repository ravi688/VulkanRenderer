
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>
#include <renderer/alloc.h>
#include <bufferlib/buffer.h>

#define create_buffer(type) __create_buffer(sizeof(type))
static BUFFER* __create_buffer(u32 size)
{
	BUFFER* buffer = heap_new(BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

RENDERER_API void vulkan_vertex_buffer_layout_description_begin(vulkan_vertex_buffer_layout_description_t* description, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	description->binding = binding_number;
	description->input_rate = input_rate;
	description->size = stride;

	description->attribute_locations = CAST_TO(u16*, create_buffer(u16));
	description->attribute_formats = CAST_TO(VkFormat*, create_buffer(VkFormat));
	description->attribute_offsets = CAST_TO(u32*, create_buffer(u32));
}

RENDERER_API void vulkan_vertex_buffer_layout_description_add_attribute(vulkan_vertex_buffer_layout_description_t* description, u16 location, VkFormat format, u32 offset)
{
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_locations), location);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_formats), format);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_offsets), offset);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_end(vulkan_vertex_buffer_layout_description_t* description)
{
	BUFFER* buffer = CAST_TO(BUFFER*, description->attribute_locations);
	description->attribute_count = buf_get_element_count(buffer);
	description->attribute_locations = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_formats);
	description->attribute_formats = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_offsets);
	description->attribute_offsets = buf_get_ptr(buffer);
	heap_free(buffer);
}
