
#pragma once

#include <renderer/defines.h>


typedef struct render_pass_t
{
	void* handle; 			// handle to the render pass object specific to the backend API
	void* info;	 			// info about the backend API specific render pass
} render_pass_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API render_pass_t* render_pass_new();
RENDERER_API render_pass_t* render_pass_create(void* handle, void* info);
RENDERER_API void render_pass_destroy(render_pass_t* pass);
RENDERER_API void render_pass_release_resources(render_pass_t* pass);

RENDERER_API void render_pass_begin(render_pass_t* pass, renderer_t* context);
RENDERER_API void render_pass_end(render_pass_t* pass, renderer_t* context);
RENDERER_API void render_pass_next(render_pass_t* pass, renderer_t* context);

END_CPP_COMPATIBLE
