
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>

typedef struct vulkan_render_pass_t vulkan_render_pass_t;

/* structure to store create information for vulkan framebuffer object */
typedef struct vulkan_framebuffer_create_info_t
{
	vulkan_render_pass_t* render_pass;
	/* index of this framebuffer in the list */
	u32 id;	
	/* width of the framebuffer */
	u32 width;
	/* height of the framebuffer */
	u32 height;
	u32 attachment_count;
	VkImageView* attachments;
} vulkan_framebuffer_create_info_t;

typedef struct vulkan_framebuffer_t
{
	vulkan_renderer_t* renderer;
	VkFramebuffer vo_handle;

	/* used in vulkan_render_pass_begin() to check render_pass compatiblity */
	vulkan_render_pass_t* render_pass;

	/* width of this framebuffer */
	u32 width;
	/* height of this framebuffer */
	u32 height;
	u32 id; 		// index of this frambuffer in the list of the framebuffers (in case of double or tripple buffering)
} vulkan_framebuffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator);
RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info);
RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer);
RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer);

END_CPP_COMPATIBLE

