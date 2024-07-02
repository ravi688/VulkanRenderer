/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: camera.c is a part of VulkanRenderer

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


#include <renderer/camera.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/renderer.h>
#include <renderer/render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

#include <renderer/alloc.h>
#include <renderer/assert.h>

/* constructors & destructors */
SGE_API camera_t* camera_new(memory_allocator_t* allocator)
{
	return vulkan_camera_new(allocator);
}

SGE_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type)
{
	camera_t* camera = camera_new(renderer->allocator);
	camera_create_no_alloc(renderer, projection_type, camera);
	return camera;
}

SGE_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, camera_t OUT camera)
{
	render_pass_pool_t* pool = renderer_get_render_pass_pool(renderer);
	/* create a new path as we wan't to keep default camera pass isolated */
	vulkan_render_pass_pool_create_path(renderer->vulkan_driver->render_pass_pool);
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type),
		.swapchain_depth_clear_pass = render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN),
	};
	vulkan_camera_create_no_alloc(renderer->vulkan_handle, &create_info, camera);
}

SGE_API void camera_destroy(camera_t* camera)
{
	vulkan_camera_destroy(camera);
}

SGE_API void camera_release_resources(camera_t* camera)
{
	vulkan_camera_release_resources(camera);
}

/* logic functions */
SGE_API void camera_set_clear(camera_t* camera, color_t color, float depth)
{
	vulkan_camera_set_clear(camera, color, depth);
}

SGE_API void camera_set_active(camera_t* camera, bool is_active)
{
	vulkan_camera_set_active(camera, is_active);
}

SGE_API void camera_set_render_target(camera_t* camera, camera_render_target_type_t target_type, camera_render_target_binding_type_t binding_type, texture_t* texture)
{
	vulkan_camera_set_render_target(camera, REINTERPRET_TO(vulkan_camera_render_target_type_t, target_type),
											REINTERPRET_TO(vulkan_camera_render_target_binding_type_t, binding_type), texture);
}

SGE_API bool camera_is_active(camera_t* camera)
{
	return vulkan_camera_is_active(camera);
}

SGE_API void camera_render(camera_t* camera, render_scene_t* scene, u64 queue_mask)
{
	vulkan_camera_render(camera, scene, queue_mask);
}

SGE_API void camera_render_to_texture(camera_t* camera, render_scene_t* scene, texture_t* texture)
{
	vulkan_camera_render_to_texture(camera, scene, texture);
}

/* getters */

SGE_API mat4_t camera_get_view(camera_t* camera)
{
	return vulkan_camera_get_view(camera);
}

SGE_API mat4_t camera_get_transform(camera_t* camera)
{
	return vulkan_camera_get_transform(camera);
}

SGE_API mat4_t camera_get_projection(camera_t* camera)
{
	return vulkan_camera_get_projection(camera);
}

SGE_API vec3_t camera_get_position(camera_t* camera)
{
	return vulkan_camera_get_position(camera);
}

SGE_API vec3_t camera_get_rotation(camera_t* camera)
{
	return vulkan_camera_get_rotation(camera);
}

SGE_API vec2_t camera_get_clip_planes(camera_t* camera)
{
	return vulkan_camera_get_clip_planes(camera);
}

SGE_API float camera_get_near_clip_plane(camera_t* camera)
{
	return vulkan_camera_get_near_clip_plane(camera);
}

SGE_API float camera_get_far_clip_plane(camera_t* camera)
{
	return vulkan_camera_get_far_clip_plane(camera);
}

SGE_API float camera_get_field_of_view(camera_t* camera)
{
	return vulkan_camera_get_field_of_view(camera);
}

SGE_API float camera_get_height(camera_t* camera)
{
	return vulkan_camera_get_height(camera);
}


/* setters */

SGE_API void camera_set_transform(camera_t* camera, mat4_t transform)
{
	vulkan_camera_set_transform(camera, transform);
}

SGE_API void camera_set_position(camera_t* camera, vec3_t position)
{
	vulkan_camera_set_position(camera, position);
}

SGE_API void camera_set_rotation(camera_t* camera, vec3_t rotation)
{
	vulkan_camera_set_rotation(camera, rotation);
}

SGE_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	vulkan_camera_set_clip_planes(camera, near_clip_plane, far_clip_plane);
}

SGE_API void camera_set_field_of_view(camera_t* camera, float fov)
{
	vulkan_camera_set_field_of_view(camera, fov);
}

SGE_API void camera_set_height(camera_t* camera, float height)
{
	vulkan_camera_set_height(camera, height);
}

SGE_API void camera_set_render_area(camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height)
{
	vulkan_camera_set_render_area(camera, offset_x, offset_y, width, height);
}

SGE_API void camera_set_render_area_relative(camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height)
{
	vulkan_camera_set_render_area_relative(camera, offset_x, offset_y, width, height);
}

SGE_API void camera_set_render_area_default(camera_t* camera)
{
	vulkan_camera_set_render_area_default(camera);
}
