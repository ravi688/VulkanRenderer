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


#include <sge/defines.h>

#if 	defined(SGE_VULKAN_DRIVER)
	typedef struct vulkan_render_scene_t vulkan_render_scene_t;
	typedef vulkan_render_scene_t render_scene_t;
	typedef struct vulkan_camera_t vulkan_camera_t;
	typedef vulkan_camera_t camera_t;
	typedef struct vulkan_light_t vulkan_light_t;
	typedef vulkan_light_t light_t;
	#include <sge/internal/vulkan/vulkan_handles.h>
	typedef vulkan_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID VULKAN_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <sge/internal/vulkan/vulkan_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES VULKAN_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR VULKAN_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE VULKAN_RENDER_SCENE_DONT_CARE

#elif 	defined(SGE_OPENGL_DRIVER)
	typedef struct opengl_render_scene_t opengl_render_scene_t;
	typedef opengl_render_scene_t render_scene_t;
	typedef struct opengl_camera_t opengl_camera_t;
	typedef opengl_camera_t camera_t;
	typedef struct opengl_light_t opengl_light_t;
	typedef opengl_light_t light_t;
	#include <sge/internal/opengl/opengl_handles.h>
	typedef opengl_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID OPENGL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <sge/internal/opengl/opengl_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES OPENGL_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR OPENGL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE OPENGL_RENDER_SCENE_DONT_CARE

#elif  	defined(SGE_DIRECTX_DRIVER)
	typedef struct directx_render_scene_t directx_render_scene_t;
	typedef directx_render_scene_t render_scene_t;
	typedef struct directx_camera_t directx_camera_t;
	typedef directx_camera_t camera_t;
	typedef struct directx_light_t directx_light_t;
	typedef directx_light_t light_t;
	#include <sge/internal/directx/directx_handles.h>
	typedef directx_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID DIRECTX_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <sge/internal/directx/directx_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES DIRECTX_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR DIRECTX_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE DIRECTX_RENDER_SCENE_DONT_CARE

#elif  	defined(SGE_METAL_DRIVER)
	typedef struct metal_render_scene_t metal_render_scene_t;
	typedef metal_render_scene_t render_scene_t;
	typedef struct metal_camera_t metal_camera_t;
	typedef metal_camera_t camera_t;
	typedef struct metal_light_t metal_light_t;
	typedef metal_light_t light_t;
	#include <sge/internal/metal/metal_handles.h>
	typedef metal_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID METAL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <sge/internal/metal/metal_render_scene.h>
	#define RENDER_SCENE_ALL_QUEUES METAL_RENDER_SCENE_ALL_QUEUES
	#define RENDER_SCENE_CLEAR METAL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE METAL_RENDER_SCENE_DONT_CARE

#endif

#include <sge/render_object.h>
#include <sge/render_queue.h>

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

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API render_scene_t* render_scene_new(memory_allocator_t* allocator);
SGE_API render_scene_t* render_scene_create(renderer_t* renderer, render_scene_create_info_t* create_info);
SGE_API void render_scene_create_no_alloc(renderer_t* renderer, render_scene_create_info_t* create_info, render_scene_t OUT scene);
SGE_API render_scene_t* render_scene_create_from_preset(renderer_t* renderer, render_scene_preset_type_t type);
SGE_API render_scene_t* render_scene_create_from_mask(renderer_t* renderer, u64 queue_mask);
SGE_API void render_scene_destroy(render_scene_t* scene);
SGE_API void render_scene_release_resources(render_scene_t* scene);

/* logic functions */
SGE_API void render_scene_create_queue(render_scene_t* scene, render_queue_type_t queue_type);
SGE_API void render_scene_add_queue(render_scene_t* scene, render_queue_t* queue);
SGE_API void render_scene_render(render_scene_t* scene, u64 queue_mask, u32 flags);
SGE_API render_object_t* render_scene_getH(render_scene_t* scene, render_scene_object_handle_t handle);
SGE_API render_scene_object_handle_t render_scene_create_object(render_scene_t* scene, render_object_type_t object_type, render_queue_type_t queue_type);
SGE_API void render_scene_destroy_objectH(render_scene_t* scene, render_scene_object_handle_t handle);
SGE_API void render_scene_destroy_object(render_scene_t* scene, render_object_t* obj);
SGE_API void render_scene_build_queues(render_scene_t* scene);
SGE_API void render_scene_add_camera(render_scene_t* scene, camera_t* camera);
SGE_API void render_scene_remove_camera(render_scene_t* scene, camera_t* camera);
SGE_API void render_scene_add_light(render_scene_t* scene, light_t* light);
SGE_API void render_scene_remove_light(render_scene_t* scene, light_t* light);

SGE_API void render_scene_set_use_lights(render_scene_t* scene, bool is_use_lights);

END_CPP_COMPATIBLE
