
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_swapchain.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>
#include <renderer/assert.h>


vulkan_mesh_t* vulkan_mesh_new()
{
	vulkan_mesh_t* mesh = heap_new(vulkan_mesh_t);
	memset(mesh, 0, sizeof(vulkan_mesh_t));
	return mesh;
}

void vulkan_mesh_create_no_alloc(renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	assert(create_info->vertex_count != 0);
	assert(create_info->p_position_data != 0);

	mesh->vertex_count = create_info->vertex_count;

	//create position buffer object
	u32 position_buffer_size = create_info->position_stride * create_info->vertex_count;
	mesh->position_buffer = vk_get_buffer(renderer->vk_device, position_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
	mesh->position_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->position_buffer, position_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(renderer->vk_device, mesh->position_buffer_memory, 0, position_buffer_size, 0, &data);
	memcpy(data, create_info->p_position_data, position_buffer_size);
	vkUnmapMemory(renderer->vk_device, mesh->position_buffer_memory);
	mesh->position_stride = create_info->position_stride;

	if(create_info->p_color_data != NULL)
	{
		//create color buffer object
		u32 color_buffer_size = create_info->color_stride * create_info->vertex_count;
		mesh->color_buffer = vk_get_buffer(renderer->vk_device, color_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
		mesh->color_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->color_buffer, color_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(renderer->vk_device, mesh->color_buffer_memory, 0, color_buffer_size, 0, &data);
		memcpy(data, create_info->p_color_data, color_buffer_size);
		vkUnmapMemory(renderer->vk_device, mesh->color_buffer_memory);
		mesh->color_stride = create_info->color_stride;
	}

	if(create_info->p_normal_data != NULL)
	{
		//create normal buffer object
		u32 normal_buffer_size = create_info->normal_stride * create_info->vertex_count;
		mesh->normal_buffer = vk_get_buffer(renderer->vk_device, normal_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
		mesh->normal_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->normal_buffer, normal_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(renderer->vk_device, mesh->normal_buffer_memory, 0, normal_buffer_size, 0, &data);
		memcpy(data, create_info->p_normal_data, normal_buffer_size);
		vkUnmapMemory(renderer->vk_device, mesh->normal_buffer_memory);
		mesh->normal_stride = create_info->normal_stride;
	}

	if(create_info->p_uv_data != NULL)
	{
		//create uv buffer object
		u32 uv_buffer_size = create_info->uv_stride * create_info->vertex_count;
		mesh->uv_buffer = vk_get_buffer(renderer->vk_device, uv_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
		mesh->uv_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->uv_buffer, uv_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(renderer->vk_device, mesh->uv_buffer_memory, 0, uv_buffer_size, 0, &data);
		memcpy(data, create_info->p_uv_data, uv_buffer_size);
		vkUnmapMemory(renderer->vk_device, mesh->uv_buffer_memory);
		mesh->uv_stride = create_info->uv_stride;
	}

	if(create_info->p_index_data != NULL)
	{
		//create index buffer object
		u32 index_buffer_size = create_info->index_stride * create_info->index_count;
		mesh->index_buffer = vk_get_buffer(renderer->vk_device, index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
		mesh->index_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->index_buffer, index_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(renderer->vk_device, mesh->index_buffer_memory, 0, index_buffer_size, 0, &data);
		memcpy(data, create_info->p_index_data, index_buffer_size);
		vkUnmapMemory(renderer->vk_device, mesh->index_buffer_memory);
		mesh->index_count = create_info->index_count;
		mesh->index_stride = create_info->index_stride;
	}
}


vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new();
	vulkan_mesh_create_no_alloc(renderer, create_info, mesh);
	return mesh;
}

void vulkan_mesh_destroy(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	vkDestroyBuffer(renderer->vk_device, mesh->position_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, mesh->color_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, mesh->normal_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, mesh->uv_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, mesh->index_buffer, NULL);
	vkFreeMemory(renderer->vk_device, mesh->position_buffer_memory, NULL);
	vkFreeMemory(renderer->vk_device, mesh->color_buffer_memory, NULL);
	vkFreeMemory(renderer->vk_device, mesh->normal_buffer_memory, NULL);
	vkFreeMemory(renderer->vk_device, mesh->uv_buffer_memory, NULL);
	vkFreeMemory(renderer->vk_device, mesh->index_buffer_memory, NULL);
}

void vulkan_mesh_sync(vulkan_mesh_t* mesh, renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	LOG_FETAL_ERR("Method not defined\n");
}

void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	heap_free(mesh);
}

void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	ASSERT(mesh->index_buffer != VK_NULL_HANDLE, "vulkan_mesh_t doesn't have indices but you are calling vulkan_mesh_draw_index\n");
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 0, 1, &(mesh->position_buffer), offsets);
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 1, 1, &(mesh->color_buffer), offsets);
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 2, 1, &(mesh->normal_buffer), offsets);
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 3, 1, &(mesh->uv_buffer), offsets);
	vkCmdBindIndexBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->index_buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->index_count, 1, 0, 0, 0);
}

void vulkan_mesh_draw(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	VkBuffer vertex_buffers[1] = { mesh->position_buffer };
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 0, 1, vertex_buffers, offsets);
	vkCmdDraw(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->vertex_count, 1, 0, 0);
}
