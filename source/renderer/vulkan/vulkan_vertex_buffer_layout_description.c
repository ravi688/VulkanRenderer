/* here order matters, as vulkan_vertex_buffer_layout_description.h defines only VkFormat if vulkan/vulkan.h is not included,
 * but if it is include then the inclusion of vulkan/vulkan.h must come first to let the header define VULKAN_CORE_H_ 
 * here vulkan/vulkan_renderer.h includes vulkan/vulkan.h under the hood, hence it should come first. */
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>

#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <bufferlib/buffer.h>

#define create_buffer(allocator, type) __create_buffer(allocator, sizeof(type))
static BUFFER* __create_buffer(memory_allocator_t* allocator,  u32 size)
{
	BUFFER* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_BUFFER, BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

RENDERER_API void vulkan_vertex_buffer_layout_description_begin(vulkan_renderer_t* renderer,  vulkan_vertex_buffer_layout_description_t* description, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	description->binding = binding_number;
	description->input_rate = input_rate;
	description->size = stride;

	description->attribute_locations = CAST_TO(u16*, create_buffer(renderer->allocator, u16));
	description->attribute_formats = CAST_TO(VkFormat*, create_buffer(renderer->allocator, VkFormat));
	description->attribute_offsets = CAST_TO(u32*, create_buffer(renderer->allocator, u32));
}

RENDERER_API void vulkan_vertex_buffer_layout_description_add_attribute(vulkan_vertex_buffer_layout_description_t* description, u16 location, VkFormat format, u32 offset)
{
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_locations), location);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_formats), format);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_offsets), offset);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_end(vulkan_renderer_t* renderer,  vulkan_vertex_buffer_layout_description_t* description)
{
	BUFFER* buffer = CAST_TO(BUFFER*, description->attribute_locations);
	description->attribute_count = buf_get_element_count(buffer);
	description->attribute_locations = buf_get_ptr(buffer);
	memory_allocator_dealloc(renderer->allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_formats);
	description->attribute_formats = buf_get_ptr(buffer);
	memory_allocator_dealloc(renderer->allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_offsets);
	description->attribute_offsets = buf_get_ptr(buffer);
	memory_allocator_dealloc(renderer->allocator, buffer);
}
