
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>

typedef struct vulkan_image_view_t vulkan_image_view_t;
typedef struct vulkan_render_pass_t vulkan_render_pass_t;

/* structure to store create information for vulkan framebuffer object */
typedef struct vulkan_framebuffer_create_info_t
{
	/* render pass for which this framebuffer would work */
	vulkan_render_pass_t* render_pass;
	/* index of this framebuffer in the list */
	u32 id;	
} vulkan_framebuffer_create_info_t;

typedef struct vulkan_framebuffer_t
{
	vulkan_renderer_t* renderer;
	VkFramebuffer vo_handle;
	
	vulkan_render_pass_t* pass;


	/* INTERNAL use only */
	/* stack to be used to hold the modified image views in the original image view list 
	 * the modifications (replacements) are done via calling vulkan_framebufffer_set_{supplementary|depth}
	 * and vulkan_framebuffer_restore_{supplementary|depth} functions. 
	 * Also, refreshing the framebuffer (copying the updated image view list from the render pass) uses this stack
	 * to reapply the modifications after refresh. */
	BUFFER modification_stack;
	/* width of this framebuffer */
	u32 width;
	/* height of this framebuffer */
	u32 height;
	bool is_supplementary_supported;
	bool is_depth_supported;
	u32 depth_index; 	// index of the depth attachment in this framebuffer, otherwise U32_MAX
	// TODO: this should be shared among all the identical framebuffers (typically for a render pass)
	VkImageView* image_views;	// temporary buffer to be used when recreating this framebuffer
	u32 id; 		// index of this frambuffer in the list of the framebuffers (in case of double or tripple buffering)
} vulkan_framebuffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator);
RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info);
RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer);
RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer);

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view);
RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_set_depth(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view);
RENDERER_API void vulkan_framebuffer_restore_depth(vulkan_framebuffer_t* framebuffer);

/* structure to store refresh information while refreshing the framebuffer object */
typedef struct vulkan_framebuffer_refresh_info_t
{
	/* width of the framebuffer */
	u32 width;
	/* height of the framebuffer */
	u32 height;
	/* if true, causes the copying of VkImageView from render pass */
	bool is_update_image_views;
} vulkan_framebuffer_refresh_info_t;

/* refreshes (recreates) the framebuffer object */
RENDERER_API void vulkan_framebuffer_refresh(vulkan_framebuffer_t* framebuffer, vulkan_framebuffer_refresh_info_t* info);

END_CPP_COMPATIBLE

