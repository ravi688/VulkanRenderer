
#pragma once

#include <renderer/defines.h>

#include <renderer/render_pass.h>

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
	typedef vulkan_render_pass_pool_t render_pass_pool_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_render_pass_pool_t directx_render_pass_pool_t;
	typedef directx_render_pass_pool_t render_pass_pool_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_render_pass_pool_t metal_render_pass_pool_t;
	typedef metal_render_pass_pool_t render_pass_pool_t;
#endif

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new();
RENDERER_API render_pass_pool_t* render_pass_pool_create(renderer_t* renderer);
RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool);
RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool);

/* logic functions */
RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, render_pass_create_info_t* create_info);
RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle);

END_CPP_COMPATIBLE
