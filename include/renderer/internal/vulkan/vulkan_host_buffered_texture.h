
#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h> // vulkan_host_buffered_buffer_t
#include <renderer/buffer2d_view.h> // buffer2d_view_t
#include <bufferlib/buffer.h> // buffer_t

typedef struct vulkan_host_buffered_texture_create_info_t
{
	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count; 		// ignored if this is a depth texture, otherwise it must be in the range [1, 4]

	vulkan_texture_type_t type;						// type of the final vulkan texure i.e VULKAN_TEXTURE_TYPE_ALBEDO, VULKAN_TEXTURE_TYPE_NORMAL or VULKAN_TEXTURE_TYPE_CUBE
	vulkan_texture_usage_t usage; 					// runtime usage of the texture
	vulkan_texture_usage_t final_usage; 			// final usage of the texture
} vulkan_host_buffered_texture_create_info_t;

typedef struct vulkan_host_buffered_texture_t
{
	DERIVE_FROM(vulkan_texture_t);
	vulkan_renderer_t* renderer;
	/* linear buffer to store store pixel values on the host side and stage on the device (gpu) side */
	vulkan_host_buffered_buffer_t buffer;
	/* 2d view of the above linear buffer */
	buffer2d_view_t view;
} vulkan_host_buffered_texture_t;

#define VULKAN_HOST_BUFFERED_TEXTURE(ptr) DYNAMIC_CAST(vulkan_host_buffered_texture_t*, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_new(memory_allocator_t* allocator);
RENDERER_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_create(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info);
RENDERER_API void vulkan_host_buffered_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info, vulkan_host_buffered_texture_t OUT texture);
RENDERER_API void vulkan_host_buffered_texture_destroy(vulkan_host_buffered_texture_t* texture);
RENDERER_API void vulkan_host_buffered_texture_release_resources(vulkan_host_buffered_texture_t* texture);

/* flushes the host side buffer to the device (gpu) side VkDeviceMemory */
RENDERER_API void vulkan_host_buffered_texture_commit(vulkan_host_buffered_texture_t* texture);
/* returns pointer to the 2d view object used over the host side linear buffer */
static INLINE_IF_RELEASE_MODE buffer2d_view_t* vulkan_host_buffered_texture_get_view(vulkan_host_buffered_texture_t* texture)
{ 
	return &texture->view; 
}
/* returns pointer to the host side linear buffer object */
static INLINE_IF_RELEASE_MODE buffer_t* vulkan_host_buffered_texture_get_host_buffer(vulkan_host_buffered_texture_t* texture) 
{ 
	return vulkan_host_buffered_buffer_get_host_buffer(&texture->buffer); 
}


END_CPP_COMPATIBLE
