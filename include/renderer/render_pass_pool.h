
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID VULKAN_RENDER_PASS_HANDLE_INVALID
	typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
	typedef vulkan_render_pass_pool_t render_pass_pool_t;
	typedef struct vulkan_render_pass_t vulkan_render_pass_t;
	typedef vulkan_render_pass_t render_pass_t;
	typedef struct vulkan_render_pass_create_info_t vulkan_render_pass_create_info_t;
	typedef vulkan_render_pass_create_info_t render_pass_create_info_t;
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID OPENGL_RENDER_PASS_HANDLE_INVALID
	typedef struct opengl_render_pass_pool_t opengl_render_pass_pool_t;
	typedef opengl_render_pass_pool_t render_pass_pool_t;
	typedef struct opengl_render_pass_t opengl_render_pass_t;
	typedef opengl_render_pass_t render_pass_t;
	typedef struct opengl_render_pass_create_info_t opengl_render_pass_create_info_t;
	typedef opengl_render_pass_create_info_t render_pass_create_info_t;
#elif RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID DIRECTX_RENDER_PASS_HANDLE_INVALID
	typedef struct directx_render_pass_pool_t directx_render_pass_pool_t;
	typedef directx_render_pass_pool_t render_pass_pool_t;
	typedef struct directx_render_pass_t directx_render_pass_t;
	typedef directx_render_pass_t render_pass_t;
	typedef struct directx_render_pass_create_info_t directx_render_pass_create_info_t;
	typedef directx_render_pass_create_info_t render_pass_create_info_t;
#elif RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID METAL_RENDER_PASS_HANDLE_INVALID
	typedef struct metal_render_pass_pool_t metal_render_pass_pool_t;
	typedef metal_render_pass_pool_t render_pass_pool_t;
	typedef struct metal_render_pass_t metal_render_pass_t;
	typedef metal_render_pass_t render_pass_t;
	typedef struct metal_render_pass_create_info_t metal_render_pass_create_info_t;
	typedef metal_render_pass_create_info_t render_pass_create_info_t;
#endif

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API render_pass_pool_t* render_pass_pool_new();
RENDERER_API render_pass_pool_t* render_pass_pool_create(renderer_t* renderer);
RENDERER_API void render_pass_pool_destroy(render_pass_pool_t* pool);
RENDERER_API void render_pass_pool_release_resources(render_pass_pool_t* pool);

typedef enum render_pass_pool_pass_preset_t
{
	RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN,
	RENDER_PASS_POOL_PASS_PRESET_COLOR_DEPTH_SWAPCHAIN,
	RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP
} render_pass_pool_pass_preset_t;

/* logic functions */
RENDERER_API render_pass_handle_t render_pass_pool_create_pass_from_preset(render_pass_pool_t* pool, render_pass_pool_pass_preset_t preset);
RENDERER_API render_pass_handle_t render_pass_pool_create_pass(render_pass_pool_t* pool, render_pass_create_info_t* create_info);
RENDERER_API render_pass_t* render_pass_pool_getH(render_pass_pool_t* pool, render_pass_handle_t handle);

END_CPP_COMPATIBLE
