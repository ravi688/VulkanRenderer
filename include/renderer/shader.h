
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_shader.h>
	typedef vulkan_shader_t shader_t;
	typedef vulkan_shader_create_info_t shader_create_info_t;
	typedef vulkan_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID VULKAN_SHADER_HANDLE_INVALID
#elif RENDERER_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_shader.h>
	typedef opengl_shader_t shader_t;
	typedef opengl_shader_create_info_t shader_create_info_t;
	typedef opengl_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID OPENGL_SHADER_HANDLE_INVALID
#elif RENDERER_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_shader.h>
	typedef directx_shader_t shader_t;
	typedef directx_shader_create_info_t shader_create_info_t;
	typedef directx_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID DIRECTX_SHADER_HANDLE_INVALID
#elif RENDERER_METAL_DRIVER
	#include <renderer/internal/metal/metal_shader.h>
	typedef metal_shader_t shader_t;
	typedef metal_shader_create_info_t shader_create_info_t;
	typedef metal_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID METAL_SHADER_HANDLE_INVALID
#endif

BEGIN_CPP_COMPATIBLE

RENDERER_API shader_t* shader_create(renderer_t* renderer, shader_create_info_t* create_info);
RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path);
RENDERER_API void shader_destroy(shader_t* shader);
RENDERER_API void shader_release_resources(shader_t* shader);

END_CPP_COMPATIBLE
