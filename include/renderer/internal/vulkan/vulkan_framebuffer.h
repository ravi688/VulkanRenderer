
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>

typedef struct vulkan_attachment_t vulkan_attachment_t;
typedef struct vulkan_render_pass_t vulkan_render_pass_t;

typedef struct vulkan_framebuffer_t
{
	vulkan_renderer_t* renderer;
	VkFramebuffer vo_handle;
	
	vulkan_render_pass_t* pass;


	/* INTERNAL use only */
	bool is_supplementary_supported;
	VkImageView* image_views;
	u32 id;
} vulkan_framebuffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new();
RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id);
RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_t* render_pass, u32 id, vulkan_framebuffer_t OUT framebuffer);
RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer);

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_attachment_t* attachment);
RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer);

END_CPP_COMPATIBLE
