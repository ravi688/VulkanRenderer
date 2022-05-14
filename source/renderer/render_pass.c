
#include <renderer/render_pass.h>


#include <renderer/internal/vulkan/vulkan_render_pass.h>

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new()
{
	return vulkan_render_pass_new();
}

RENDERER_API render_pass_t* render_pass_create(renderer_t* context, void* api_specific)
{
	return vulkan_render_pass_create(context, api_specific);
}

RENDERER_API void render_pass_destroy(render_pass_t* pass)
{
	vulkan_render_pass_destroy(pass);
}

RENDERER_API void render_pass_release_resources(render_pass_t* pass)
{
	vulkan_render_pass_release_resources(pass);
}


RENDERER_API void render_pass_begin(render_pass_t* pass)
{
	vulkan_render_pass_begin(pass);
}

RENDERER_API void render_pass_end(render_pass_t* pass)
{
	vulkan_render_pass_end(pass);
}


/* Vulkan API specific */
RENDERER_API void render_pass_next(render_pass_t* pass)
{
	vulkan_render_pass_next(pass);
}

RENDERER_API void render_pass_bind_resource(render_pass_t* pass, u32 resource_index)
{
	vulkan_render_pass_bind_set(pass, resource_index);
}

RENDERER_API u32 render_pass_get_sub_pass_count(render_pass_t* pass)
{
	return vulkan_render_pass_get_sub_pass_count(pass);
}


