/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: mesh.h is a part of VulkanRenderer

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

typedef struct mesh3d_t mesh3d_t;

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_mesh_t vulkan_mesh_t;
	typedef vulkan_mesh_t mesh_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_mesh_t opengl_mesh_t;
	typedef opengl_mesh_t mesh_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_mesh_t directx_mesh_t;
	typedef directx_mesh_t mesh_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_mesh_t metal_mesh_t;
	typedef metal_mesh_t mesh_t;
#endif


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API mesh_t* mesh_new(memory_allocator_t* allocator);
RENDERER_API mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data);
RENDERER_API void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh);
RENDERER_API void mesh_destroy(mesh_t* mesh);
RENDERER_API void mesh_release_resources(mesh_t* mesh);

/* draw calls */
RENDERER_API DEPRECATED void mesh_draw(mesh_t* mesh);
RENDERER_API void mesh_draw_indexed(mesh_t* mesh);
RENDERER_API void mesh_draw_instanced(mesh_t* mesh, u32 instance_count);
RENDERER_API void mesh_draw_indexed_instanced(mesh_t* mesh, u32 instance_count);

END_CPP_COMPATIBLE
