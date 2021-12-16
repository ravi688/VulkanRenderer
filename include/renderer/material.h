
#pragma once

typedef struct renderer_t renderer_t;

#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_material_t vulkan_material_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef vulkan_texture_t texture_t;
typedef vulkan_shader_t stage_shader_t;
typedef vulkan_material_t material_t;
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

material_t* material_new();
material_t* material_create(renderer_t* renderer, u32 shader_count, stage_shader_t** shaders);
void material_create_no_alloc(renderer_t* renderer, u32 shader_count, stage_shader_t** shaders, material_t* material);
void material_destroy(material_t* material, renderer_t* renderer);
void material_release_resources(material_t* material);

void material_bind(material_t* material, renderer_t* renderer);
void material_push_constants(material_t* material, renderer_t* renderer, void* bytes);
void material_set_texture(material_t* material, renderer_t* renderer, texture_t* texture);
