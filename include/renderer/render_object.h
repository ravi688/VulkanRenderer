
#pragma once

#include <renderer/defines.h>
#include <renderer/material.h>
#include <bufferlib/buffer.h>

/*Vulkan*/
#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_render_object_t vulkan_render_object_t;
typedef vulkan_render_object_t render_object_t;
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

typedef struct render_object_create_info_t
{
	material_t* material; 					// material that this object uses
	void* user_data;						// user data such as ptr to mesh_t or text_mesh_t
	void (*draw)(void* user_data);			// ptr to the draw function, depends on the render object implementation
} render_object_create_info_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_object_t* render_object_new();
RENDERER_API render_object_t* render_object_create(render_object_create_info_t* create_info);
RENDERER_API void render_object_destroy(render_object_t* object);
RENDERER_API void render_object_release_resources(render_object_t* object);

/* logic functions */
RENDERER_API void render_object_draw(render_object_t* object);

END_CPP_COMPATIBLE
