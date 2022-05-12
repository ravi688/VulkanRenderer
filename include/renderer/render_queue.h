
#pragma once

#include <renderer/defines.h>


#define RENDER_QUEUE_NAME_MAX_SIZE 32

typedef struct render_object_t render_object_t;
typedef struct render_queue_t render_queue_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */

/*
	Description: Creates a render queue object
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_new();

/*
	Description: Creates a render queue object with identification name "name"
	renderer: ptr to the context
	name: identification name of this queue
	returns: ptr to the newly created render_queue_t object
 */
RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, const char* name);

/*
	Description: Destroys the API specific resources
	queue: ptr to render queue
 */
RENDERER_API void render_queue_destroy(render_queue_t* queue);

/*
	Description: Releases the CPU side cached resources
	queue: ptr to render queue
 */
RENDERER_API void render_queue_release_resources(render_queue_t* queue);

/* logic functions */
/*
	Description: Adds a render object into the render queue
	queue: ptr to render queue
	obj: render object to be added into the render queue
	NOTE: Adding render object into the queue doesn't rebuild the queue, 
		you would still need to call render_queue_build()
		Adding render object into the queue sets is_ready to false
 */
RENDERER_API void render_queue_add(render_queue_t* queue, render_object_t* obj);

/*
	Description: Removes a render object from the render queue
	queue: ptr to render queue
	obj: render object to be removed from the render queue
	NOTE: Removing render object from the queue doesn't rebuild the queue, 
		you would still need to call render_queue_build()
		Removing render object from the queue sets is_ready to false
 */
RENDERER_API void render_queue_remove(render_queue_t* queue, render_object_t* obj);

/*
	Description: Builds and Orders the render objects according to the render pass handles
	queue: ptr to render queue
	NOTE: sets is_ready to true
 */
RENDERER_API void render_queue_build(render_queue_t* queue);

/*
	Description: Builds the actual API specific command buffers
	queue: this render queue
	NOTE: if this render queue isn't ready (is_ready = false), then it first calls render_queue_build()
 */
RENDERER_API void render_queue_dispatch(render_queue_t* queue);

END_CPP_COMPATIBLE
