
#pragma once

typedef struct mesh3d_t mesh3d_t;
typedef struct renderer_t renderer_t;

/*Vulkan*/
#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_mesh_t vulkan_mesh_t;
typedef vulkan_mesh_t mesh_t;
#endif

/*OpenGL*/
#ifdef RENDERER_OPENGL_DRIVER
#error "OpenGL is not supported yet!"
#endif

/*DirectX*/
#ifdef RENDERER_DIRECTX_DRIVER
#error "DirectX is not supported yet!"
#endif

/*Metal*/
#ifdef RENDERER_METAL_DRIVER
#error "Metal is not supported yet!"
#endif

#include <renderer/defines.h>


mesh_t* mesh_new();
mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data);
void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh);
void mesh_destroy(mesh_t* mesh, renderer_t* renderer);
void mesh_release_resources(mesh_t* mesh);

void mesh_sync(mesh_t* mesh, renderer_t* renderer, mesh3d_t* mesh_data);
__attribute__((deprecated))
void mesh_draw(mesh_t* mesh, renderer_t* renderer);
void mesh_draw_indexed(mesh_t* mesh, renderer_t* renderer);
void mesh_draw_instanced(mesh_t* mesh, renderer_t* renderer, u32 instance_count);
void mesh_draw_indexed_instanced(mesh_t* mesh, renderer_t* renderer, u32 instance_count);
