
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
#include <bufferlib/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define shader_load(renderer, file_path) shader_load_and_create(renderer, file_path)
RENDERER_API shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary);
RENDERER_API shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path);
RENDERER_API void shader_destroy(shader_t* shader);
RENDERER_API void shader_release_resources(shader_t* shader);

#ifdef __cplusplus
}
#endif // __cplusplus
