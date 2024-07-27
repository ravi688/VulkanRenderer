/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_mesh.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_mesh.h>
#include <sge/internal/vulkan/vulkan_swapchain.h>
#include <sge/internal/vulkan/vulkan_buffer.h>
#include <sge/internal/vulkan/vulkan_command.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <memory.h>
#include <sge/assert.h>

#include <stdio.h> // TODO: remove it

static u32 get_index_stride(VkIndexType index_type);

SGE_API vulkan_mesh_t* vulkan_mesh_new(memory_allocator_t* allocator)
{
	vulkan_mesh_t* mesh = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_MESH, vulkan_mesh_t);
	memzero(mesh, vulkan_mesh_t);
	VULKAN_OBJECT_INIT(mesh, VULKAN_OBJECT_TYPE_MESH, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return mesh;
}

SGE_API vulkan_mesh_t* vulkan_mesh_create(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new(renderer->allocator);
	vulkan_mesh_create_no_alloc(renderer, create_info, mesh);
	return mesh;
}

SGE_API void vulkan_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	VULKAN_OBJECT_MEMZERO(mesh, vulkan_mesh_t);
	mesh->renderer = renderer;

	mesh->vertex_buffers = memory_allocator_buf_new(renderer->allocator, vulkan_vertex_buffer_t);

	/* create vertex buffers */
	if((create_info->vertex_buffer_info_count != 0) && (create_info->vertex_buffer_infos != NULL))
	{
		u32 vertex_count = create_info->vertex_buffer_infos[0].count;
		for(u32 i = 0; i < create_info->vertex_buffer_info_count; i++)
		{
			vulkan_vertex_buffer_create_info_t* info = &create_info->vertex_buffer_infos[i];
			if(vertex_count != info->count)
				DEBUG_LOG_FETAL_ERROR("Vertex buffer creation failed, all the per-vertex vertex buffers should have the same count");
			vulkan_mesh_create_and_add_vertex_buffer(mesh, info);
		}
	}

	/* create index buffer */
	if(create_info->index_buffer_info.count != 0)
		vulkan_mesh_create_and_set_index_buffer(mesh, &create_info->index_buffer_info);
	else
	{
		mesh->index_buffer.buffer = NULL;
		mesh->index_buffer.index_type = VK_INDEX_TYPE_MAX_ENUM; 	//invalid index type
	}
}

SGE_API void vulkan_mesh_destroy(vulkan_mesh_t* mesh)
{
	/* destroy the vertex buffers */
	vulkan_mesh_destroy_vertex_buffers(mesh);

	/* destroy the index buffer if it non null and created internally */
	vulkan_mesh_destroy_index_buffer(mesh);
	
	mesh->binding_index = 0;
}

SGE_API void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	buf_free(&mesh->vertex_buffers);
	if(VULKAN_OBJECT_IS_INTERNAL(mesh))
		memory_allocator_dealloc(mesh->renderer->allocator, mesh);
}

SGE_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_indexed_instanced(mesh, 1);
}

SGE_API void vulkan_mesh_draw(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_instanced(mesh, 1);
}

SGE_API void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer.buffer != NULL);
	_debug_assert__(mesh->index_buffer.index_type != VK_INDEX_TYPE_MAX_ENUM);
	
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->current_frame_index];
	vkCmdBindIndexBuffer(vo_command_buffer, mesh->index_buffer.buffer->vo_handle, 0, mesh->index_buffer.index_type);
	vkCmdDrawIndexed(vo_command_buffer, mesh->index_buffer.buffer->count, instance_count, 0, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

SGE_API void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh)
{	
	u32 count = buf_get_element_count(&mesh->vertex_buffers);
	VkBuffer buffers[count];
	u32 bindings[count];
	for(u32 i = 0; i < count; i++)
	{
		AUTO vertex_buffer = CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, i));
		buffers[i] = vertex_buffer->buffer->vo_handle;
		bindings[i] = vertex_buffer->binding;
	}

	if(count > 0)
	{
		VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->current_frame_index];
		vulkan_command_bind_vertex_buffers(vo_command_buffer, bindings, buffers, count);
	}
}

SGE_API void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer.buffer != NULL);
	_debug_assert__(mesh->index_buffer.index_type != VK_INDEX_TYPE_MAX_ENUM);

	vulkan_mesh_bind_all_vertex_buffers(mesh);
	vulkan_mesh_draw_indexed_instanced_only(mesh, instance_count);
}

SGE_API void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	vulkan_mesh_bind_all_vertex_buffers(mesh);
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->current_frame_index];
	vkCmdDraw(vo_command_buffer, CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, 0))->buffer->count, instance_count, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

SGE_API void vulkan_mesh_set_material(vulkan_mesh_t* mesh, vulkan_material_t* material)
{
	mesh->material = material;
}

static bool binding_less_than(void* lhs, void* rhs, void* user_data)
{
	return CAST_TO(vulkan_vertex_buffer_t*, lhs)->binding < CAST_TO(vulkan_vertex_buffer_t*, rhs)->binding;
}

UNUSED_FUNCTION static void print_binding(void* ptr, void* user_data)
{
	printf("%u ", CAST_TO(vulkan_vertex_buffer_t*, ptr)->binding);
}

static void _vulkan_mesh_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, u32 binding, bool is_internal)
{
	_debug_assert__(buffer != NULL);
	vulkan_vertex_buffer_t _buffer = { .buffer = buffer, .binding = binding, .is_internal = is_internal };
	/* push and sort the vertex buffers in increasing order of binding  */
	buf_push_sort(&mesh->vertex_buffers, &_buffer, binding_less_than, NULL);
	// printf("Bindings: ");
	// buf_traverse_elements(&mesh->vertex_buffers, 0, buf_get_element_count(&mesh->vertex_buffers) - 1, print_binding, NULL);
	// puts("");
}

SGE_API void vulkan_mesh_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, u32 binding)
{
	_vulkan_mesh_add_vertex_buffer(mesh, buffer, binding, false);
}

SGE_API void vulkan_mesh_destroy_vertex_buffers(vulkan_mesh_t* mesh)
{
	u32 count = buf_get_element_count(&mesh->vertex_buffers);
	for(u32 i = 0; i < count; i++)
	{
		AUTO vbuffer = CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, i));
		/* only destroy the vulkan buffers which were created internally */
		if(vbuffer->is_internal)
		{
			_debug_assert__(vbuffer->buffer != NULL);
			vulkan_buffer_destroy(vbuffer->buffer);
			vulkan_buffer_release_resources(vbuffer->buffer);
		}
	}
	buf_clear(&mesh->vertex_buffers, NULL);
}

typedef vulkan_vertex_buffer_t* vulkan_vertex_buffer_ptr_t;
typedef_pair_t(vulkan_vertex_buffer_ptr_t, u32);

static pair_t(vulkan_vertex_buffer_ptr_t, u32) find_vertex_buffer(vulkan_mesh_t* mesh, u32 binding)
{
	u32 count = buf_get_element_count(&mesh->vertex_buffers);
	for(u32 i = 0; i < count; i++)
	{
		AUTO vbuffer = CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, i));
		if(vbuffer->binding == binding)
			return make_pair_t(vulkan_vertex_buffer_ptr_t, u32) { vbuffer, i };
	}
	return make_pair_t(vulkan_vertex_buffer_ptr_t, u32) { NULL, U32_MAX };
}

SGE_API void vulkan_mesh_destroy_vertex_buffer(vulkan_mesh_t* mesh, u32 binding)
{
	/* find the vertex buffer with the supplied binding */
	AUTO result = find_vertex_buffer(mesh, binding);
	_debug_assert__(result.first != NULL);

	// destroy it only if it is internally created
	if(result.first->is_internal)
	{
		vulkan_buffer_destroy(result.first->buffer);
		vulkan_buffer_release_resources(result.first->buffer);
	}

	// remove the reference from the list of vertex buffers of this mesh
	bool _result = buf_remove_at(&mesh->vertex_buffers, result.second, NULL);
	_debug_assert__(_result == true);
}

SGE_API vulkan_buffer_t* vulkan_mesh_get_vertex_buffer_at(vulkan_mesh_t* mesh, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&mesh->vertex_buffers));
	return CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, index))->buffer;
}

SGE_API vulkan_buffer_t* vulkan_mesh_get_vertex_buffer(vulkan_mesh_t* mesh, u32 binding)
{
	/* find the vertex buffer with the supplied binding */
	AUTO result = find_vertex_buffer(mesh, binding);
	if(result.first == NULL)
		return NULL;
	_debug_assert__(result.first->buffer != NULL);
	return result.first->buffer;
}

SGE_API u32 vulkan_mesh_get_vertex_buffer_count(vulkan_mesh_t* mesh)
{
	return buf_get_element_count(&mesh->vertex_buffers);
}

SGE_API void vulkan_mesh_set_index_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, VkIndexType vo_type)
{
	/* if index buffer has already been created internally, then destroy that */
	vulkan_mesh_destroy_index_buffer(mesh);

	mesh->index_buffer.buffer = buffer;
	mesh->index_buffer.index_type = vo_type;
	mesh->index_buffer.is_internal = false;
}

SGE_API void vulkan_mesh_destroy_index_buffer(vulkan_mesh_t* mesh)
{
	/* destroy the index buffer only if it is internal */
	if(mesh->index_buffer.is_internal)
	{
		_debug_assert__(mesh->index_buffer.buffer != NULL);
		vulkan_buffer_destroy(mesh->index_buffer.buffer);
		vulkan_buffer_release_resources(mesh->index_buffer.buffer);
	}

	/* remove the reference */
	mesh->index_buffer.buffer = NULL;
	mesh->index_buffer.index_type = VK_INDEX_TYPE_MAX_ENUM;
	mesh->index_buffer.is_internal = false;
}

SGE_API void vulkan_mesh_create_and_set_index_buffer(vulkan_mesh_t* mesh, vulkan_index_buffer_create_info_t* create_info)
{
	/* now create new index buffer */
	vulkan_buffer_create_info_t buffer_create_info =
	{
		.data = create_info->data,
		.stride = get_index_stride(create_info->index_type),
		.count = create_info->count,
		.vo_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_mesh_set_index_buffer(mesh, vulkan_buffer_create(mesh->renderer, &buffer_create_info), create_info->index_type);
}

SGE_API void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info)
{
	_debug_assert__(create_info != NULL);
	_debug_assert__(create_info->count != 0);
	_debug_assert__(create_info->data != NULL);
	
	vulkan_buffer_create_info_t buffer_create_info =
	{
		.data = create_info->data,
		.stride = create_info->stride,
		.count = create_info->count,
		.vo_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};

	_vulkan_mesh_add_vertex_buffer(mesh, vulkan_buffer_create(mesh->renderer, &buffer_create_info), create_info->binding, true);
}

static u32 get_index_stride(VkIndexType index_type)
{
	switch(index_type)
	{
		case VK_INDEX_TYPE_UINT16: return sizeof(u16);
		case VK_INDEX_TYPE_UINT32: return sizeof(u32);
		case VK_INDEX_TYPE_UINT8_EXT: return sizeof(u8);
		case VK_INDEX_TYPE_NONE_KHR: DEBUG_LOG_FETAL_ERROR("Index buffer creation failed, VK_INDEX_TYPE_NONE_KHR or VK_INDEX_TYPE_NONE_NV isn't supported");
		default: DEBUG_LOG_FETAL_ERROR("Index buffer creation failed, \"%u\" index type isn't defined", index_type); UNREACHABLE();
	}
}
