
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <stdint.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef struct vulkan_mesh_create_info_t
{
	void* p_vertex_data;
	void* p_index_data;
	uint32_t vertex_stride;
	uint32_t index_stride;
	uint32_t vertex_count;
	uint32_t index_count;
} vulkan_mesh_create_info_t;


typedef struct vulkan_mesh_t
{
	VkBuffer vertex_buffer;
	VkBuffer index_buffer;
	VkDeviceMemory vertex_buffer_memory;
	VkDeviceMemory index_buffer_memory;
	uint32_t vertex_stride;
	uint32_t index_stride;
	uint32_t vertex_count;
	uint32_t index_count;
} vulkan_mesh_t;


vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* createInfo);
void vulkan_mesh_destroy(vulkan_mesh_t* mesh, renderer_t* renderer);
void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh, renderer_t* renderer);
void vulkan_mesh_draw(vulkan_mesh_t* mesh, renderer_t* renderer);
