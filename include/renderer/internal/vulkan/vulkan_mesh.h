/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_mesh.h is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <bufferlib/buffer.h>

// TODO: need to be refactored
enum
{
	// vertex attribute binding indices
	VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING = 0,
	VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING = 1,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING = 2,

	// vertex attribute location indices
	VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION = 0,
	VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION = 1,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION = 2
};

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef struct vulkan_vertex_buffer_create_info_t
{
	void* data;
	u32 stride;
	u32 count;
} vulkan_vertex_buffer_create_info_t;

typedef struct vulkan_index_buffer_create_info_t
{
	void* data;
	VkIndexType index_type;
	u32 count;
} vulkan_index_buffer_create_info_t;

typedef struct vulkan_mesh_create_info_t
{
	vulkan_vertex_buffer_create_info_t* vertex_buffer_infos;
	vulkan_vertex_buffer_create_info_t* instance_buffer_infos;
	u32 vertex_buffer_info_count;
	u32 instance_buffer_info_count;
	vulkan_index_buffer_create_info_t index_buffer_info;
} vulkan_mesh_create_info_t;

typedef struct vulkan_mesh_t
{
	vulkan_renderer_t* renderer;
	BUFFER /*typeof(vulkan_buffer_t*)*/ vertex_buffers;
	vulkan_buffer_t* index_buffer;
	VkIndexType index_type;

	u32 binding_index;  // INTERNAL 
} vulkan_mesh_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
vulkan_mesh_t* vulkan_mesh_new(memory_allocator_t* allocator);
vulkan_mesh_t* vulkan_mesh_create(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info);
void vulkan_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh);
void vulkan_mesh_destroy(vulkan_mesh_t* mesh);
void vulkan_mesh_release_resources(vulkan_mesh_t* mesh);


// draw calls
RENDERER_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh);
RENDERER_API void vulkan_mesh_draw(vulkan_mesh_t* mesh);
RENDERER_API void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count);
RENDERER_API void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count);
RENDERER_API void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count);

RENDERER_API void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info);
RENDERER_API void vulkan_mesh_bind_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer);
RENDERER_API void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh);

END_CPP_COMPATIBLE
