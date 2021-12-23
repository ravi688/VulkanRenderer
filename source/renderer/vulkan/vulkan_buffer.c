
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

vulkan_buffer_t* vulkan_buffer_new()
{
	vulkan_buffer_t* buffer = heap_new(vulkan_buffer_t);
	memset(buffer, 0, sizeof(vulkan_buffer_t));
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
	assert(create_info->data != NULL);
	assert(create_info->stride != 0);
	assert(create_info->count != 0);

	u32 buffer_size = create_info->stride * create_info->count;
	buffer->handle = vk_get_buffer(renderer->vk_device, buffer_size, create_info->usage_flags, create_info->sharing_mode);
	buffer->memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, buffer->handle, buffer_size, create_info->memory_property_flags);
	void* ptr;
	vkMapMemory(renderer->vk_device, buffer->memory, 0, buffer_size, 0, &ptr);
	memcpy(ptr, create_info->data, buffer_size);
	vkUnmapMemory(renderer->vk_device, buffer->memory);
	buffer->stride = create_info->stride;
	buffer->count = create_info->count;
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
