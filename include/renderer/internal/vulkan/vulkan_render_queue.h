/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_queue.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


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

#include <renderer/internal/vulkan/vulkan_handles.h>
#include <renderer/internal/vulkan/vulkan_render_pass_graph.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_render_object_t vulkan_render_object_t;
typedef struct vulkan_camera_t vulkan_camera_t;

typedef enum vulkan_render_queue_type_t
{
	VULKAN_RENDER_QUEUE_TYPE_UNDEFINED = 0,
	VULKAN_RENDER_QUEUE_TYPE_BACKGROUND,	// this render queue is rendered before any others
	VULKAN_RENDER_QUEUE_TYPE_GEOMETRY, 		// opaque geometry uses this queue
	VULKAN_RENDER_QUEUE_TYPE_ALPHA_TESTED, 	// alpha tested geometry uses this queue
	VULKAN_RENDER_QUEUE_TYPE_GEOMETRY_LAST, // last render queue that is considered "opaque"
	VULKAN_RENDER_QUEUE_TYPE_TRANSPARENT, 	// this render queue is rendered after Geometry and AlphaTest, in back-to-front order
	VULKAN_RENDER_QUEUE_TYPE_OVERLAY, 		// this render queue is meant for overlay effects
	VULKAN_RENDER_QUEUE_TYPE_QUEUE0, 		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE1, 		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE2,		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE3,		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE4, 		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE5, 		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE6, 		// general purpose queue
	VULKAN_RENDER_QUEUE_TYPE_QUEUE7, 		// general purpose queue

	VULKAN_RENDER_QUEUE_TYPE_MAX, 			// maximum invalid value
	VULKAN_RENDER_QUEUE_TYPE_MIN = VULKAN_RENDER_QUEUE_TYPE_UNDEFINED, 	// minimum invalid value
} vulkan_render_queue_type_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const char* vulkan_render_queue_type_str(const vulkan_render_queue_type_t type)
{
	switch(type)
	{
		case VULKAN_RENDER_QUEUE_TYPE_UNDEFINED : return "VULKAN_RENDER_QUEUE_TYPE_UNDEFINED";
		case VULKAN_RENDER_QUEUE_TYPE_BACKGROUND : return "VULKAN_RENDER_QUEUE_TYPE_BACKGROUND";
		case VULKAN_RENDER_QUEUE_TYPE_GEOMETRY : return "VULKAN_RENDER_QUEUE_TYPE_GEOMETRY";
		case VULKAN_RENDER_QUEUE_TYPE_ALPHA_TESTED : return "VULKAN_RENDER_QUEUE_TYPE_ALPHA_TESTED";
		case VULKAN_RENDER_QUEUE_TYPE_GEOMETRY_LAST : return "VULKAN_RENDER_QUEUE_TYPE_GEOMETRY_LAST";
		case VULKAN_RENDER_QUEUE_TYPE_TRANSPARENT : return "VULKAN_RENDER_QUEUE_TYPE_TRANSPARENT";
		case VULKAN_RENDER_QUEUE_TYPE_OVERLAY : return "VULKAN_RENDER_QUEUE_TYPE_OVERLAY";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE0 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE0";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE1 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE1";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE2 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE2";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE3 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE3";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE4 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE4";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE5 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE5";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE6 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE6";
		case VULKAN_RENDER_QUEUE_TYPE_QUEUE7 : return "VULKAN_RENDER_QUEUE_TYPE_QUEUE7";
		case VULKAN_RENDER_QUEUE_TYPE_MAX : return "VULKAN_RENDER_QUEUE_TYPE_MAX";
	}
	return "<undefined>";
}

typedef struct vulkan_render_queue_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;

	vulkan_render_queue_handle_t handle;  // handle to this queue in the vulkan render scene

	/* type of this queue */
	vulkan_render_queue_type_t type;

	/* < vulkan_render_pass_handle_t, list of <list of shader handles > >*/
	dictionary_t render_pass_handles;
	/* < vulkan_shader_handle_t , list of dictionary<vulkan_material_handle_t, list of render objects > > */
	dictionary_t shader_handles;
	/* true if this render queue is ready to dispatch */
	bool is_ready;

	/* render pass graph for this render queue */
	vulkan_render_pass_graph_t pass_graph;

} vulkan_render_queue_t;

#define VULKAN_RENDER_QUEUE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_render_queue_t*, VULKAN_OBJECT_TYPE_RENDER_QUEUE, ptr)
#define VULKAN_RENDER_QUEUE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_render_queue_t*, VULKAN_OBJECT_TYPE_RENDER_QUEUE, ptr)


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */

/*
	description: Creates a render queue object
	returns: ptr to the newly created vulkan_render_queue_t object
 */
RENDERER_API vulkan_render_queue_t* vulkan_render_queue_new(memory_allocator_t* allocator);

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
	returns: ptr to the newly created vulkan_render_queue_t object
 */
RENDERER_API vulkan_render_queue_t* vulkan_render_queue_create(vulkan_renderer_t* renderer, vulkan_render_queue_type_t type);

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
		OUT queue: pre allocated vulkan_render_queue_t object (using vulkan_render_queue_new())
	returns: nothing
 */
RENDERER_API void vulkan_render_queue_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_queue_type_t type, vulkan_render_queue_t OUT queue);

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

RENDERER_API void vulkan_render_queue_destroy_all_objects(vulkan_render_queue_t* queue);

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
RENDERER_API vulkan_render_object_handle_t vulkan_render_queue_add(vulkan_render_queue_t* queue, vulkan_render_object_t* obj);

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
RENDERER_API void vulkan_render_queue_removeH(vulkan_render_queue_t* queue, vulkan_render_object_handle_t handle);

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
RENDERER_API void vulkan_render_queue_dispatch(vulkan_render_queue_t* queue, vulkan_camera_t* camera);

END_CPP_COMPATIBLE
