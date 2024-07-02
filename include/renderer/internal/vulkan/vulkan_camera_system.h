/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_camera_system.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_handles.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_camera_system_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	library_t lib;
} vulkan_camera_system_t;

#define VULKAN_CAMERA_SYSTEM(ptr) VULKAN_OBJECT_UP_CAST(vulkan_camera_system_t*, VULKAN_OBJECT_TYPE_CAMERA_SYSTEM, ptr)
#define VULKAN_CAMERA_SYSTEM_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_camera_system_t*, VULKAN_OBJECT_TYPE_CAMERA_SYSTEM, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API vulkan_camera_system_t* vulkan_camera_system_new(memory_allocator_t* allocator);
SGE_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer);
SGE_API void vulkan_camera_system_destroy(vulkan_camera_system_t* system);
SGE_API void vulkan_camera_system_release_resources(vulkan_camera_system_t* system);

/* getters */
static FORCE_INLINE u32 vulkan_camera_system_get_count(vulkan_camera_system_t* system) 
{
	return library_get_count(&system->lib);
}
static FORCE_INLINE vulkan_camera_t* vulkan_camera_system_get_at(vulkan_camera_system_t* system, buf_ucount_t index)
{
	return library_get_at(&system->lib, index);
}
SGE_API vulkan_camera_t* vulkan_camera_system_getH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);


/* logic functions */
SGE_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type);
SGE_API void vulkan_camera_system_destroy_cameraH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);

END_CPP_COMPATIBLE
