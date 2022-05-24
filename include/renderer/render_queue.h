
#pragma once


#include <renderer/defines.h>


#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_render_queue_t vulkan_render_queue_t;
	typedef vulkan_render_queue_t render_queue_t;
	typedef struct vulkan_render_object_t vulkan_render_object_t;
	typedef vulkan_render_object_t render_object_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_render_queue_t opengl_render_queue_t;
	typedef opengl_render_queue_t render_queue_t;
	typedef struct opengl_render_object_t opengl_render_object_t;
	typedef opengl_render_object_t render_object_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_render_queue_t directx_render_queue_t;
	typedef directx_render_queue_t render_queue_t;
	typedef struct directx_render_object_t directx_render_object_t;
	typedef directx_render_object_t render_object_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_render_queue_t metal_render_queue_t;
	typedef metal_render_queue_t render_queue_t;
	typedef struct metal_render_object_t metal_render_object_t;
	typedef metal_render_object_t render_object_t;
#endif


/*
	description: Creates a render queue object
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_new();

/*
	description: Creates a render queue object with identification name "name"
	params:
		renderer: ptr to the context
		name: identification name of this queue
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, const char* name);

/*
	description: Creates a render queue object with identification name "name"
	params:
		renderer: ptr to the context
		name: identification name of this queue
		OUT queue: pre allocated render_queue_t object (using render_queue_new())
	returns: nothing
 */
RENDERER_API void render_queue_create_no_alloc(renderer_t* renderer, const char* name, render_queue_t OUT queue);

/*
	description: Destroys the API specific resources
	params:
		queue: ptr to render queue
	return: nothing
 */
RENDERER_API void render_queue_destroy(render_queue_t* queue);

/*
	description: Releases the CPU side cached resources
	params:
		queue: ptr to render queue
	return: nothing
 */
RENDERER_API void render_queue_release_resources(render_queue_t* queue);

/* logic functions */
/*
	description: Adds a render object into the render queue
	params:
		queue: ptr to render queue
		obj: render object to be added into the render queue
	returns:
		nothing
	NOTE: Adding render object into the queue doesn't rebuild the queue, 
		you would still need to call render_queue_build()
		Adding render object into the queue sets is_ready to false
 */
RENDERER_API void render_queue_add(render_queue_t* queue, render_object_t* obj);

/*
	description: Removes a render object from the render queue
	params:
		queue: ptr to render queue
		obj: render object to be removed from the render queue
	returns:
		nothing
	NOTE: Removing render object from the queue doesn't rebuild the queue, 
		you would still need to call render_queue_build()
		Removing render object from the queue sets is_ready to false
 */
RENDERER_API void render_queue_remove(render_queue_t* queue, render_object_t* obj);

/*
	description: Builds and Orders the render objects according to the render pass handles
	params:
		queue: ptr to render queue
	returns:
		nothing
	NOTE: sets is_ready to true
 */
RENDERER_API void render_queue_build(render_queue_t* queue);

/*
	description: Builds the actual API specific command buffers
	params:
		queue: this render queue
	returns:
		nothing
	NOTE: if this render queue isn't ready (is_ready = false), then it first calls render_queue_build()
 */
RENDERER_API void render_queue_dispatch(render_queue_t* queue);
