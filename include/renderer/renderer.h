
#pragma once

#include <renderer/defines.h>

typedef struct material_library_t material_library_t;
typedef struct shader_library_t shader_library_t;
typedef struct render_pass_pool_t render_pass_pool_t;

typedef struct render_window_t render_window_t;
#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_renderer_t vulkan_renderer_t;
#endif

typedef struct renderer_t
{
#ifdef RENDERER_VULKAN_DRIVER
	vulkan_renderer_t* handle;
#else
	void* handle;
#endif
	shader_library_t* shader_library;
	material_library_t* material_library;
	render_pass_pool_t* render_pass_pool;
} renderer_t;

typedef enum renderer_gpu_type_t
{
	RENDERER_GPU_TYPE_AUTO = 0,
	RENDERER_GPU_TYPE_INTEGRATED,
	RENDERER_GPU_TYPE_DISCRETE
} renderer_gpu_type_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen);
RENDERER_API void renderer_terminate(renderer_t* renderer);
RENDERER_API void renderer_update(renderer_t* renderer);
RENDERER_API bool renderer_is_running(renderer_t* renderer);

/*
	description:
		begins command buffer recording
	params:
		renderer: ptr to the renderer_t object
	returns:
		nothing
 */
RENDERER_API void renderer_begin_frame(renderer_t* renderer);

/*
	description:
		ends command buffer recording
 */
RENDERER_API void renderer_end_frame(renderer_t* renderer);

/* getters */
RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer);
RENDERER_API shader_library_t* renderer_get_shader_library(renderer_t* renderer);
RENDERER_API material_library_t* renderer_get_material_library(renderer_t* renderer);
RENDERER_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer);

END_CPP_COMPATIBLE
