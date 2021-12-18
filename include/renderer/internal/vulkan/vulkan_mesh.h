
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <stdint.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef struct vulkan_mesh_create_info_t
{
	void* p_position_data;
	void* p_color_data;
	void* p_normal_data;
	void* p_uv_data;
	void* p_index_data;

	uint32_t position_stride;
	uint32_t color_stride;
	uint32_t normal_stride;
	uint32_t uv_stride;
	uint32_t index_stride;

	uint32_t vertex_count;
	uint32_t index_count;
} vulkan_mesh_create_info_t;


typedef struct vulkan_mesh_t
{
	VkBuffer position_buffer;
	VkBuffer color_buffer;
	VkBuffer normal_buffer;
	VkBuffer uv_buffer;
	VkBuffer index_buffer;

	VkDeviceMemory position_buffer_memory;
	VkDeviceMemory color_buffer_memory;
	VkDeviceMemory normal_buffer_memory;
	VkDeviceMemory uv_buffer_memory;
	VkDeviceMemory index_buffer_memory;

	uint32_t position_stride;
	uint32_t color_stride;
	uint32_t normal_stride;
	uint32_t uv_stride;
	uint32_t index_stride;

	uint32_t vertex_count;
	uint32_t index_count;
} vulkan_mesh_t;


vulkan_mesh_t* vulkan_mesh_new();
vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* create_info);
void vulkan_mesh_create_no_alloc(renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh);
void vulkan_mesh_destroy(vulkan_mesh_t* mesh, renderer_t* renderer);
void vulkan_mesh_sync(vulkan_mesh_t* mesh, renderer_t* renderer, vulkan_mesh_create_info_t* create_info);
void vulkan_mesh_release_resources(vulkan_mesh_t* mesh);
void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh, renderer_t* renderer);
void vulkan_mesh_draw(vulkan_mesh_t* mesh, renderer_t* renderer);
void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, renderer_t* renderer, uint32_t instance_count);
void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, renderer_t* renderer, uint32_t instance_count);
