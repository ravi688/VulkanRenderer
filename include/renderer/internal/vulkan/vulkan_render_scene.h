/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_scene.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
#include <renderer/internal/vulkan/vulkan_handles.h> 				// for vulkan_render_scene_handle_t
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

typedef struct vulkan_render_scene_t
{
	vulkan_renderer_t* renderer;
	dictionary_t queues;
} vulkan_render_scene_t;

/* constructors & destructors */
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info);
RENDERER_API void vulkan_render_scene_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info, vulkan_render_scene_t OUT scene);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_preset(vulkan_renderer_t* renderer, vulkan_render_scene_preset_type_t type);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_mask(vulkan_renderer_t* renderer, u64 mask);
RENDERER_API void vulkan_render_scene_destroy(vulkan_render_scene_t* scene);
RENDERER_API void vulkan_render_scene_release_resources(vulkan_render_scene_t* scene);

/* logic functions */

RENDERER_API void vulkan_render_scene_add_queue(vulkan_render_scene_t* scene, vulkan_render_queue_type_t queue_type);
#define VULKAN_RENDER_SCENE_ALL_QUEUES (~0ULL)
#define VULKAN_RENDER_SCENE_DONT_CARE 1UL
#define VULKAN_RENDER_SCENE_CLEAR 0UL
RENDERER_API void vulkan_render_scene_render(vulkan_render_scene_t* scene, u64 queue_mask, u32 render_flags);
RENDERER_API vulkan_render_object_t* vulkan_render_scene_getH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
RENDERER_API vulkan_render_scene_object_handle_t vulkan_render_scene_create_object(vulkan_render_scene_t* scene, vulkan_render_object_type_t object_type, vulkan_render_queue_type_t queue_type);
RENDERER_API void vulkan_render_scene_destroy_objectH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
RENDERER_API void vulkan_render_scene_build_queues(vulkan_render_scene_t* scene);
