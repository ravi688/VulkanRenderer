
#include <renderer/render_pass.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new()
{
	return vulkan_render_pass_new();
}

RENDERER_API render_pass_t* render_pass_create(renderer_t* renderer, render_pass_create_info_t* create_info)
{
	return vulkan_render_pass_create(renderer->vulkan_handle, create_info);
}

RENDERER_API void render_pass_create_no_alloc(renderer_t* renderer, render_pass_create_info_t* create_info, render_pass_t OUT pass)
{
	vulkan_render_pass_create_no_alloc(renderer->vulkan_handle, create_info, pass);
}

RENDERER_API void render_pass_destroy(render_pass_t* pass)
{
	vulkan_render_pass_destroy(pass);
}

RENDERER_API void render_pass_release_resources(render_pass_t* pass)
{
	vulkan_render_pass_release_resources(pass);
}

RENDERER_API void render_pass_begin(render_pass_t* pass, void* api_specific)
{
	u32 framebuffer_index;
	if(api_specific == NULL)
		framebuffer_index = VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN;
	else
		framebuffer_index = DEREF_TO(u32, api_specific);
	vulkan_render_pass_begin(pass, framebuffer_index);
}

RENDERER_API void render_pass_end(render_pass_t* pass)
{
	vulkan_render_pass_end(pass);
}
