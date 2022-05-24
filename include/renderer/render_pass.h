
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_render_pass.h>
	typedef vulkan_render_pass_t render_pass_t;
	typedef vulkan_render_pass_create_info_t render_pass_create_info_t;
	typedef vulkan_render_pass_handle_t render_pass_handle_t;
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_render_pass.h>
	typedef opengl_render_pass_t render_pass_t;
	typedef opengl_render_pass_create_info_t render_pass_create_info_t;
	typedef opengl_render_pass_handle_t render_pass_handle_t;
#ifdef RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_render_pass.h>
	typedef directx_render_pass_t render_pass_t;
	typedef directx_render_pass_create_info_t render_pass_create_info_t;
	typedef directx_render_pass_handle_t render_pass_handle_t;
#ifdef RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_render_pass.h>
	typedef metal_render_pass_t render_pass_t;
	typedef metal_render_pass_create_info_t render_pass_create_info_t;
	typedef metal_render_pass_handle_t render_pass_handle_t;
#endif


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new();
RENDERER_API render_pass_t* render_pass_create(renderer_t* renderer, render_pass_create_info_t* create_info);
RENDERER_API void render_pass_create_no_alloc(renderer_t* renderer, render_pass_create_info_t* create_info, render_pass_t OUT pass);
RENDERER_API void render_pass_destroy(render_pass_t* pass);
RENDERER_API void render_pass_release_resources(render_pass_t* pass);

/* logic functions */
RENDERER_API void render_pass_begin(render_pass_t* pass, void* api_specific);
RENDERER_API void render_pass_end(render_pass_t* pass);

END_CPP_COMPATIBLE
