/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: camera.h is a part of VulkanRenderer

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

/* API level object selection */
#ifdef SGE_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_camera.h>
	typedef vulkan_camera_t camera_t;
	typedef vulkan_render_scene_t render_scene_t;
	typedef vulkan_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN VULKAN_CAMERA_RENDER_TARGET_SCREEN
#elif SGE_OPENGL_DRIVER
	#include <renderer/internal/opengl/opengl_camera.h>
	typedef opengl_camera_t camera_t;
	typedef opengl_render_scene_t render_scene_t;
	typedef opengl_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN OPENGL_CAMERA_RENDER_TARGET_SCREEN
#elif SGE_DIRECTX_DRIVER
	#include <renderer/internal/directx/directx_camera.h>
	typedef directx_camera_t camera_t;
	typedef directx_render_scene_t render_scene_t;
	typedef directx_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN DIRECTX_CAMERA_RENDER_TARGET_SCREEN
#elif SGE_METAL_DRIVER
	#include <renderer/internal/metal/metal_camera.h>
	typedef metal_camera_t camera_t;
	typedef metal_render_scene_t render_scene_t;
	typedef metal_texture_t texture_t;
	#define CAMERA_RENDER_TARGET_SCREEN METAL_CAMERA_RENDER_TARGET_SCREEN
#endif

#include <renderer/defines.h>
#include <renderer/color.h>

#include <hpml/vec3.h>
#include <hpml/vec2.h>
#include <hpml/mat4.h>

/* NOTE: should be in sync with vulkan_camera_projection_type_t */
typedef enum camera_projection_type_t
{
	CAMERA_PROJECTION_TYPE_UNDEFINED,
	CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
	CAMERA_PROJECTION_TYPE_PERSPECTIVE,
	CAMERA_PROJECTION_TYPE_STEROGRAPHIC
} camera_projection_type_t;

typedef enum camera_render_target_type_t
{
	CAMERA_RENDER_TARGET_TYPE_UNDEFINED = 0,
	CAMERA_RENDER_TARGET_TYPE_COLOR,
	CAMERA_RENDER_TARGET_TYPE_DEPTH
} camera_render_target_type_t;

typedef enum camera_render_target_binding_type_t
{
	CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED,
	CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE
} camera_render_target_binding_type_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API camera_t* camera_new(memory_allocator_t* allocator);
SGE_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type);
SGE_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, camera_t OUT camera);
SGE_API void camera_destroy(camera_t* camera);
SGE_API void camera_release_resources(camera_t* camera);

/* logic functions */
SGE_API void camera_set_clear(camera_t* camera, color_t color, float depth);
SGE_API void camera_set_active(camera_t* camera, bool is_active);
SGE_API void camera_set_render_target(camera_t* camera, camera_render_target_type_t target_type, camera_render_target_binding_type_t binding_type, texture_t* texture);
SGE_API bool camera_is_active(camera_t* camera);
SGE_API void camera_render(camera_t* camera, render_scene_t* scene, u64 queue_mask);
SGE_API void camera_render_to_texture(camera_t* camera, render_scene_t* scene, texture_t* texture);

/* getters */
SGE_API mat4_t camera_get_view(camera_t* camera);
SGE_API mat4_t camera_get_transform(camera_t* camera);
SGE_API mat4_t camera_get_projection(camera_t* camera);
SGE_API vec3_t camera_get_position(camera_t* camera);
SGE_API vec3_t camera_get_rotation(camera_t* camera);
SGE_API vec2_t camera_get_clip_planes(camera_t* camera);
SGE_API float camera_get_near_clip_plane(camera_t* camera);
SGE_API float camera_get_far_clip_plane(camera_t* camera);
SGE_API float camera_get_field_of_view(camera_t* camera);
SGE_API float camera_get_height(camera_t* camera);

/* setters */
SGE_API void camera_set_transform(camera_t* camera, mat4_t transform);
SGE_API void camera_set_position(camera_t* camera, vec3_t position);
SGE_API void camera_set_rotation(camera_t* camera, vec3_t rotation);
SGE_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane);
SGE_API void camera_set_field_of_view(camera_t* camera, float fov);
SGE_API void camera_set_height(camera_t* camera, float height);
SGE_API void camera_set_render_area(camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height);
SGE_API void camera_set_render_area_relative(camera_t* camera, u32 offset_x, u32 offset_y, u32 width, u32 height);
SGE_API void camera_set_render_area_default(camera_t* camera);

END_CPP_COMPATIBLE