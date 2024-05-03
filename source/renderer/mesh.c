/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: mesh.c is a part of VulkanRenderer

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

#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>

#include <renderer/mesh.h>
#include <renderer/mesh3d.h>
#include <renderer/assert.h>
#include <renderer/alloc.h>

static VkIndexType get_vulkan_index_from_stride(u32 stride);

RENDERER_API mesh_t* mesh_new(memory_allocator_t* allocator)
{
	return vulkan_mesh_new(allocator);
}

RENDERER_API mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data)
{
	mesh_t* vulkan_mesh = vulkan_mesh_new(renderer->allocator);
	mesh_create_no_alloc(renderer, mesh_data, vulkan_mesh);
	return vulkan_mesh;
}

RENDERER_API void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh)
{
	debug_assert__(mesh_data != NULL, "Failed to create mesh_t, mesh3d_t* mesh_data == NULL\n");
	vulkan_vertex_buffer_create_info_t* vertex_buffer_infos = stack_newv(vulkan_vertex_buffer_create_info_t, MESH3D_MAX_ATTRIBUTE_COUNT);
	BUFFER** buffers = (BUFFER**)mesh_data;
	u8 buffer_count = 0;
	for(u8 i = 0; i < MESH3D_MAX_ATTRIBUTE_COUNT; i++)
	{
		if((buffers[i] == NULL) || (buffers[i]->element_count == 0)) continue;
		vulkan_vertex_buffer_create_info_t* create_info = &vertex_buffer_infos[buffer_count];
		create_info->data = buffers[i]->bytes;
		create_info->stride = buffers[i]->element_size;
		create_info->count = buffers[i]->element_count;
		create_info->binding = i;
		++buffer_count;
	}
	_debug_assert_wrn__(buffer_count != 0);

	vulkan_mesh_create_info_t create_info =
	{
		.vertex_buffer_infos = vertex_buffer_infos,
		.vertex_buffer_info_count = buffer_count
	};
	BUFFER* index_buffer = buffers[MESH3D_MAX_ATTRIBUTE_COUNT];
	if((index_buffer != NULL) && (index_buffer->element_count != 0))
	{
		create_info.index_buffer_info = (vulkan_index_buffer_create_info_t)
		{
			.data = index_buffer->bytes,
			.index_type = get_vulkan_index_from_stride(MESH3D_INDEX_SIZE),
			.count = index_buffer->element_count * 3 //element_size = sizeof(vec3_t(index_t)), and element_count = count of vec3_t(index_t)
		};
	}
	vulkan_mesh_create_no_alloc(renderer->handle, &create_info, mesh);
	stack_free(vertex_buffer_infos);
}

RENDERER_API void mesh_create_no_alloc_ext(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh)
{
	RDR_OBJECT_INIT(mesh, RDR_OBJECT_TYPE_MESH, RDR_OBJECT_NATIONALITY_EXTERNAL);
	mesh_create_no_alloc(renderer, mesh_data, mesh);
}

RENDERER_API void mesh_destroy(mesh_t* mesh)
{
	vulkan_mesh_destroy(mesh);
}

RENDERER_API void mesh_release_resources(mesh_t* mesh)
{
	vulkan_mesh_release_resources(mesh);
}

RENDERER_API void mesh_draw(mesh_t* mesh)
{
	vulkan_mesh_draw(mesh);
}

RENDERER_API void mesh_draw_indexed(mesh_t* mesh)
{
	vulkan_mesh_draw_indexed(mesh);
}

RENDERER_API void mesh_draw_instanced(mesh_t* mesh, uint32_t instance_count)
{
	vulkan_mesh_draw_instanced(mesh, instance_count);
}
RENDERER_API void mesh_draw_indexed_instanced(mesh_t* mesh, uint32_t instance_count)
{
	vulkan_mesh_draw_indexed_instanced(mesh, instance_count);
}


static VkIndexType get_vulkan_index_from_stride(u32 stride)
{
	switch(stride)
	{
		case sizeof(u16): return VK_INDEX_TYPE_UINT16;
		case sizeof(u32): return VK_INDEX_TYPE_UINT32;
		case sizeof(u8): return VK_INDEX_TYPE_UINT8_EXT;
		default:
		{
			LOG_FETAL_ERR("There is no Vulkan Index Type corresponding to the stride \"%u\"\n", stride);
			UNREACHABLE();
		}
	}
}

