/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_scene.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_render_queue.h>	 		// for vulkan_render_queue_type_t
#include <renderer/internal/vulkan/vulkan_render_object.h> 			// for vulkan_render_object_type_t
#include <renderer/internal/vulkan/vulkan_handles.h> 				// for vulkan_render_scene_handle_t, vulkan_render_queue_handle_t
#include <renderer/internal/vulkan/vulkan_object.h>
#include <renderer/dictionary.h>

typedef enum vulkan_render_scene_preset_type_t
{
	VULKAN_RENDER_SCENE_PRESET_TYPE_DEFAULT = 0,
	VULKAN_RENDER_SCENE_PRESET_TYPE_GEOMETRY_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_ALPHA_TESTED_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_OVERLAY_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_TRANSPARENT_ONLY,
} vulkan_render_scene_preset_type_t;

typedef struct vulkan_render_scene_create_info_t
{
	vulkan_render_queue_type_t* required_queues;
	u32 required_queue_count;
} vulkan_render_scene_create_info_t;

typedef struct vulkan_camera_t vulkan_camera_t;
typedef struct vulkan_light_t vulkan_light_t;

typedef buffer_t /* element_type: vulkan_camera_t* */ vulkan_camera_ptr_list_t;
typedef buffer_t /* element_type: vulkan_light_t* */ vulkan_light_ptr_list_t;

typedef struct vulkan_render_scene_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	dictionary_t /* key: vulkan_render_queue_type_t, value: vulkan_render_queue_t* */ queues;
	/* list of cameras in this render scene */
	vulkan_camera_ptr_list_t cameras;
	/* list of lights in this render scene */
	vulkan_light_ptr_list_t lights;
	bool is_use_lights;
} vulkan_render_scene_t;

/* performs dynamic casting (expensive), use only when you don't know the source type */
#define VULKAN_RENDER_SCENE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_render_scene_t*, VULKAN_OBJECT_TYPE_RENDER_SCENE, ptr)
#define VULKAN_RENDER_SCENE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_render_scene_t*, VULKAN_OBJECT_TYPE_RENDER_SCENE, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_RENDER_SCENE) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_RENDER_SCENE_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_render_scene_t*, VULKAN_OBJECT_TYPE_RENDER_SCENE, ptr)
#define VULKAN_RENDER_SCENE_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_render_scene_t*, VULKAN_OBJECT_TYPE_RENDER_SCENE, ptr)

/* constructors & destructors */
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info);
RENDERER_API void vulkan_render_scene_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info, vulkan_render_scene_t OUT scene);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_preset(vulkan_renderer_t* renderer, vulkan_render_scene_preset_type_t type);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_mask(vulkan_renderer_t* renderer, u64 mask);
RENDERER_API void vulkan_render_scene_destroy(vulkan_render_scene_t* scene);
RENDERER_API void vulkan_render_scene_release_resources(vulkan_render_scene_t* scene);

/* logic functions */

RENDERER_API void vulkan_render_scene_create_queue(vulkan_render_scene_t* scene, vulkan_render_queue_type_t queue_type);
typedef struct vulkan_render_queue_t vulkan_render_queue_t;
RENDERER_API void vulkan_render_scene_add_queue(vulkan_render_scene_t* scene, vulkan_render_queue_t* queue);
#define VULKAN_RENDER_SCENE_ALL_QUEUES (~0ULL)
#define VULKAN_RENDER_SCENE_DONT_CARE 1UL
#define VULKAN_RENDER_SCENE_CLEAR 0UL
RENDERER_API void vulkan_render_scene_render(vulkan_render_scene_t* scene, u64 queue_mask, u32 render_flags);
RENDERER_API vulkan_render_object_t* vulkan_render_scene_getH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
RENDERER_API vulkan_render_scene_object_handle_t vulkan_render_scene_create_object(vulkan_render_scene_t* scene, vulkan_render_object_type_t object_type, vulkan_render_queue_type_t queue_type);
RENDERER_API void vulkan_render_scene_destroy_objectH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
RENDERER_API void vulkan_render_scene_build_queues(vulkan_render_scene_t* scene);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_render_scene_add_camera(vulkan_render_scene_t* scene, vulkan_camera_t* camera) { buf_push(&scene->cameras, &camera); }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_render_scene_remove_camera(vulkan_render_scene_t* scene, vulkan_camera_t* camera)
{ 
	bool result = buf_remove(&scene->cameras, &camera, buf_ptr_comparer);
	if(!result)
		DEBUG_LOG_WARNING("You're trying to remove vulkan_camera_t from vulkan_render_scene_t which doesn't exists in it"); 
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_render_scene_add_light(vulkan_render_scene_t* scene, vulkan_light_t* light) { buf_push(&scene->lights, &light); }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_render_scene_remove_light(vulkan_render_scene_t* scene, vulkan_light_t* light)
{ 
	bool result = buf_remove(&scene->lights, &light, buf_ptr_comparer);
	if(!result)
		DEBUG_LOG_WARNING("You're trying to remove vulkan_light_t from vulkan_render_scene_t which doesn't exists in it"); 
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_render_scene_set_use_lights(vulkan_render_scene_t* scene, bool is_use_lights) { scene->is_use_lights = is_use_lights; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_render_scene_is_use_lights(vulkan_render_scene_t* scene) { return scene->is_use_lights; }
