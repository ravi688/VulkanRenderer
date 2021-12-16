
#pragma once

typedef struct renderer_t renderer_t;

#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_texture_t vulkan_texture_t;
typedef vulkan_texture_t texture_t;
#endif

#ifdef RENDERER_OPENGL_DRIVER
#error "OpenGL is not supported yet!"
#endif

#ifdef RENDERER_DIRECTX_DRIVER
#error "DirectX is not supported yet!"
#endif

#ifdef RENDERER_METAL_DRIVER
#error "Metal is not supported yet!"
#endif

#include <renderer/defines.h>

typedef struct texture_create_info_t
{
	void* data;
	u32 width;
	u32 height;
	u8 channel_count;
} texture_create_info_t;

texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info);
texture_t* texture_load(renderer_t* renderer, const char* file_path);
void texture_destroy(texture_t* texture, renderer_t* renderer);
void texture_release_resources(texture_t* texture);
