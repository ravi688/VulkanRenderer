
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_mesh_t* mesh);
#else
#	define check_pre_condition(mesh)
#endif

static uint32_t get_index_stride(VkIndexType index_type);

vulkan_mesh_t* vulkan_mesh_new()
{
	vulkan_mesh_t* mesh = heap_new(vulkan_mesh_t);
	memset(mesh, 0, sizeof(vulkan_mesh_t));
	mesh->vertex_buffers = buf_create(sizeof(vulkan_buffer_t), 0, 0);
	return mesh;
}

void vulkan_mesh_create_no_alloc(renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	assert(create_info != 0);
	assert(mesh != NULL);
	mesh->renderer = renderer;
	buf_clear(&mesh->vertex_buffers, NULL);
	//create vertex buffers
	if((create_info->vertex_buffer_info_count != 0) && (create_info->vertex_buffer_infos != NULL))
	{
		uint32_t vertex_count = refp(vulkan_vertex_buffer_create_info_t, create_info->vertex_buffer_infos, 0)->count;
		for(uint32_t i = 0; i < create_info->vertex_buffer_info_count; i++)
		{
			vulkan_vertex_buffer_create_info_t* info = refp(vulkan_vertex_buffer_create_info_t, create_info->vertex_buffer_infos, i);
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
			.usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			.sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
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


vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new();
	vulkan_mesh_create_no_alloc(renderer, create_info, mesh);
	return mesh;
}

void vulkan_mesh_destroy(vulkan_mesh_t* mesh)
{
	check_pre_condition(mesh);
	for(uint32_t i = 0; i < mesh->vertex_buffers.element_count; i++)
		vulkan_buffer_destroy((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i));
	buf_clear(&mesh->vertex_buffers, NULL);
	if(mesh->index_buffer != NULL)
		vulkan_buffer_destroy(refp(vulkan_buffer_t, mesh->index_buffer, 0));
	mesh->binding_index = 0;
}

void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	check_pre_condition(mesh);
	//NOTE: We are using BUFFER of vulkan_buffer_t itself, which is not allocated by safe_memory/memory_allocator
	//so, no need to call release_resources kind of function because they call heap_free internally, which tries to
	//free as if it was allocated by safe_memory/memory_allocator
	// for(uint32_t i = 0; i < mesh->vertex_buffers.element_count; i++)
		// vulkan_buffer_release_resources((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i));
	buf_free(&mesh->vertex_buffers);
	if(mesh->index_buffer != NULL)
		vulkan_buffer_release_resources(refp(vulkan_buffer_t, mesh->index_buffer, 0));
	heap_free(mesh);
}

void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_indexed_instanced(mesh, 1);
}

void vulkan_mesh_draw(vulkan_mesh_t* mesh)
{
	vulkan_mesh_draw_instanced(mesh, 1);
}

void vulkan_mesh_bind_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer)
{
	check_pre_condition(mesh);
	assert(buffer != NULL);
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer command_buffer = mesh->renderer->vk_command_buffers.value2[mesh->renderer->swapchain->current_image_index];
	vkCmdBindVertexBuffers(command_buffer, mesh->binding_index, 1, &buffer->handle, offsets);
	mesh->binding_index++;
}

void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, uint32_t instance_count)
{
	check_pre_condition(mesh);
	assert(mesh->index_buffer != NULL);
	assert(mesh->index_type != VK_INDEX_TYPE_MAX_ENUM);
	VkCommandBuffer command_buffer = mesh->renderer->vk_command_buffers.value2[mesh->renderer->swapchain->current_image_index];
	vkCmdBindIndexBuffer(command_buffer, mesh->index_buffer->handle, 0, mesh->index_type);
	vkCmdDrawIndexed(command_buffer, mesh->index_buffer->count, instance_count, 0, 0, 0);
	mesh->binding_index = 0;
}

void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh)
{
	check_pre_condition(mesh);
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer command_buffer = mesh->renderer->vk_command_buffers.value2[mesh->renderer->swapchain->current_image_index];
	for(uint32_t i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->handle), offsets);	
}

void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, uint32_t instance_count)
{
	check_pre_condition(mesh);
	assert(mesh->index_buffer != NULL);
	assert(mesh->index_type != VK_INDEX_TYPE_MAX_ENUM);
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer command_buffer = mesh->renderer->vk_command_buffers.value2[mesh->renderer->swapchain->current_image_index];
	for(uint32_t i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->handle), offsets);
	vkCmdBindIndexBuffer(command_buffer, mesh->index_buffer->handle, 0, mesh->index_type);
	vkCmdDrawIndexed(command_buffer, mesh->index_buffer->count, instance_count, 0, 0, 0);
	mesh->binding_index = 0;
}

void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, uint32_t instance_count)
{
	check_pre_condition(mesh);
	VkDeviceSize offsets[1] = { 0 };
	VkCommandBuffer command_buffer = mesh->renderer->vk_command_buffers.value2[mesh->renderer->swapchain->current_image_index];
	for(uint32_t i = 0; i < mesh->vertex_buffers.element_count; i++, mesh->binding_index++)
		vkCmdBindVertexBuffers(command_buffer, mesh->binding_index, 1, &(((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, i))->handle), offsets);
	vkCmdDraw(command_buffer, ((vulkan_buffer_t*)buf_get_ptr_at(&mesh->vertex_buffers, 0))->count, instance_count, 0, 0);
	mesh->binding_index = 0;
}

void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info)
{
	check_pre_condition(mesh);
	assert(create_info != NULL);
	assert(create_info->count != 0);
	assert(create_info->data != NULL);
	vulkan_buffer_create_info_t buffer_create_info =
	{
		.data = create_info->data,
		.stride = create_info->stride,
		.count = create_info->count,
		.usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	buf_push_pseudo(&mesh->vertex_buffers, 1);
	vulkan_buffer_t* buffer = buf_peek_ptr(&mesh->vertex_buffers);
	vulkan_buffer_init(buffer);
	vulkan_buffer_create_no_alloc(mesh->renderer, &buffer_create_info, buffer);
}

static uint32_t get_index_stride(VkIndexType index_type)
{
	switch(index_type)
	{
		case VK_INDEX_TYPE_UINT16: return sizeof(uint16_t);
		case VK_INDEX_TYPE_UINT32: return sizeof(uint32_t);
		case VK_INDEX_TYPE_UINT8_EXT: return sizeof(uint8_t);
		case VK_INDEX_TYPE_NONE_KHR: LOG_FETAL_ERR("Index buffer creation failed, VK_INDEX_TYPE_NONE_KHR or VK_INDEX_TYPE_NONE_NV isn't supported\n");
		default: LOG_FETAL_ERR("Index buffer creation failed, \"%u\" index type isn't defined\n", index_type);
	}
}


#ifdef GLOBAL_DEBUG
static void check_pre_condition(vulkan_mesh_t* mesh)
{
	assert(mesh != NULL);
	assert(mesh->renderer != NULL);
}
#endif /* GLOBAL_DEBUG */
