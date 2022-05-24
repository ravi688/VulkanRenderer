
#include <renderer/render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new()
{
	return vulkan_render_pass_pool_new();
}

RENDERER_API render_pass_pool_t* render_pass_pool_create(renderer_t* renderer)
{
	return vulkan_render_pass_pool_create(renderer->vulkan_handle);
}

RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool)
{
	vulkan_render_pass_pool_destroy(pool);
}

RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool)
{
	vulkan_render_pass_pool_release_resources(pool);
}


/* logic functions */
RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, render_pass_create_info_t* create_info)
{
	return render_pass_pool_create_pass(pool, create_info);
}

RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle)
{
	return render_pass_pool_getH(pool, handle);
}

