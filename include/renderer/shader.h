
#pragma once

typedef struct renderer_t renderer_t;

#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_shader_t vulkan_shader_t;
typedef vulkan_shader_t shader_t;
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
#include <buffer.h>

#define shader_load(renderer, file_path) shader_load_and_create(renderer, file_path)
shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary);
shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path);
void shader_destroy(shader_t* shader, renderer_t* renderer);
void shader_release_resources(shader_t* shader);
