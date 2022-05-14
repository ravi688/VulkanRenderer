
#pragma once

#include <renderer/defines.h>

/*Vulkan*/
#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef vulkan_render_pass_t render_pass_t;
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


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new();
RENDERER_API render_pass_t* render_pass_create(renderer_t* context, void* api_specific);
RENDERER_API void render_pass_destroy(render_pass_t* pass);
RENDERER_API void render_pass_release_resources(render_pass_t* pass);

RENDERER_API void render_pass_begin(render_pass_t* pass);
RENDERER_API void render_pass_end(render_pass_t* pass);

/* Vulkan API specific */
RENDERER_API void render_pass_next(render_pass_t* pass);
RENDERER_API void render_pass_bind_resource(render_pass_t* pass, u32 resource_index);
RENDERER_API u32 render_pass_get_sub_pass_count(render_pass_t* pass);


END_CPP_COMPATIBLE
