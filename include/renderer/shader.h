
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_shader.h>
	typedef vulkan_shader_t shader_t;
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_shader.h>
	typedef opengl_shader_t shader_t;
#elif RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_shader.h>
	typedef directx_shader_t shader_t;
#elif RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_shader.h>
	typedef metal_shader_t shader_t;
#endif

BEGIN_CPP_COMPATIBLE

RENDERER_API shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary);
RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path);
RENDERER_API void shader_destroy(shader_t* shader);
RENDERER_API void shader_release_resources(shader_t* shader);

END_CPP_COMPATIBLE
