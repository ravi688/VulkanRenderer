
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

typedef enum shader_type_t
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_TESSELLATION
} shader_type_t;

shader_t* shader_new();
shader_t* shader_create(renderer_t*, const char* file_name, shader_type_t shader_type);
void shader_create_no_alloc(renderer_t*, const char* file_name, shader_type_t shader_type, shader_t* shader);
void shader_destroy(shader_t* shader, renderer_t*);
void shader_release_resources(shader_t* shader);
