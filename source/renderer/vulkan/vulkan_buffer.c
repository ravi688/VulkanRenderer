
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <memory_allocator/memory_allocator.h>
#include <renderer/assert.h>

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_buffer_t* buffer);
#else
#	define check_pre_condition(buffer)
#endif /* GLOBAL_DEBUG */

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

	buffer->renderer = renderer;
	u32 buffer_size = (create_info->size == 0) ? (create_info->stride * create_info->count) : create_info->size;
	buffer->handle = vk_get_buffer(renderer->logical_device->handle, buffer_size, create_info->usage_flags, create_info->sharing_mode);
	buffer->memory = vk_get_device_memory_for_buffer(renderer->logical_device->handle, renderer->physical_device->handle, buffer->handle, create_info->memory_property_flags);
	buffer->stride = create_info->stride;
	buffer->count = create_info->count;
	buffer->size = buffer_size;
	if(create_info->data != NULL)
		vulkan_buffer_copy_data(buffer, 0, create_info->data, buffer->size);
}

void vulkan_buffer_destroy(vulkan_buffer_t* buffer)
{
	check_pre_condition(buffer);
	vkDestroyBuffer(buffer->renderer->logical_device->handle, buffer->handle, NULL);
	vkFreeMemory(buffer->renderer->logical_device->handle, buffer->memory, NULL);
}

void vulkan_buffer_release_resources(vulkan_buffer_t* buffer)
{
	check_pre_condition(buffer);
	heap_free(buffer);
}

void vulkan_buffer_copy_data(vulkan_buffer_t* buffer, u32 buffer_offset, void* data, u32 data_size)
{
	check_pre_condition(buffer);
	assert(data != NULL);
	assert_wrn(data_size != 0);
	assert((buffer_offset + data_size) <= buffer->size);

	void* ptr;
	vkMapMemory(buffer->renderer->logical_device->handle, buffer->memory, 0, buffer->size, 0, &ptr);
	memcpy(ptr + buffer_offset, data, data_size);
	vkUnmapMemory(buffer->renderer->logical_device->handle, buffer->memory);
}

void* vulkan_buffer_map(vulkan_buffer_t* buffer)
{
	check_pre_condition(buffer);
	assert_wrn(buffer->size != 0);
	void* ptr;
	vkMapMemory(buffer->renderer->logical_device->handle, buffer->memory, 0, buffer->size, 0, &ptr);
	return ptr;
}

void vulkan_buffer_unmap(vulkan_buffer_t* buffer)
{
	check_pre_condition(buffer);
	vkUnmapMemory(buffer->renderer->logical_device->handle, buffer->memory);
}



#ifdef GLOBAL_DEBUG

static void check_pre_condition(vulkan_buffer_t* buffer)
{
	assert(buffer != NULL);
	assert(buffer->renderer != NULL);
}

#endif /* GLOBAL DEBUG */
