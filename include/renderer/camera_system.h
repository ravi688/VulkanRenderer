/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: camera_system.h is a part of VulkanRenderer

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

#include<renderer/defines.h>

#ifdef SGE_VULKAN_DRIVER
	typedef struct vulkan_camera_system_t vulkan_camera_system_t;
	typedef vulkan_camera_system_t camera_system_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_camera_handle_t camera_handle_t;
#elif defined(SGE_OPENGL_DRIVER)
	typedef struct opengl_camera_system_t opengl_camera_system_t;
	typedef opengl_camera_system_t camera_system_t;
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_camera_handle_t camera_handle_t;
#elif defined(SGE_DIRECTX_DRIVER)
	typedef struct directx_camera_system_t directx_camera_system_t;
	typedef directx_camera_system_t camera_system_t;
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_camera_handle_t camera_handle_t;
#elif defined(SGE_METAL_DRIVER)
	typedef struct metal_camera_system_t metal_camera_system_t;
	typedef metal_camera_system_t camera_system_t;
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_camera_handle_t camera_handle_t;
#endif

#include <renderer/camera.h> 		// camera_projection_type_t

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API camera_system_t* camera_system_new(memory_allocator_t* allocator);
SGE_API camera_system_t* camera_system_create(renderer_t* renderer);
SGE_API void camera_system_destroy(camera_system_t* system);
SGE_API void camera_system_release_resources(camera_system_t* system);

/* getters */
SGE_API u32 camera_system_get_count(camera_system_t* system);
SGE_API camera_t* camera_system_get_at(camera_system_t* system, buf_ucount_t index);
SGE_API camera_t* camera_system_getH(camera_system_t* system, camera_handle_t handle);


/* logic functions */
SGE_API camera_handle_t camera_system_create_camera(camera_system_t* system, camera_projection_type_t projection_type);
SGE_API void camera_system_destroy_cameraH(camera_system_t* system, camera_handle_t handle);

END_CPP_COMPATIBLE

