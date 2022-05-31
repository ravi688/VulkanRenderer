
#pragma once

#include <renderer/defines.h>
#include <renderer/material.h>
#include <bufferlib/buffer.h>

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_render_object_t vulkan_render_object_t;
	typedef vulkan_render_object_t render_object_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_render_object_t opengl_render_object_t;
	typedef opengl_render_object_t render_object_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_render_object_t directx_render_object_t;
	typedef directx_render_object_t render_object_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_render_object_t metal_render_object_t;
	typedef metal_render_object_t metal_render_object_t;
#endif

#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef void (*render_object_draw_handler_t)(void* user_data);

typedef struct render_object_create_info_t
{
	material_t* material; 						// material that this object uses
	void* user_data;							// user data such as ptr to mesh_t or text_mesh_t
	render_object_draw_handler_t draw_handler;;	// ptr to the draw function, depends on the render object implementation
} render_object_create_info_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_object_t* render_object_new();
RENDERER_API render_object_t* render_object_create(renderer_t* renderer, render_object_create_info_t* create_info);
RENDERER_API void render_object_create_no_alloc(renderer_t* renderer, render_object_create_info_t* create_info, render_object_t OUT object);
RENDERER_API void render_object_destroy(render_object_t* object);
RENDERER_API void render_object_release_resources(render_object_t* object);

/* logic functions */
RENDERER_API void render_object_draw(render_object_t* object);
RENDERER_API void render_object_set_material(render_object_t* object, material_t* material);
RENDERER_API material_t* render_object_get_material(render_object_t* object);
RENDERER_API void render_object_set_transform(render_object_t* obj, mat4_t(float) transform);
RENDERER_API mat4_t(float) render_object_get_transform(render_object_t* obj);
RENDERER_API mat4_t(float) render_object_get_normal(render_object_t* obj);

END_CPP_COMPATIBLE
