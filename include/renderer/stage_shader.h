
#pragma once

typedef struct renderer_t renderer_t;

#ifdef RENDERER_VULKAN_DRIVER

typedef struct vulkan_shader_t vulkan_shader_t;
typedef vulkan_shader_t stage_shader_t;

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

/*This enumeration should be in sync with vulkan_shader_type_t*/
typedef enum shader_stage_t
{
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_GEOMETRY,
	SHADER_STAGE_TESSELLATION
} shader_stage_t;

stage_shader_t* stage_shader_new();
stage_shader_t* stage_shader_create(renderer_t*, void* spirv, u32 length, shader_stage_t shader_stage);
void stage_shader_create_no_alloc(renderer_t*, void* spirv, u32 length, shader_stage_t shader_stage, stage_shader_t* shader);
stage_shader_t* stage_shader_load_and_create(renderer_t*, const char* file_name, shader_stage_t shader_stage);
void stage_shader_load_and_create_no_alloc(renderer_t*, const char* file_name, shader_stage_t shader_stage, stage_shader_t* shader);
void stage_shader_destroy(stage_shader_t* shader, renderer_t*);
void stage_shader_release_resources(stage_shader_t* shader);
