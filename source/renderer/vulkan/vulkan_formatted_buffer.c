#include <renderer/internal/vulkan/vulkan_formatted_buffer.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>
#include <renderer/struct_descriptor.h>

RENDERER_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_new(memory_allocator_t* allocator)
{
	vulkan_formatted_buffer_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_FORMATTED_BUFFER, vulkan_formatted_buffer_t);
	memzero(buffer, vulkan_formatted_buffer_t);
	VULKAN_OBJECT_INIT(buffer, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return buffer;
}

RENDERER_API void vulkan_formatted_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info, vulkan_formatted_buffer_t* buffer)
{
	VULKAN_OBJECT_MEMZERO(buffer, vulkan_formatted_buffer_t);
	buffer->renderer = renderer;
	buffer->format = struct_descriptor_clone_p(create_info->format);
	buffer->host_buffered_buffer = create_info->host_buffered_buffer;
	buffer->is_user_supplied_host_buffered_buffer = (create_info->host_buffered_buffer != NULL);
	/* if the user hasn't supplied already existing host buffered buffer object then create internally */
	if(!buffer->is_user_supplied_host_buffered_buffer)
	{
		vulkan_host_buffered_buffer_create_info_t _create_info = 
		{
			.stride = struct_descriptor_sizeof(buffer->format),
			.capacity = 1,
			.vo_usage_flags = create_info->vo_usage_flags
		};
		buffer->host_buffered_buffer = vulkan_host_buffered_buffer_create(renderer, &_create_info);
	}
}

RENDERER_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_create(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info)
{
	AUTO buffer = vulkan_formatted_buffer_new(renderer->allocator);
	vulkan_formatted_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

RENDERER_API void vulkan_formatted_buffer_destroy(vulkan_formatted_buffer_t* buffer)
{
	if(!buffer->is_user_supplied_host_buffered_buffer)
		vulkan_host_buffered_buffer_destroy(buffer->host_buffered_buffer);
	struct_descriptor_destroy(buffer->renderer->allocator, buffer->format);
}

RENDERER_API void vulkan_formatted_buffer_release_resources(vulkan_formatted_buffer_t* buffer)
{
	if(!buffer->is_user_supplied_host_buffered_buffer)
		vulkan_host_buffered_buffer_release_resources(buffer->host_buffered_buffer);
}


RENDERER_API buffer_t* vulkan_formatted_buffer_get_array_buffer(vulkan_formatted_buffer_t* buffer, const char* name)
{
	return NULL;
}

RENDERER_API u32 vulkan_formatted_buffer_get_array_length(vulkan_formatted_buffer_t* buffer, const char* name)
{
	return 0;
}

RENDERER_API void vulkan_formatted_buffer_set_uint(vulkan_formatted_buffer_t* buffer, const char* name, u32 value)
{

}

RENDERER_API bool vulkan_formatted_buffer_is_dirty(vulkan_formatted_buffer_t* buffer)
{
	return false;
}

RENDERER_API void vulkan_formatted_buffer_commit(vulkan_formatted_buffer_t* buffer)
{

}
