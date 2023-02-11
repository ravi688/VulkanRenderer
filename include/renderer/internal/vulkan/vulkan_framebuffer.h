
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>

typedef struct vulkan_image_view_t vulkan_image_view_t;
typedef struct vulkan_render_pass_t vulkan_render_pass_t;

typedef struct vulkan_framebuffer_t
{
	vulkan_renderer_t* renderer;
	VkFramebuffer vo_handle;
	
	vulkan_render_pass_t* pass;


	/* INTERNAL use only */
	bool is_supplementary_supported;
	bool is_depth_supported;
	u32 depth_index; 	// index of the depth attachment in this framebuffer, otherwise U32_MAX
	// TODO: this should be shared among all the identical framebuffers (typically for a render pass)
	VkImageView* image_views;	// temporary buffer to be used when recreating this framebuffer
	u32 id; 		// index of this frambuffer in the list of the framebuffers (in case of double or tripple buffering)
} vulkan_framebuffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator);
RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id);
RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id, vulkan_framebuffer_t OUT framebuffer);
RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer);

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view);
RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_set_depth(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view);
RENDERER_API void vulkan_framebuffer_restore_depth(vulkan_framebuffer_t* framebuffer);

END_CPP_COMPATIBLE

