/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_mesh.h is a part of VulkanRenderer

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

#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_object.h>

// TODO: need to be refactored
enum
{
	// vertex attribute binding indices
	VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING = 0,
	VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING = 1,
	VULKAN_MESH_VERTEX_ATTRIBUTE_COLOR_BINDING = 2 ,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING = 3,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TANGENT_BINDING = 4,

	// vertex attribute location indices
	VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION = 0,
	VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION = 1,
	VULKAN_MESH_VERTEX_ATTRIBUTE_COLOR_LOCATION = 2,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION = 3,
	VULKAN_MESH_VERTEX_ATTRIBUTE_TANGENT_LOCATION = 4
};

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef struct vulkan_vertex_buffer_create_info_t
{
	void* data;
	u32 stride;
	u32 count;
	u32 binding;
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
	u32 vertex_buffer_info_count;
	vulkan_index_buffer_create_info_t index_buffer_info;
} vulkan_mesh_create_info_t;

typedef struct vulkan_buffer_t vulkan_buffer_t;
typedef vulkan_buffer_t* vulkan_buffer_ptr_t;

typedef struct vulkan_vertex_buffer_t
{
	vulkan_buffer_ptr_t buffer;
	u32 binding;
	/* holds true if this vulkan buffer has been created internally by calling vulkan_buffer_destroy, 
	 * this extra information is needed for graceful vulkan object deallocation */
	bool is_internal;
} vulkan_vertex_buffer_t;

/* list of vulkan_vertex_buffer_t */
typedef buffer_t vulkan_vertex_buffer_list_t;

typedef struct vulkan_index_buffer_t
{
	vulkan_buffer_ptr_t buffer;
	VkIndexType index_type;
	/* holds true if this vulkan buffer has been created internally, otherwise false, i.e.
	 * if the vulkan buffer has been set using vulkan_mesh_set_index_buffer */
	bool is_internal;
} vulkan_index_buffer_t;

typedef struct vulkan_material_t vulkan_material_t;

typedef struct vulkan_mesh_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_material_t* material;

	/* list of pointer to the vertex buffers */
	vulkan_vertex_buffer_list_t vertex_buffers;

	/* index buffer may be NULL also if not provided by the user */
	vulkan_index_buffer_t index_buffer;

	u32 binding_index;  // INTERNAL
} vulkan_mesh_t;

#define VULKAN_MESH(ptr) VULKAN_OBJECT_UP_CAST(vulkan_mesh_t*, VULKAN_OBJECT_TYPE_MESH, ptr)
#define VULKAN_MESH_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_mesh_t*, VULKAN_OBJECT_TYPE_MESH, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_mesh_t* vulkan_mesh_new(memory_allocator_t* allocator);
RENDERER_API vulkan_mesh_t* vulkan_mesh_create(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info);
RENDERER_API void vulkan_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh);
RENDERER_API void vulkan_mesh_destroy(vulkan_mesh_t* mesh);
RENDERER_API void vulkan_mesh_release_resources(vulkan_mesh_t* mesh);


/* draw calls */

/* binds vertex buffers and index buffer as well, and issues a vulkan_mesh_draw_indexed_instanced call with instanced_count = 1 */
RENDERER_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh);
/* binds only vertex buffers, and issues a vulkan_mesh_draw_instanced call with instance count = 1 */
RENDERER_API void vulkan_mesh_draw(vulkan_mesh_t* mesh);
/* binds vertex buffers and index buffer as well, and issues a vkCmdDrawIndexed call */
RENDERER_API void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count);
/* binds index buffer only and issues a vkCmdDrawIndexed call */
RENDERER_API void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count);
/* binds vertex buffesr, and isues a vkCmdDraw call */
RENDERER_API void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count);

RENDERER_API void vulkan_mesh_set_material(vulkan_mesh_t* mesh, vulkan_material_t* material);

/* adds a vertex buffer to the mesh with binding as 'binding'
 * NOTE: it's more efficient to pass 'binding' as consecutive integers (>= 0)
 * Because, this function internally groups consecutive bindings and binds the vertex buffers with consecutive bindings in one vkCmdBindVertexBuffers call */
RENDERER_API void vulkan_mesh_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, u32 binding);
RENDERER_API void vulkan_mesh_set_index_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, VkIndexType vo_type);
/* similar to the above function but it creates the buffer internally with a create info and calls the above function to add it */
RENDERER_API void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info);
/* binds all the vertex buffers this mesh has */
RENDERER_API void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh);

END_CPP_COMPATIBLE
