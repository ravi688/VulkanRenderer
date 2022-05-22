
/*
	Render Queue: 

	description: 
		Render queue contains a list of objects to be rendered
		It's the render queue which decides order of rendering of the objects in the list
		Render queue binds the resources and issues a draw call for the render object.

	how it works:

		adding an render object in the render queue:
		--------------------------------------------

		for each render pass handles in the render object's material.shader
			if render queue doesn't contain the render pass handle
				add it to the queue
			for each sub pass j
				if shader handles doesn't contain this shader handle
					add it to the list of shader handles for this sub pass
				if material handles of this shader handle doesn't contain this material handle
					add it
				if render objects of this material handle doesn't contain this render object
					add it


		In-efficient algorithm:
		-------------------------

		for each render pass i
			for each subpass j
				for each render object using this sub pass
					bind pipeline for render pass index i and sub pass index j
					bind global set
					bind render set
					bind sub render set
					bind material set
					bind object set
					draw

		Efficient algorithm:
		------------------------

		for each render pass i
			for each subpass j
				for each shader
					bind pipeline (pass.handle, j)
					bind global set
					bind render set
					bind sub render set
					for each material
						bind material set
						for each render object
							bind object set
							draw

 */

#pragma once

#include <renderer/defines.h>

#include <renderer/string.h> 		// for string_t
#include <renderer/dictionary.h> 	// for dictionary_t

typedef struct vulkan_render_object_t vulkan_render_object_t;

typedef struct vulkan_render_queue_t
{
	vulkan_renderer_t* renderer;
	/* identification name of this render queue */
	string_t name;
	/* < vulkan_render_pass_handle_t, list of <list of shader handles > >*/
	dictionary_t render_pass_handles;
	/* < vulkan_shader_handle_t , list of dictionary<vulkan_material_handle_t, list of render objects > > */
	dictionary_t shader_handles;
	/* true if this render queue is ready to dispatch */
	bool is_ready;
} vulkan_render_queue_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */

/*
	description: Creates a render queue object
	returns: ptr to the newly created vulkan_render_queue_t object
 */
RENDERER_API vulkan_render_queue_t* vulkan_render_queue_new();

/*
	description: Creates a render queue object with identification name "name"
	params:
		renderer: ptr to the context
		name: identification name of this queue
	returns: ptr to the newly created vulkan_render_queue_t object
 */
RENDERER_API vulkan_render_queue_t* vulkan_render_queue_create(vulkan_renderer_t* renderer, const char* name);

/*
	description: Creates a render queue object with identification name "name"
	params:
		renderer: ptr to the context
		name: identification name of this queue
		OUT queue: pre allocated vulkan_render_queue_t object (using vulkan_render_queue_new())
	returns: nothing
 */
RENDERER_API void vulkan_render_queue_create_no_alloc(vulkan_renderer_t* renderer, const char* name, vulkan_render_queue_t OUT queue);

/*
	description: Destroys the API specific resources
	params:
		queue: ptr to render queue
	return: nothing
 */
RENDERER_API void vulkan_render_queue_destroy(vulkan_render_queue_t* queue);

/*
	description: Releases the CPU side cached resources
	params:
		queue: ptr to render queue
	return: nothing
 */
RENDERER_API void vulkan_render_queue_release_resources(vulkan_render_queue_t* queue);

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
RENDERER_API void vulkan_render_queue_add(vulkan_render_queue_t* queue, vulkan_render_object_t* obj);

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
RENDERER_API void vulkan_render_queue_remove(vulkan_render_queue_t* queue, vulkan_render_object_t* obj);

/*
	description: Builds and Orders the render objects according to the render pass handles
	params:
		queue: ptr to render queue
	returns:
		nothing
	NOTE: sets is_ready to true
 */
RENDERER_API void vulkan_render_queue_build(vulkan_render_queue_t* queue);

/*
	description: Builds the actual API specific command buffers
	params:
		queue: this render queue
	returns:
		nothing
	NOTE: if this render queue isn't ready (is_ready = false), then it first calls render_queue_build()
 */
RENDERER_API void vulkan_render_queue_dispatch(vulkan_render_queue_t* queue);

END_CPP_COMPATIBLE
