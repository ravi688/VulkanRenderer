
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_render_pass.h>
	typedef vulkan_render_pass_t render_pass_t;
	typedef vulkan_render_pass_create_info_t render_pass_create_info_t;
	typedef vulkan_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID VULKAN_RENDER_PASS_HANDLE_INVALID

	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;

#elif defined(RENDERER_OPENGL_DRIVER)
	#include <renderer/internal/opengl/opengl_render_pass.h>
	typedef opengl_render_pass_t render_pass_t;
	typedef opengl_render_pass_create_info_t render_pass_create_info_t;
	typedef opengl_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID OPENGL_RENDER_PASS_HANDLE_INVALID

	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;

#elif defined(RENDERER_DIRECTX_DRIVER)
	#include <renderer/internal/directx/directx_render_pass.h>
	typedef directx_render_pass_t render_pass_t;
	typedef directx_render_pass_create_info_t render_pass_create_info_t;
	typedef directx_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID DIRECTX_RENDER_PASS_HANDLE_INVALID

	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;

#elif defined(RENDERER_METAL_DRIVER)
	#include <renderer/internal/metal/metal_render_pass.h>
	typedef metal_render_pass_t render_pass_t;
	typedef metal_render_pass_create_info_t render_pass_create_info_t;
	typedef metal_render_pass_handle_t render_pass_handle_t;
	#define RENDER_PASS_HANDLE_INVALID METAL_RENDER_PASS_HANDLE_INVALID

	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;

#endif


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new();
RENDERER_API render_pass_t* render_pass_create(renderer_t* renderer, render_pass_create_info_t* create_info);
RENDERER_API void render_pass_create_no_alloc(renderer_t* renderer, render_pass_create_info_t* create_info, render_pass_t OUT pass);
RENDERER_API void render_pass_destroy(render_pass_t* pass);
RENDERER_API void render_pass_release_resources(render_pass_t* pass);

/* logic functions */
RENDERER_API void render_pass_set_clear(render_pass_t* pass, color_t color, float depth);
RENDERER_API void render_pass_begin(render_pass_t* pass, void* api_specific, camera_t* camera);
RENDERER_API void render_pass_end(render_pass_t* pass);

END_CPP_COMPATIBLE
