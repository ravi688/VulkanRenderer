#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_new(memory_allocator_t* allocator)
{
	vulkan_host_buffered_buffer_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_HOST_BUFFERED_BUFFER, vulkan_host_buffered_buffer_t);
	memzero(buffer, vulkan_host_buffered_buffer_t);
	return buffer;
}

RENDERER_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_create(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info)
{
	vulkan_host_buffered_buffer_t* buffer = vulkan_host_buffered_buffer_new(renderer->allocator);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

RENDERER_API void vulkan_host_buffered_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info, vulkan_host_buffered_buffer_t OUT buffer)
{
	memzero(buffer, vulkan_host_buffered_buffer_t);
	buffer->renderer = renderer;
	/* if capacity is non zero (> 0) then create a GPU side buffer */
	if(create_info->capacity > 0)
	{
		vulkan_buffer_create_info_t buffer_create_info = 
		{
			.count = create_info->capacity,
			.stride = create_info->stride,
			.vo_usage_flags = create_info->vo_usage_flags,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		vulkan_buffer_create_no_alloc(renderer, &buffer_create_info, &buffer->device_buffer);
		buffer->has_device_buffer = true;
	}
	buffer->vo_usage_flags = create_info->vo_usage_flags;
	/* create a host side buffer */
	buffer->host_buffer = buf_create(create_info->stride, create_info->capacity, 0);
}

RENDERER_API void vulkan_host_buffered_buffer_destroy(vulkan_host_buffered_buffer_t* buffer)
{
	if(buffer->has_device_buffer)
	{
		vulkan_buffer_destroy(&buffer->device_buffer);
		vulkan_buffer_release_resources(&buffer->device_buffer);
		buffer->has_device_buffer = false;
	}
	buf_clear(&buffer->host_buffer, NULL);
}

RENDERER_API void vulkan_host_buffered_buffer_release_resources(vulkan_host_buffered_buffer_t* buffer)
{
	buf_free(&buffer->host_buffer);
}


RENDERER_API buffer_t* vulkan_host_buffered_buffer_get_host_buffer(vulkan_host_buffered_buffer_t* buffer)
{
	return &buffer->host_buffer;
}

RENDERER_API vulkan_buffer_t* vulkan_host_buffered_buffer_get_device_buffer(vulkan_host_buffered_buffer_t* buffer)
{
	return buffer->has_device_buffer ? &buffer->device_buffer : NULL;
}


RENDERER_API bool vulkan_host_buffered_buffer_commit(vulkan_host_buffered_buffer_t* buffer)
{
	buffer_t* host_buffer = vulkan_host_buffered_buffer_get_host_buffer(buffer);
	vulkan_buffer_t* device_buffer = vulkan_host_buffered_buffer_get_device_buffer(buffer);
	
	u32 size = buf_get_element_count(host_buffer);

	if(size == 0)
		/* no update on the gpu side buffer */
		return false;

	/* if there is no vulkan_buffer constructed or the size of the vulkan_buffer is less than that of the host buffer, then reconstruct it. */
	if((!buffer->has_device_buffer) || (buffer->has_device_buffer && (size > vulkan_buffer_get_size(device_buffer))))
	{
		/* destory the previously constructed vulkan buffer object */
		if(buffer->has_device_buffer)
		{
			_debug_assert__(vulkan_buffer_get_size(device_buffer) != 0);
			vulkan_buffer_destroy(device_buffer);
		}

		/* create a new vulkan buffer object */
		vulkan_buffer_create_info_t create_info = 
		{
			.size = size,
			.vo_usage_flags = buffer->vo_usage_flags,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	
		};
		vulkan_buffer_create_no_alloc(buffer->renderer, &create_info, device_buffer);
		buffer->has_device_buffer = true;
	}

	_debug_assert__(buffer->has_device_buffer == true);
	
	/* copy the host side buffer to the GPU side buffer */
	_debug_assert__(buf_get_element_size(host_buffer) == sizeof(u8));
	vulkan_buffer_copy_data(device_buffer, 0, buf_get_ptr(host_buffer), buf_get_element_count(host_buffer));

	/* updated the gpu side buffer */
	return true;
}

