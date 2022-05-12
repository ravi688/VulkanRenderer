
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

typedef struct render_pass_t render_pass_t; 
typedef bool (*render_pass_comparer_t)(render_pass_t* pass, render_pass_t* ref);
typedef buf_ucount_t render_pass_handle_t;

typedef struct render_pass_pool_t
{
	render_pass_comparer_t is_identical;	// ptr to the render pass compare function
	BUFFER render_passes; 					// list of render_pass_t objects			
} render_pass_pool_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API render_pass_pool_t* render_pass_pool_new();
RENDERER_API render_pass_pool_t* render_pass_pool_create(render_pass_comparer_t comparer);
RENDERER_API render_pass_handle_t render_pass_pool_add(render_pass_pool_t* pool, render_pass_t* pass);
RENDERER_API render_pass_t* render_pass_pool_get(render_pass_pool_t* pool, render_pass_handle_t handle);
RENDERER_API void render_pass_pool_remove(render_pass_pool_t* pool, render_pass_handle_t handle);
RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool);
RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool);

END_CPP_COMPATIBLE
