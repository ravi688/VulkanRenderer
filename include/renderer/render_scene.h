/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_scene.h is a part of VulkanRenderer

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

#if 	defined(RENDERER_VULKAN_DRIVER)
	typedef struct vulkan_render_scene_t vulkan_render_scene_t;
	typedef vulkan_render_scene_t render_scene_t;
	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;
	typedef struct vulkan_light_t vulkan_light_t;
	typedef vulkan_light_t light_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID VULKAN_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/vulkan/vulkan_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES VULKAN_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR VULKAN_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE VULKAN_RENDER_SCENE_DONT_CARE

#elif 	defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_render_scene_t opengl_render_scene_t;
	typedef opengl_render_scene_t render_scene_t;
	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;
	typedef struct opengl_light_t opengl_light_t;
	typedef opengl_light_t light_t;
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID OPENGL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/opengl/opengl_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES OPENGL_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR OPENGL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE OPENGL_RENDER_SCENE_DONT_CARE

#elif  	defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_render_scene_t directx_render_scene_t;
	typedef directx_render_scene_t render_scene_t;
	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;
	typedef struct directx_light_t directx_light_t;
	typedef directx_light_t light_t;
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID DIRECTX_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/directx/directx_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES DIRECTX_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR DIRECTX_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE DIRECTX_RENDER_SCENE_DONT_CARE

#elif  	defined(RENDERER_METAL_DRIVER)
	typedef struct metal_render_scene_t metal_render_scene_t;
	typedef metal_render_scene_t render_scene_t;
	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;
	typedef struct metal_light_t metal_light_t;
	typedef metal_light_t light_t;
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID METAL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/metal/metal_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES METAL_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR METAL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE METAL_RENDER_SCENE_DONT_CARE

#endif

#include <renderer/render_object.h>
#include <renderer/render_queue.h>

typedef enum render_scene_preset_type_t
{
	RENDER_SCENE_PRESET_TYPE_DEFAULT = 0,
	RENDER_SCENE_PRESET_TYPE_GEOMETRY_ONLY,
	RENDER_SCENE_PRESET_TYPE_ALPHA_TESTED_ONLY,
	RENDER_SCENE_PRESET_TYPE_OVERLAY_ONLY,
	RENDER_SCENE_PRESET_TYPE_TRANSPARENT_ONLY,
} render_scene_preset_type_t;

typedef struct render_scene_create_info_t
{
	render_queue_type_t* required_queues;
	u32 required_queue_count;
} render_scene_create_info_t;

/* constructors & destructors */
RENDERER_API render_scene_t* render_scene_new(memory_allocator_t* allocator);
RENDERER_API render_scene_t* render_scene_create(renderer_t* renderer, render_scene_create_info_t* create_info);
RENDERER_API void render_scene_create_no_alloc(renderer_t* renderer, render_scene_create_info_t* create_info, render_scene_t OUT scene);
RENDERER_API render_scene_t* render_scene_create_from_preset(renderer_t* renderer, render_scene_preset_type_t type);
RENDERER_API render_scene_t* render_scene_create_from_mask(renderer_t* renderer, u64 queue_mask);
RENDERER_API void render_scene_destroy(render_scene_t* scene);
RENDERER_API void render_scene_release_resources(render_scene_t* scene);

/* logic functions */
RENDERER_API void render_scene_create_queue(render_scene_t* scene, render_queue_type_t queue_type);
RENDERER_API void render_scene_add_queue(render_scene_t* scene, render_queue_t* queue);
RENDERER_API void render_scene_render(render_scene_t* scene, u64 queue_mask, u32 flags);
RENDERER_API render_object_t* render_scene_getH(render_scene_t* scene, render_scene_object_handle_t handle);
RENDERER_API render_scene_object_handle_t render_scene_create_object(render_scene_t* scene, render_object_type_t object_type, render_queue_type_t queue_type);
RENDERER_API void render_scene_destroy_objectH(render_scene_t* scene, render_scene_object_handle_t handle);
RENDERER_API void render_scene_build_queues(render_scene_t* scene);
RENDERER_API void render_scene_add_camera(render_scene_t* scene, camera_t* camera);
RENDERER_API void render_scene_remove_camera(render_scene_t* scene, camera_t* camera);
RENDERER_API void render_scene_add_light(render_scene_t* scene, light_t* light);
RENDERER_API void render_scene_remove_light(render_scene_t* scene, light_t* light);

RENDERER_API void render_scene_set_use_lights(render_scene_t* scene, bool is_use_lights);
