
#pragma once

#include <renderer/defines.h>

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_renderer_t vulkan_renderer_t;
	typedef vulkan_renderer_t driver_t;
	typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
	typedef vulkan_render_pass_pool_t render_pass_pool_t;
	typedef struct vulkan_shader_library_t vulkan_shader_library_t;
	typedef vulkan_shader_library_t shader_library_t;
	typedef struct vulkan_material_library_t vulkan_material_library_t;
	typedef vulkan_material_library_t material_library_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_renderer_t opengl_renderer_t;
	typedef opengl_renderer_t driver_t;
	typedef struct opengl_render_pass_pool_t opengl_render_pass_pool_t;
	typedef opengl_render_pass_pool_t render_pass_pool_t;
	typedef struct opengl_shader_library_t opengl_shader_library_t;
	typedef opengl_shader_library_t shader_library_t;
	typedef struct opengl_material_library_t opengl_material_library_t;
	typedef opengl_material_library_t material_library_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_renderer_t directx_renderer_t;
	typedef directx_renderer_t driver_t;
	typedef struct directx_render_pass_pool_t directx_render_pass_pool_t;
	typedef directx_render_pass_pool_t render_pass_pool_t;
	typedef struct directx_shader_library_t directx_shader_library_t;
	typedef directx_shader_library_t shader_library_t;
	typedef struct directx_material_library_t directx_material_library_t;
	typedef directx_material_library_t material_library_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_renderer_t metal_renderer_t;
	typedef metal_renderer_t driver_t;
	typedef struct metal_render_pass_pool_t metal_render_pass_pool_t;
	typedef metal_render_pass_pool_t render_pass_pool_t;
	typedef struct metal_shader_library_t metal_shader_library_t;
	typedef metal_shader_library_t shader_library_t;
	typedef struct material_shader_library_t material_shader_library_t;
	typedef metal_material_library_t material_library_t;
#endif

typedef struct render_window_t render_window_t;

typedef struct renderer_t
{
	union
	{
		driver_t*
#ifdef RENDERER_VULKAN_DRIVER
		vulkan_driver, *vulkan_handle;
#elif RENDERER_OPENGL_DRIVER
		opengl_driver, *opengl_handle;
#elif RENDERER_DIRECTX_DRIVER
		directx_driver, directx_handle;
#elif RENDERER_METAL_DRIVER
		metal_driver, *metal_handle;
#endif
		driver_t* handle;
		driver_t* driver;
	};
	// shader_library_t* shader_library;
	// material_library_t* material_library;
	// render_pass_pool_t* render_pass_pool;
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
