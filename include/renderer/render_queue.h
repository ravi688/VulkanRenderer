
#pragma once


#include <renderer/defines.h>


#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_render_queue_t vulkan_render_queue_t;
	typedef vulkan_render_queue_t render_queue_t;
	typedef struct vulkan_render_object_t vulkan_render_object_t;
	typedef vulkan_render_object_t render_object_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_render_object_handle_t render_object_handle_t;
	#define RENDER_OBJECT_HANDLE_INVALID VULKAN_RENDER_OBJECT_HANDLE_INVALID
	typedef vulkan_render_queue_handle_t render_queue_handle_t;
	#define RENDER_QUEUE_HANDLE_INVALID VULKAN_RENDER_QUEUE_HANDLE_INVALID
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_render_queue_t opengl_render_queue_t;
	typedef opengl_render_queue_t render_queue_t;
	typedef struct opengl_render_object_t opengl_render_object_t;
	typedef opengl_render_object_t render_object_t;
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_render_object_handle_t render_object_handle_t;
	#define RENDER_OBJECT_HANDLE_INVALID OPENGL_RENDER_OBJECT_HANDLE_INVALID
	typedef opengl_render_queue_handle_t render_queue_handle_t;
	#define RENDER_QUEUE_HANDLE_INVALID OPENGL_RENDER_QUEUE_HANDLE_INVALID
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_render_queue_t directx_render_queue_t;
	typedef directx_render_queue_t render_queue_t;
	typedef struct directx_render_object_t directx_render_object_t;
	typedef directx_render_object_t render_object_t;
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_render_object_handle_t render_object_handle_t;
	#define RENDER_OBJECT_HANDLE_INVALID DIRECTX_RENDER_OBJECT_HANDLE_INVALID
	typedef directx_render_queue_handle_t render_queue_handle_t;
	#define RENDER_QUEUE_HANDLE_INVALID DIRECTX_RENDER_QUEUE_HANDLE_INVALID
#elif RENDERER_METAL_DRIVER
	typedef struct metal_render_queue_t metal_render_queue_t;
	typedef metal_render_queue_t render_queue_t;
	typedef struct metal_render_object_t metal_render_object_t;
	typedef metal_render_object_t render_object_t;
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_render_object_handle_t render_object_handle_t;
	#define RENDER_OBJECT_HANDLE_INVALID METAL_RENDER_OBJECT_HANDLE_INVALID
	typedef metal_render_queue_handle_t render_queue_handle_t;
	#define RENDER_QUEUE_HANDLE_INVALID METAL_RENDER_QUEUE_HANDLE_INVALID
#endif

// NOTE: this should be in sync with vulkan_render_queue_type_t
typedef enum render_queue_type_t
{
	RENDER_QUEUE_TYPE_UNDEFINED = 0,
	RENDER_QUEUE_TYPE_BACKGROUND,	// this render queue is rendered before any others
	RENDER_QUEUE_TYPE_GEOMETRY, 		// opaque geometry uses this queue
	RENDER_QUEUE_TYPE_ALPH_TESTED, 	// alpha tested geometry uses this queue
	RENDER_QUEUE_TYPE_GEOMETRY_LAST, // last render queue that is considered "opaque"
	RENDER_QUEUE_TYPE_TRANSPARENT, 	// this render queue is rendered after Geometry and AlphaTest, in back-to-front order
	RENDER_QUEUE_TYPE_OVERLAY 		// this render queue is meant for overlay effects
} render_queue_type_t;



/*
	description: Creates a render queue object
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_new();

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, render_queue_type_t type);

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
		OUT queue: pre allocated render_queue_t object (using render_queue_new())
	returns: nothing
 */
RENDERER_API void render_queue_create_no_alloc(renderer_t* renderer, render_queue_type_t type, render_queue_t OUT queue);

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
RENDERER_API render_object_handle_t render_queue_add(render_queue_t* queue, render_object_t* obj);

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
RENDERER_API void render_queue_removeH(render_queue_t* queue, render_object_handle_t handle);

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
