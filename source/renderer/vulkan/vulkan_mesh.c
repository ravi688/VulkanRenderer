#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <memory.h>
#include <renderer/assert.h>

static u32 get_index_stride(VkIndexType index_type);

RENDERER_API vulkan_mesh_t* vulkan_mesh_new(memory_allocator_t* allocator)
{
	vulkan_mesh_t* mesh = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_MESH, vulkan_mesh_t);
	memzero(mesh, vulkan_mesh_t);
	return mesh;
}

RENDERER_API void vulkan_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	_debug_assert__(renderer->logical_device->vo_handle != VK_NULL_HANDLE);
	_debug_assert__(create_info != 0);
	_debug_assert__(mesh != NULL);

	memzero(mesh, vulkan_mesh_t);

	mesh->vertex_buffers = buf_create(sizeof(vulkan_buffer_t), 0, 0);
	mesh->renderer = renderer;
	
	// TODO
	// clear the list of vertex buffers if the same mesh object has been recreated
	//buf_clear(&mesh->vertex_buffers, NULL);
	
	//create vertex buffers
	if((create_info->vertex_buffer_info_count != 0) && (create_info->vertex_buffer_infos != NULL))
	{
		u32 vertex_count = create_info->vertex_buffer_infos[0].count;
		for(u32 i = 0; i < create_info->vertex_buffer_info_count; i++)
		{
			vulkan_vertex_buffer_create_info_t* info = &create_info->vertex_buffer_infos[i];
			if(vertex_count != info->count)
				LOG_FETAL_ERR("Vertex buffer creation failed, all the per-vertex vertex buffers should have the same count\n");
			vulkan_mesh_create_and_add_vertex_buffer(mesh, info);
		}
	}
	else
		LOG_FETAL_ERR("there is no vertex buffers in vulkan_mesh_create_info_t* create_info\n");

	//create index buffer
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
		if(mesh->index_buffer != NULL)
			vulkan_buffer_create_no_alloc(renderer, &buffer_create_info, mesh->index_buffer);
		else
			mesh->index_buffer = vulkan_buffer_create(renderer, &buffer_create_info);
		mesh->index_type = create_info->index_buffer_info.index_type;
	}
	else
	{
		mesh->index_buffer = NULL;
		mesh->index_type = VK_INDEX_TYPE_MAX_ENUM; 	//invalid index type
	}
}


RENDERER_API vulkan_mesh_t* vulkan_mesh_create(vulkan_renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new(renderer->allocator);
	vulkan_mesh_create_no_alloc(renderer, create_info, mesh);
	return mesh;
}

RENDERER_API void vulkan_mesh_destroy(vulkan_mesh_t* mesh)
{
	for(u32 i = 0; i < mesh->vertex_buffers.element_count; i++)
		vulkan_buffer_destroy((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i));
	buf_clear(&mesh->vertex_buffers, NULL);
	if(mesh->index_buffer != NULL)
		vulkan_buffer_destroy(mesh->index_buffer);
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	//NOTE: We are using BUFFER of vulkan_buffer_t itself, which is not allocated by safe_memory/memory_allocator
	//so, no need to call release_resources kind of function because they call heap_free internally, which tries to
	//free as if it was allocated by safe_memory/memory_allocator
	// for(u32 i = 0; i < mesh->vertex_buffers.element_count; i++)
		// vulkan_buffer_release_resources((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i));
	buf_free(&mesh->vertex_buffers);
	// TODO
	// if(mesh->index_buffer != NULL)
	// 	vulkan_buffer_release_resources(mesh->index_buffer);
	// TODO
	// heap_free(mesh);
}

RENDERER_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_indexed_instanced(mesh, 1);
}

RENDERER_API void vulkan_mesh_draw(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_instanced(mesh, 1);
}

RENDERER_API void vulkan_mesh_bind_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer)
{
	_debug_assert__(buffer != NULL);

	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	vkCmdBindVertexBuffers(vo_command_buffer, mesh->binding_index, 1, &buffer->vo_handle, offsets);
	
	// increment the binding index for the next vertex buffer
	mesh->binding_index++;
}

RENDERER_API void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer != NULL);
	_debug_assert__(mesh->index_type != VK_INDEX_TYPE_MAX_ENUM);
	
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	vkCmdBindIndexBuffer(vo_command_buffer, mesh->index_buffer->vo_handle, 0, mesh->index_type);
	vkCmdDrawIndexed(vo_command_buffer, mesh->index_buffer->count, instance_count, 0, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh)
{
	
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	for(u32 i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(vo_command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->vo_handle), offsets);	
}

RENDERER_API void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	_debug_assert__(mesh->index_buffer != NULL);
	_debug_assert__(mesh->index_type != VK_INDEX_TYPE_MAX_ENUM);
	
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	for(u32 i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(vo_command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->vo_handle), offsets);
	vkCmdBindIndexBuffer(vo_command_buffer, mesh->index_buffer->vo_handle, 0, mesh->index_type);
	vkCmdDrawIndexed(vo_command_buffer, mesh->index_buffer->count, instance_count, 0, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
}

RENDERER_API void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count)
{
	
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer vo_command_buffer = mesh->renderer->vo_command_buffers[mesh->renderer->swapchain->current_image_index];
	for(u32 i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(vo_command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->vo_handle), offsets);
	vkCmdDraw(vo_command_buffer, ((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, 0))->count, instance_count, 0, 0);
	
	// set the binding index to zero, for the next frame
	mesh->binding_index = 0;
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

	// allocates memory if current size doesn't fits
	buf_push_pseudo(&mesh->vertex_buffers, 1);
	
	vulkan_buffer_t* buffer = buf_peek_ptr(&mesh->vertex_buffers);
	vulkan_buffer_init(buffer);
	vulkan_buffer_create_no_alloc(mesh->renderer, &buffer_create_info, buffer);
}

static u32 get_index_stride(VkIndexType index_type)
{
	switch(index_type)
	{
		case VK_INDEX_TYPE_UINT16: return sizeof(uint16_t);
		case VK_INDEX_TYPE_UINT32: return sizeof(u32);
		case VK_INDEX_TYPE_UINT8_EXT: return sizeof(uint8_t);
		case VK_INDEX_TYPE_NONE_KHR: LOG_FETAL_ERR("Index buffer creation failed, VK_INDEX_TYPE_NONE_KHR or VK_INDEX_TYPE_NONE_NV isn't supported\n");
		default: LOG_FETAL_ERR("Index buffer creation failed, \"%u\" index type isn't defined\n", index_type);
	}
}
