
#pragma once

#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <buffer.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_mesh_t vulkan_mesh_t;

typedef struct vulkan_vertex_buffer_create_info_t
{
	void* data;
	uint32_t stride;
	uint32_t count;
} vulkan_vertex_buffer_create_info_t;

typedef struct vulkan_index_buffer_create_info_t
{
	void* data;
	VkIndexType index_type;
	uint32_t count;
} vulkan_index_buffer_create_info_t;

typedef struct vulkan_mesh_create_info_t
{
	vulkan_vertex_buffer_create_info_t* vertex_buffer_infos;
	vulkan_vertex_buffer_create_info_t* instance_buffer_infos;
	uint32_t vertex_buffer_info_count;
	uint32_t instance_buffer_info_count;
	vulkan_index_buffer_create_info_t index_buffer_info;
} vulkan_mesh_create_info_t;

typedef struct vulkan_mesh_t
{
	renderer_t* renderer;
	BUFFER /*typeof(vulkan_buffer_t*)*/ vertex_buffers;
	vulkan_buffer_t* index_buffer;
	VkIndexType index_type;

	u32 binding_index;  // INTERNAL 
} vulkan_mesh_t;



// constructors and destructors
vulkan_mesh_t* vulkan_mesh_new();
vulkan_mesh_t* vulkan_mesh_create(renderer_t* renderer, vulkan_mesh_create_info_t* create_info);
void vulkan_mesh_create_no_alloc(renderer_t* renderer, vulkan_mesh_create_info_t* create_info, vulkan_mesh_t* mesh);
void vulkan_mesh_destroy(vulkan_mesh_t* mesh);
void vulkan_mesh_release_resources(vulkan_mesh_t* mesh);


// draw calls
void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh);
void vulkan_mesh_draw(vulkan_mesh_t* mesh);
void vulkan_mesh_draw_indexed_instanced(vulkan_mesh_t* mesh, u32 instance_count);
void vulkan_mesh_draw_indexed_instanced_only(vulkan_mesh_t* mesh, u32 instance_count);
void vulkan_mesh_draw_instanced(vulkan_mesh_t* mesh, u32 instance_count);

void vulkan_mesh_create_and_add_vertex_buffer(vulkan_mesh_t* mesh, vulkan_vertex_buffer_create_info_t* create_info);
void vulkan_mesh_bind_vertex_buffer(vulkan_mesh_t* mesh, vulkan_buffer_t* buffer);
void vulkan_mesh_bind_all_vertex_buffers(vulkan_mesh_t* mesh);
