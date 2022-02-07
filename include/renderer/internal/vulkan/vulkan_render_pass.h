
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_render_pass_create_info_t
{
	VkFormat color_attachment_format; // could be VK_FORMAT_B8G8R8A8_SRGB
	VkFormat depth_attachment_format; // could be VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT

} vulkan_render_pass_create_info_t;

typedef struct vulkan_render_pass_begin_info_t
{
	VkFramebuffer framebuffer;	// framebuffer to render on
	u32 width;					// width of the framebuffer in pixels
	u32 height;					// height of the framebuffer in pixels
	u32 stencil;				// stencil buffer clear value
	float depth;				// depth buffer clear value
	struct
	{
		float r;
		float g;
		float b;
		float a;
	} color;					// color attachment clear value
} vulkan_render_pass_begin_info_t;

typedef struct vulkan_render_pass_t
{
	vulkan_renderer_t* renderer;
	VkRenderPass handle;
} vulkan_render_pass_t;

BEGIN_CPP_COMPATIBLE

/*
	description:
		allocates memory on heap and intiializes it to zero
	params:
		nothing
	returns:
		pointer to newly created vulkan_render_pass_t object on heap
 */
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new();

/*
	description:
		creates a vulkan render pass, it calls vulkan_render_pass_new() internally to allocate memory
	params:
		renderer: pointer to a valid vulkan_renderer_t object
		create_info: pointer to vulkan_render_pass_create_info_t object
	returns:
		pointer to newly created vulkan_render_pass_t object on heap
 */
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info);

/*
	description:
		destroys the vulkan objects
	params:
		render_pass: pointer to a valid vulkan_render_pass_t object, must have been created by vulkan_render_pass_create
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass);

/*
	description:
		releases heap allocated memory
	params:
		render_pass: pointer to a valid vulkan_render_pass_t object, must have been created by vulkan_render_pass_create or  allocated by vulkan_render_pass_new
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass);

RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, vulkan_render_pass_begin_info_t* begin_info);
RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass);

END_CPP_COMPATIBLE
