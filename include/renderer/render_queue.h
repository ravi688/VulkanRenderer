/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_queue.h is a part of VulkanRenderer

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


#pragma once


#include <renderer/defines.h>


#ifdef SGE_VULKAN_DRIVER
	typedef struct vulkan_render_queue_t vulkan_render_queue_t;
	typedef vulkan_render_queue_t render_queue_t;
	typedef struct vulkan_render_object_t vulkan_render_object_t;
	typedef vulkan_render_object_t render_object_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>

	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;
	typedef struct vulkan_render_scene_t vulkan_render_scene_t;
	typedef vulkan_render_scene_t render_scene_t;

#elif SGE_OPENGL_DRIVER
	typedef struct opengl_render_queue_t opengl_render_queue_t;
	typedef opengl_render_queue_t render_queue_t;
	typedef struct opengl_render_object_t opengl_render_object_t;
	typedef opengl_render_object_t render_object_t;
	#include <renderer/internal/opengl/opengl_handles.h>

	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;
	typedef struct opengl_render_scene_t opengl_render_scene_t;
	typedef opengl_render_scene_t render_scene_t;
	
#elif SGE_DIRECTX_DRIVER
	typedef struct directx_render_queue_t directx_render_queue_t;
	typedef directx_render_queue_t render_queue_t;
	typedef struct directx_render_object_t directx_render_object_t;
	typedef directx_render_object_t render_object_t;
	#include <renderer/internal/directx/directx_handles.h>

	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;
	typedef struct directx_render_scene_t directx_render_scene_t;
	typedef directx_render_scene_t render_scene_t;
	
#elif SGE_METAL_DRIVER
	typedef struct metal_render_queue_t metal_render_queue_t;
	typedef metal_render_queue_t render_queue_t;
	typedef struct metal_render_object_t metal_render_object_t;
	typedef metal_render_object_t render_object_t;
	#include <renderer/internal/metal/metal_handles.h>

	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;
	typedef struct metal_render_scene_t metal_render_scene_t;
	typedef metal_render_scene_t render_scene_t;
	
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
	RENDER_QUEUE_TYPE_OVERLAY, 		// this render queue is meant for overlay effects
	RENDER_QUEUE_TYPE_QUEUE0, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE1, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE2, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE3, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE4, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE5, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE6, 		// general purpose queue
	RENDER_QUEUE_TYPE_QUEUE7 		// general purpose queue
} render_queue_type_t;



/*
	description: Creates a render queue object
	returns: ptr to the newly created render_queue_t object
 */
SGE_API render_queue_t* render_queue_new(memory_allocator_t* allocator);

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
	returns: ptr to the newly created render_queue_t object
 */
SGE_API render_queue_t* render_queue_create(renderer_t* renderer, render_queue_type_t type);

/*
	description: Creates a render queue object
	params:
		renderer: ptr to the context
		OUT queue: pre allocated render_queue_t object (using render_queue_new())
	returns: nothing
 */
SGE_API void render_queue_create_no_alloc(renderer_t* renderer, render_queue_type_t type, render_queue_t OUT queue);

/*
	description: Destroys the API specific resources
	params:
		queue: ptr to render queue
	return: nothing
 */
SGE_API void render_queue_destroy(render_queue_t* queue);

/*
	description: Releases the CPU side cached resources
	params:
		queue: ptr to render queue
	return: nothing
 */
SGE_API void render_queue_release_resources(render_queue_t* queue);

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
SGE_API void render_queue_add(render_queue_t* queue, render_object_t* obj);

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
SGE_API void render_queue_removeH(render_queue_t* queue, render_object_t* obj);

/*
	description: Builds and Orders the render objects according to the render pass handles
	params:
		queue: ptr to render queue
	returns:
		nothing
	NOTE: sets is_ready to true
 */
SGE_API void render_queue_build(render_queue_t* queue);

/*
	description: Builds the actual API specific command buffers
	params:
		queue: this render queue
		camera: the camera to which output have to be written
		scene: pointer to the render_scene_t object, it is used to query scene specific information and pass that to shaders
	returns:
		nothing
	NOTE: if this render queue isn't ready (is_ready = false), then it first calls render_queue_build()
 */
SGE_API void render_queue_dispatch(render_queue_t* queue, camera_t* camera, render_scene_t* scene);
