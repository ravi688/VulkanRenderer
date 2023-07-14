#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>

typedef struct vulkan_host_buffered_buffer_create_info_t
{
	/* element size */
	u32 stride;
	/* number of elements that can be stored initially */
	u32 capacity;
	/* usage flags for the device buffer */
	VkBufferUsageFlags vo_usage_flags;
} vulkan_host_buffered_buffer_create_info_t;

typedef BUFFER buffer_t;

/* a GPU buffer having a coherent copy on the host side */
typedef struct vulkan_host_buffered_buffer_t
{
	vulkan_renderer_t* renderer;
	vulkan_buffer_t device_buffer;
	buffer_t host_buffer;
	VkBufferUsageFlags vo_usage_flags;
	bool has_device_buffer;
	bool is_dirty;
} vulkan_host_buffered_buffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_new(memory_allocator_t* allocator);
RENDERER_API vulkan_host_buffered_buffer_t* vulkan_host_buffered_buffer_create(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info);
RENDERER_API void vulkan_host_buffered_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_buffer_create_info_t* create_info, vulkan_host_buffered_buffer_t OUT buffer);
RENDERER_API void vulkan_host_buffered_buffer_destroy(vulkan_host_buffered_buffer_t* buffer);
RENDERER_API void vulkan_host_buffered_buffer_release_resources(vulkan_host_buffered_buffer_t* buffer);

RENDERER_API buffer_t* vulkan_host_buffered_buffer_get_host_buffer(vulkan_host_buffered_buffer_t* buffer);
RENDERER_API vulkan_buffer_t* vulkan_host_buffered_buffer_get_device_buffer(vulkan_host_buffered_buffer_t* buffer);

static INLINE_IF_RELEASE_MODE void vulkan_host_buffered_buffer_set_dirty(vulkan_host_buffered_buffer_t* buffer, bool is_dirty)
{
	buffer->is_dirty = is_dirty;
}

/* returns true if either the exisiting VkBuffer has been updated with new data or VkBuffer is recreated upon resize */
RENDERER_API bool vulkan_host_buffered_buffer_commit(vulkan_host_buffered_buffer_t* buffer, bool OUT is_resized);

END_CPP_COMPATIBLE
