/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_camera_system.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_handles.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>

typedef struct vulkan_camera_system_t
{
	vulkan_renderer_t* renderer;
	library_t lib;
} vulkan_camera_system_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API vulkan_camera_system_t* vulkan_camera_system_new(memory_allocator_t* allocator);
RENDERER_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_camera_system_destroy(vulkan_camera_system_t* system);
RENDERER_API void vulkan_camera_system_release_resources(vulkan_camera_system_t* system);

/* getters */
static FORCE_INLINE u32 vulkan_camera_system_get_count(vulkan_camera_system_t* system) 
{
	return library_get_count(&system->lib);
}
static FORCE_INLINE vulkan_camera_t* vulkan_camera_system_get_at(vulkan_camera_system_t* system, buf_ucount_t index)
{
	return library_get_at(&system->lib, index);
}
RENDERER_API vulkan_camera_t* vulkan_camera_system_getH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);


/* logic functions */
RENDERER_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type);
RENDERER_API void vulkan_camera_system_destroy_cameraH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);

END_CPP_COMPATIBLE
