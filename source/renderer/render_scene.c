/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_scene.c is a part of VulkanRenderer

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


#include <renderer/render_scene.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_camera.h> /* for VULKAN_CAMERA_CAST */

/* constructors & destructors */
RENDERER_API render_scene_t* render_scene_new(memory_allocator_t* allocator)
{
	return vulkan_render_scene_new(allocator);
}

RENDERER_API render_scene_t* render_scene_create(renderer_t* renderer, render_scene_create_info_t* create_info)
{
	if(create_info != NULL)
	{
		vulkan_render_scene_create_info_t vcreate_info = 
		{
			.required_queues = CAST_TO(vulkan_render_queue_type_t*, create_info->required_queues),
			.required_queue_count = create_info->required_queue_count
		};
		return vulkan_render_scene_create(renderer->vulkan_handle, &vcreate_info);
	}
	else
		return vulkan_render_scene_create(renderer->vulkan_handle, NULL);
}

RENDERER_API void render_scene_create_no_alloc(renderer_t* renderer, render_scene_create_info_t* create_info, render_scene_t OUT scene)
{
	if(create_info != NULL)
	{
		vulkan_render_scene_create_info_t vcreate_info = 
		{
			.required_queues = CAST_TO(vulkan_render_queue_type_t*, create_info->required_queues),
			.required_queue_count = create_info->required_queue_count
		};
		vulkan_render_scene_create_no_alloc(renderer->vulkan_handle, &vcreate_info, scene);	
	}
	else
		vulkan_render_scene_create_no_alloc(renderer->vulkan_handle, NULL, scene);
}

RENDERER_API render_scene_t* render_scene_create_from_preset(renderer_t* renderer, render_scene_preset_type_t type)
{
	return vulkan_render_scene_create_from_preset(renderer->vulkan_handle, CAST_TO(vulkan_render_scene_preset_type_t, type));
}

RENDERER_API render_scene_t* render_scene_create_from_mask(renderer_t* renderer, u64 mask)
{
	return vulkan_render_scene_create_from_mask(renderer->vulkan_handle, mask);
}

RENDERER_API void render_scene_destroy(render_scene_t* scene)
{
	vulkan_render_scene_destroy(scene);
}

RENDERER_API void render_scene_release_resources(render_scene_t* scene)
{
	vulkan_render_scene_release_resources(scene);
}

/* logic functions */

RENDERER_API void render_scene_create_queue(render_scene_t* scene, render_queue_type_t queue_type)
{
	vulkan_render_scene_create_queue(scene, REINTERPRET_TO(vulkan_render_queue_type_t, queue_type));
}

RENDERER_API void render_scene_add_queue(render_scene_t* scene, render_queue_t* queue)
{
	vulkan_render_scene_add_queue(scene, VULKAN_RENDER_QUEUE(queue));
}

RENDERER_API void render_scene_render(render_scene_t* scene, u64 queue_mask, u32 flags)
{
	vulkan_render_scene_render(scene, queue_mask, flags);
}

RENDERER_API render_object_t* render_scene_getH(render_scene_t* scene, render_scene_object_handle_t handle)
{
	return vulkan_render_scene_getH(scene, handle);
}

RENDERER_API render_scene_object_handle_t render_scene_create_object(render_scene_t* scene, render_object_type_t object_type, render_queue_type_t queue_type)
{
	return vulkan_render_scene_create_object(scene, object_type, queue_type);
}

RENDERER_API void render_scene_destroy_objectH(render_scene_t* scene, render_scene_object_handle_t handle)
{
	vulkan_render_scene_destroy_objectH(scene, handle);
}

RENDERER_API void render_scene_build_queues(render_scene_t* scene)
{
	vulkan_render_scene_build_queues(scene);
}

RENDERER_API void render_scene_add_camera(render_scene_t* scene, camera_t* camera)
{
	vulkan_render_scene_add_camera(VULKAN_RENDER_SCENE_CAST(scene), VULKAN_CAMERA_CAST(camera));
}

RENDERER_API void render_scene_remove_camera(render_scene_t* scene, camera_t* camera)
{
	vulkan_render_scene_remove_camera(VULKAN_RENDER_SCENE_CAST(scene), VULKAN_CAMERA_CAST(camera));
}
