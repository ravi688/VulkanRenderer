
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

void vulkan_mesh_create_non_alloc(renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	//create vertex buffer object
	u32 vertex_buffer_size = create_info->vertex_stride * create_info->vertex_count;
	mesh->vertex_buffer = vk_get_buffer(renderer->vk_device, vertex_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
	mesh->vertex_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->vertex_buffer, vertex_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkCall(vkBindBufferMemory(renderer->vk_device, mesh->vertex_buffer, mesh->vertex_buffer_memory, 0));
	void* data;
	vkMapMemory(renderer->vk_device, mesh->vertex_buffer_memory, 0, vertex_buffer_size, 0, &data);
	memcpy(data, create_info->p_vertex_data, vertex_buffer_size);
	vkUnmapMemory(renderer->vk_device, mesh->vertex_buffer_memory);
	mesh->vertex_count = create_info->vertex_count;
	mesh->vertex_stride = create_info->vertex_stride;

	if(create_info->p_index_data != NULL)
	{
		//create index buffer object
		u32 index_buffer_size = create_info->index_stride * create_info->index_count;
		mesh->index_buffer = vk_get_buffer(renderer->vk_device, index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
		mesh->index_buffer_memory = vk_get_device_memory_for_buffer(renderer->vk_device, renderer->vk_physical_device, mesh->index_buffer, index_buffer_size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkCall(vkBindBufferMemory(renderer->vk_device, mesh->index_buffer, mesh->index_buffer_memory, 0));
		vkMapMemory(renderer->vk_device, mesh->index_buffer_memory, 0, index_buffer_size, 0, &data);
		memcpy(data, create_info->p_index_data, index_buffer_size);
		vkUnmapMemory(renderer->vk_device, mesh->index_buffer_memory);
		mesh->index_count = create_info->index_count;
		mesh->index_stride = create_info->index_stride;
	}
	else
	{
		mesh->index_buffer = VK_NULL_HANDLE;
		mesh->index_buffer_memory = VK_NULL_HANDLE;
	}
}


vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* create_info)
{
	vulkan_mesh_t* mesh = vulkan_mesh_new();
	vulkan_mesh_create_non_alloc(renderer, create_info, mesh);
	return mesh;
}

void vulkan_mesh_destroy(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	vkDestroyBuffer(renderer->vk_device, mesh->vertex_buffer, NULL);
	vkDestroyBuffer(renderer->vk_device, mesh->index_buffer, NULL);
	vkFreeMemory(renderer->vk_device, mesh->vertex_buffer_memory, NULL);
	vkFreeMemory(renderer->vk_device, mesh->index_buffer_memory, NULL);
}

void vulkan_mesh_release_resources(vulkan_mesh_t* mesh)
{
	heap_free(mesh);
}

void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	if(mesh->index_buffer == VK_NULL_HANDLE)
	{
		log_fetal_err("vulkan_mesh_t doesn't have indices but you are calling vulkan_mesh_draw_index\n");
	}
	VkBuffer vertex_buffers[1] =  { mesh->vertex_buffer };
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->index_buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->index_stride * mesh->index_count, 1, 0, 0, 0);
}

void vulkan_mesh_draw(vulkan_mesh_t* mesh, renderer_t* renderer)
{
	VkBuffer vertex_buffers[1] = { mesh->vertex_buffer };
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], 0, 1, vertex_buffers, offsets);
	vkCmdDraw(renderer->vk_command_buffers.value2[renderer->swapchain->current_image_index], mesh->vertex_count, 1, 0, 0);
}
