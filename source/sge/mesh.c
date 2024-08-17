/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: mesh.c is a part of VulkanRenderer

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

#include <sge/sge.h>
#include <sge/internal/vulkan/vulkan_mesh.h>
#include <sge/internal/vulkan/vulkan_buffer.h> /* for VULKAN_BUFFER_CAST() */

#include <sge/mesh.h>
#include <sge/mesh3d.h>
#include <sge/assert.h>
#include <sge/alloc.h>

static VkIndexType get_vulkan_index_from_stride(u32 stride);

SGE_API mesh_t* mesh_new(memory_allocator_t* allocator)
{
	return vulkan_mesh_new(allocator);
}

SGE_API mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data)
{
	mesh_t* vulkan_mesh = vulkan_mesh_new(renderer->allocator);
	mesh_create_no_alloc(renderer, mesh_data, vulkan_mesh);
	return vulkan_mesh;
}

SGE_API void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh)
{
	vulkan_mesh_create_info_t create_info = { };
	
	/* if mesh3d data is not NULL then create vertex buffers and index buffers from mesh3d */
	if(mesh_data != NULL)
	{
		vulkan_vertex_buffer_create_info_t vertex_buffer_infos[MESH3D_MAX_ATTRIBUTE_COUNT];
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

		create_info.vertex_buffer_infos = vertex_buffer_infos,
		create_info.vertex_buffer_info_count = buffer_count;

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
	}
	/* otherwise create vulkan_mesh_t with no vertex buffers or index buffers */
	else vulkan_mesh_create_no_alloc(renderer->handle, &create_info, mesh);
}

SGE_API void mesh_create_no_alloc_ext(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh)
{
	RDR_OBJECT_INIT(mesh, RDR_OBJECT_TYPE_MESH, RDR_OBJECT_NATIONALITY_EXTERNAL);
	mesh_create_no_alloc(renderer, mesh_data, mesh);
}

SGE_API void mesh_destroy(mesh_t* mesh)
{
	vulkan_mesh_destroy(mesh);
}

SGE_API void mesh_release_resources(mesh_t* mesh)
{
	vulkan_mesh_release_resources(mesh);
}

SGE_API void mesh_set_instance_count(mesh_t* mesh, u32 instance_count)
{
	vulkan_mesh_set_instance_count(VULKAN_MESH_CAST(mesh), instance_count);
}

SGE_API u32 mesh_get_instance_count(mesh_t* mesh)
{
	return vulkan_mesh_get_instance_count(VULKAN_MESH_CAST(mesh));
}

SGE_API void mesh_add_vertex_buffer(mesh_t* mesh, sge_buffer_t* buffer, u32 binding)
{
	vulkan_mesh_add_vertex_buffer(VULKAN_MESH_CAST(mesh), VULKAN_BUFFER_CAST(buffer), binding);
}

SGE_API void mesh_create_and_add_vertex_buffer(mesh_t* mesh, vertex_buffer_create_info_t* create_info)
{
	vulkan_mesh_create_and_add_vertex_buffer(VULKAN_MESH_CAST(mesh), REINTERPRET_CAST(vulkan_vertex_buffer_create_info_t*, create_info));
}

SGE_API void mesh_destroy_vertex_buffers(mesh_t* mesh)
{
	vulkan_mesh_destroy_vertex_buffers(VULKAN_MESH_CAST(mesh));
}

SGE_API void mesh_destroy_vertex_buffer(mesh_t* mesh, u32 binding)
{
	vulkan_mesh_destroy_vertex_buffer(VULKAN_MESH_CAST(mesh), binding);
}

static VkIndexType get_vulkan_index_type_from_index_type(index_type_t type)
{
	switch(type)
	{
		case INDEX_TYPE_U16: return VK_INDEX_TYPE_UINT16;
		case INDEX_TYPE_U32: return VK_INDEX_TYPE_UINT32;
		default: DEBUG_LOG_FETAL_ERROR("Unsupport index type: %u", type);
	}
	return 0;
}

SGE_API void mesh_set_index_buffer(mesh_t* mesh, sge_buffer_t* buffer, index_type_t type)
{
	vulkan_mesh_set_index_buffer(VULKAN_MESH_CAST(mesh), VULKAN_BUFFER_CAST(buffer), get_vulkan_index_type_from_index_type(type));
}

SGE_API void mesh_destroy_index_buffer(mesh_t* mesh)
{
	vulkan_mesh_destroy_index_buffer(VULKAN_MESH_CAST(mesh));
}

SGE_API void mesh_create_and_set_index_buffer(mesh_t* mesh, index_buffer_create_info_t* create_info)
{
	vulkan_index_buffer_create_info_t v_create_info =
	{
		.data = create_info->data,
		.index_type = get_vulkan_index_type_from_index_type(create_info->index_type),
		.count = create_info->count
	};
	vulkan_mesh_create_and_set_index_buffer(VULKAN_MESH_CAST(mesh), &v_create_info);
}

SGE_API sge_buffer_t* mesh_get_index_buffer(mesh_t* mesh)
{
	return vulkan_mesh_get_index_buffer(VULKAN_MESH_CAST(mesh));
}

SGE_API bool mesh_has_index_buffer(mesh_t* mesh)
{
	return vulkan_mesh_has_index_buffer(VULKAN_MESH_CAST(mesh));
}

SGE_API sge_buffer_t* mesh_get_vertex_buffer_at(mesh_t* mesh, u32 index)
{
	return vulkan_mesh_get_vertex_buffer_at(VULKAN_MESH_CAST(mesh), index);
}

SGE_API sge_buffer_t* mesh_get_vertex_buffer(mesh_t* mesh, u32 binding)
{
	return vulkan_mesh_get_vertex_buffer(VULKAN_MESH_CAST(mesh), binding);
}

SGE_API u32 mesh_get_vertex_buffer_count(mesh_t* mesh)
{
	return vulkan_mesh_get_vertex_buffer_count(VULKAN_MESH_CAST(mesh));
}

SGE_API void mesh_draw(mesh_t* mesh)
{
	vulkan_mesh_draw(mesh);
}

SGE_API void mesh_draw_indexed(mesh_t* mesh)
{
	vulkan_mesh_draw_indexed(mesh);
}

SGE_API void mesh_draw_instanced(mesh_t* mesh, uint32_t instance_count)
{
	vulkan_mesh_draw_instanced(mesh, instance_count);
}
SGE_API void mesh_draw_indexed_instanced(mesh_t* mesh, uint32_t instance_count)
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

