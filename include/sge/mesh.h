/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: mesh.h is a part of VulkanRenderer

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
#include <sge/rdr_object.h>

typedef struct mesh3d_t mesh3d_t;

#ifdef SGE_VULKAN_DRIVER
	#include <sge/internal/vulkan/vulkan_mesh.h>
	typedef vulkan_mesh_t mesh_t;
	#define MESH(ptr) VULKAN_MESH(ptr)
	typedef struct vulkan_buffer_t vulkan_buffer_t;
	typedef vulkan_buffer_t sge_buffer_t;
#elif SGE_OPENGL_DRIVER
	#include <sge/internal/opengl/opengl_mesh.h>
	typedef opengl_mesh_t mesh_t;
	#define MESH(ptr) OPENGL_MESH(ptr)
	typedef struct opengl_buffer_t opengl_buffer_t;
	typedef opengl_buffer_t sge_buffer_t;
#elif SGE_DIRECTX_DRIVER
	#include <sge/internal/directx/directx_mesh.h>
	typedef directx_mesh_t mesh_t;
	#define MESH(ptr) DIRECTX_MESH(ptr)
	typedef struct directx_buffer_t directx_buffer_t;
	typedef directx_buffer_t sge_buffer_t;
#elif SGE_METAL_DRIVER
	#include <sge/internal/metal/metal_mesh.h>
	typedef metal_mesh_t mesh_t;
	#define MESH(ptr) METAL_MESH(ptr)
	typedef struct metal_buffer_t metal_buffer_t;
	typedef metal_buffer_t sge_buffer_t;
#endif


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API mesh_t* mesh_new(memory_allocator_t* allocator);
SGE_API mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data);
SGE_API void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh);
SGE_API void mesh_create_no_alloc_ext(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh);
SGE_API void mesh_destroy(mesh_t* mesh);
SGE_API void mesh_release_resources(mesh_t* mesh);

/* vertex and index buffer data update calls */
SGE_API void mesh_add_vertex_buffer(mesh_t* mesh, sge_buffer_t* buffer, u32 binding);
typedef struct vertex_buffer_create_info_t
{
	void* data;
	u32 stride;
	u32 count;
	u32 binding;
} vertex_buffer_create_info_t;
SGE_API void mesh_create_and_add_vertex_buffer(mesh_t* mesh, vertex_buffer_create_info_t* create_info);
typedef enum index_type_t
{
	INDEX_TYPE_UNDEFINED = 0,
	INDEX_TYPE_U16,
	INDEX_TYPE_U32
} index_type_t;
/* NOTE: the index buffer must have buffer_traits_t set */
SGE_API void mesh_set_index_buffer(mesh_t* mesh, sge_buffer_t* buffer, index_type_t type);
typedef struct index_buffer_create_info_t
{
	void* data;
	index_type_t index_type;
	u32 count;
} index_buffer_create_info_t;
SGE_API void mesh_create_and_set_index_buffer(mesh_t* mesh, index_buffer_create_info_t* create_info);
SGE_API sge_buffer_t* mesh_get_index_buffer(mesh_t* mesh);
SGE_API sge_buffer_t* mesh_get_vertex_buffer_at(mesh_t* mesh, u32 index);

/* draw calls */
SGE_API DEPRECATED void mesh_draw(mesh_t* mesh);
SGE_API void mesh_draw_indexed(mesh_t* mesh);
SGE_API void mesh_draw_instanced(mesh_t* mesh, u32 instance_count);
SGE_API void mesh_draw_indexed_instanced(mesh_t* mesh, u32 instance_count);

END_CPP_COMPATIBLE
