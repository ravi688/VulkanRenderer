
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_shader_t vulkan_shader_t;
	typedef vulkan_shader_t shader_t;
	typedef vulkan_shader_handle_t shader_handle_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_shader_t opengl_shader_t;
	typedef opengl_shader_t shader_t;
	typedef opengl_shader_handle_t shader_handle_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_shader_t directx_shader_t;
	typedef directx_shader_t shader_t;
	typedef directx_shader_handle_t shader_handle_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_shader_t metal_shader_t;
	typedef metal_shader_t shader_t;
	typedef metal_shader_handle_t shader_handle_t;
#endif

BEGIN_CPP_COMPATIBLE

RENDERER_API shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary);
RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path);
RENDERER_API void shader_destroy(shader_t* shader);
RENDERER_API void shader_release_resources(shader_t* shader);

END_CPP_COMPATIBLE
