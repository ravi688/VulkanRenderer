#include <renderer/render_texture.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

RENDERER_API render_texture_t* render_texture_create(renderer_t* renderer, render_texture_create_info_t* create_info)
{
	return vulkan_render_texture_create(renderer->handle, CAST_TO(vulkan_render_texture_create_info_t*, create_info));
}

RENDERER_API void render_texture_create_no_alloc(renderer_t* renderer, render_texture_create_info_t* create_info, render_texture_t OUT texture)
{
	vulkan_render_texture_create_no_alloc(renderer->handle, CAST_TO(vulkan_render_texture_create_info_t*, create_info), CAST_TO(vulkan_render_texture_t OUT, texture));
}

RENDERER_API void render_texture_destroy(render_texture_t* texture)
{
	vulkan_render_texture_destroy(CAST_TO(vulkan_render_texture_t*, texture));
}

RENDERER_API void render_texture_release_resources(render_texture_t* texture)
{
	vulkan_render_texture_release_resources(CAST_TO(vulkan_render_texture_t*, texture));
}

