
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

void vulkan_buffer_init(vulkan_buffer_t* buffer)
{
	memset(buffer, 0, sizeof(vulkan_buffer_t));
}

vulkan_buffer_t* vulkan_buffer_new()
{
	vulkan_buffer_t* buffer = heap_new(vulkan_buffer_t);
	vulkan_buffer_init(buffer);
	return buffer;
}


vulkan_buffer_t* vulkan_buffer_create(renderer_t* renderer, vulkan_buffer_create_info_t* create_info)
{
	vulkan_buffer_t* buffer = vulkan_buffer_new();
	vulkan_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

void vulkan_buffer_create_no_alloc(renderer_t* renderer, vulkan_buffer_create_info_t* create_info, vulkan_buffer_t* buffer)
{
	assert(renderer != NULL);
	assert(buffer != NULL);
	assert(((create_info->stride != 0) && (create_info->count != 0)) || (create_info->size != 0));

	u32 buffer_size = (create_info->size == 0) ? (create_info->stride * create_info->count) : create_info->size;
	buffer->handle = vk_get_buffer(renderer->vk_device, buffer_size, create_info->usage_flags, create_info->sharing_mode);
	buffer->memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, buffer->handle, create_info->memory_property_flags);
	buffer->stride = create_info->stride;
	buffer->count = create_info->count;
	buffer->size = buffer_size;
	if(create_info->data != NULL)
		vulkan_buffer_copy_data(buffer, renderer, create_info->data, 0, buffer->size);
}

void vulkan_buffer_destroy(vulkan_buffer_t* buffer, renderer_t* renderer)
{
	assert(renderer != NULL);
	assert(buffer != NULL);

	vkDestroyBuffer(renderer->vk_device, buffer->handle, NULL);
	vkFreeMemory(renderer->vk_device, buffer->memory, NULL);
}

void vulkan_buffer_release_resources(vulkan_buffer_t* buffer)
{
	assert(buffer != NULL);
	heap_free(buffer);
}

void vulkan_buffer_copy_data(vulkan_buffer_t* buffer, renderer_t* renderer, void* data, u32 start_offset, u32 size)
{
	assert(buffer != NULL);
	assert(data != NULL);
	assert((start_offset + size) <= buffer->size);
	assert_wrn(size != 0);

	void* ptr;
	vkMapMemory(renderer->vk_device, buffer->memory, 0, buffer->size, 0, &ptr);
	memcpy(ptr, data + start_offset, size);
	vkUnmapMemory(renderer->vk_device, buffer->memory);
}

void* vulkan_buffer_map(vulkan_buffer_t* buffer, renderer_t* renderer)
{
	assert(buffer != NULL);
	assert_wrn(buffer->size != 0);
	void* ptr;
	vkMapMemory(renderer->vk_device, buffer->memory, 0, buffer->size, 0, &ptr);
	return ptr;
}

void vulkan_buffer_unmap(vulkan_buffer_t* buffer, renderer_t* renderer)
{
	vkUnmapMemory(renderer->vk_device, buffer->memory);
}
