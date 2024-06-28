/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_queue.c is a part of VulkanRenderer

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


#include <renderer/render_queue.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_render_object.h> /* VULKAN_RENDER_OBJECT_CAST() casting */
#include <renderer/internal/vulkan/vulkan_render_scene.h> /* VULKAN_RENDER_SCENE_CAST() casting */

RENDERER_API render_queue_t* render_queue_new(memory_allocator_t* allocator)
{
	return vulkan_render_queue_new(allocator);
}

RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, render_queue_type_t type)
{
	return vulkan_render_queue_create(renderer->vulkan_handle, CAST_TO(vulkan_render_queue_type_t, type));
}

RENDERER_API void render_queue_create_no_alloc(renderer_t* renderer, render_queue_type_t type, render_queue_t OUT queue)
{
	vulkan_render_queue_create_no_alloc(renderer->vulkan_handle, CAST_TO(vulkan_render_queue_type_t, type), queue);
}

RENDERER_API void render_queue_destroy(render_queue_t* queue)
{
	vulkan_render_queue_destroy(queue);
}

RENDERER_API void render_queue_release_resources(render_queue_t* queue)
{
	vulkan_render_queue_release_resources(queue);
}


/* logic functions */
RENDERER_API void render_queue_add(render_queue_t* queue, render_object_t* obj)
{
	return vulkan_render_queue_add(queue, VULKAN_RENDER_OBJECT_CAST(obj));
}

RENDERER_API void render_queue_removeH(render_queue_t* queue, render_object_t* obj)
{
	vulkan_render_queue_removeH(queue, VULKAN_RENDER_OBJECT_CAST(obj));
}

RENDERER_API void render_queue_build(render_queue_t* queue)
{
	vulkan_render_queue_build(queue);
}

RENDERER_API void render_queue_dispatch(render_queue_t* queue, camera_t* camera, render_scene_t* scene)
{
	vulkan_render_queue_dispatch(queue, CAST_TO(vulkan_camera_t*, camera), VULKAN_RENDER_SCENE_CAST(scene));
}

