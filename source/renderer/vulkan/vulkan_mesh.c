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

#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <memory.h>
#include <renderer/assert.h>

#include <stdio.h> // TODO: remove it

static u32 get_index_stride(VkIndexType index_type);

RENDERER_API vulkan_mesh_t* vulkan_mesh_new(memory_allocator_t* allocator)
{
	vulkan_mesh_t* mesh = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_MESH, vulkan_mesh_t);
	memzero(mesh, vulkan_mesh_t);
	VULKAN_OBJECT_INIT(mesh, VULKAN_OBJECT_TYPE_MESH, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return mesh;
}

RENDERER_API vulkan_mesh_t* vulkan_mesh_create(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new(renderer->allocator);
	vulkan_mesh_create_no_alloc(renderer, create_info, mesh);
	return mesh;
}

RENDERER_API void vulkan_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	VULKAN_OBJECT_MEMZERO(mesh, vulkan_mesh_t);
	mesh->renderer = renderer;

	mesh->vertex_buffers = buf_new(vulkan_vertex_buffer_t);

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
	{
		vulkan_buffer_create_info_t buffer_create_info =
		{
			.data = create_info->index_buffer_info.data,
			.stride = get_index_stride(create_info->index_buffer_info.index_type),
			.count = create_info->index_buffer_info.count,
			.vo_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		mesh->index_buffer.buffer = vulkan_buffer_create(renderer, &buffer_create_info);
		mesh->index_buffer.index_type = create_info->index_buffer_info.index_type;
		mesh->index_buffer.is_internal = true;
	}
	else
	{
		mesh->index_buffer.buffer = NULL;
		mesh->index_buffer.index_type = VK_INDEX_TYPE_MAX_ENUM; 	//invalid index type
	}
}

RENDERER_API void vulkan_mesh_destroy(vulkan_mesh_t* mesh)
{
	/* destroy the vertex buffers */
	u32 vertex_buffer_count = buf_get_element_count(&mesh->vertex_buffers);
	for(u32 i = 0; i < vertex_buffer_count; i++)
	{
		AUTO vertex_buffer = CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, i));
		/* only destroy and release resources if the vulkan_buffer_t has been allocated internally (i.e. by vulkan_mesh_t) */
		if(vertex_buffer->is_internal)
		{
			vulkan_buffer_destroy(vertex_buffer->buffer);
			vulkan_buffer_release_resources(vertex_buffer->buffer);
		}
	}
	buf_clear(&mesh->vertex_buffers, NULL);

	/* destroy the index buffer if it non null and created internally */
	if((mesh->index_buffer.buffer != NULL) && mesh->index_buffer.is_internal)
		vulkan_buffer_destroy(mesh->index_buffer.buffer);
	
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	buf_free(&mesh->vertex_buffers);
	if(mesh->index_buffer.buffer != NULL)
		vulkan_buffer_release_resources(mesh->index_buffer.buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(mesh))
		memory_allocator_dealloc(mesh->renderer->allocator, mesh);
}

RENDERER_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_indexed_instanced(mesh, 1);
}

RENDERER_API void vulkan_mesh_draw(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_instanced(mesh, 1);
}

RENDERER_API void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer.buffer != NULL);
	_debug_assert__(mesh->index_buffer.index_type != VK_INDEX_TYPE_MAX_ENUM);
	
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	vkCmdBindIndexBuffer(vo_command_buffer, mesh->index_buffer.buffer->vo_handle, 0, mesh->index_buffer.index_type);
	vkCmdDrawIndexed(vo_command_buffer, mesh->index_buffer.buffer->count, instance_count, 0, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh)
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
		VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
		vulkan_command_bind_vertex_buffers(vo_command_buffer, bindings, buffers, count);
	}
}

RENDERER_API void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer.buffer != NULL);
	_debug_assert__(mesh->index_buffer.index_type != VK_INDEX_TYPE_MAX_ENUM);

	vulkan_mesh_bind_all_vertex_buffers(mesh);
	vulkan_mesh_draw_indexed_instanced_only(mesh, instance_count);
}

RENDERER_API void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	vulkan_mesh_bind_all_vertex_buffers(mesh);
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	vkCmdDraw(vo_command_buffer, CAST_TO(vulkan_vertex_buffer_t*, buf_get_ptr_at(&mesh->vertex_buffers, 0))->buffer->count, instance_count, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_set_material(vulkan_mesh_t* mesh, vulkan_material_t* material)
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

RENDERER_API void vulkan_mesh_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, u32 binding)
{
	_vulkan_mesh_add_vertex_buffer(mesh, buffer, binding, false);
}

RENDERER_API void vulkan_mesh_set_index_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer, VkIndexType vo_type)
{
	mesh->index_buffer.buffer = buffer;
	mesh->index_buffer.index_type = vo_type;
	mesh->index_buffer.is_internal = false;
}

RENDERER_API void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info)
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
