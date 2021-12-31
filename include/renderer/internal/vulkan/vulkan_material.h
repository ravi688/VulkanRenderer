
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_vertex_info.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_graphics_pipeline_t vulkan_graphics_pipeline_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef struct vulkan_material_create_info_t
{
	vulkan_shader_t** shaders;
	u64 shader_count;
	uint32_t vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
	VkDescriptorSetLayoutBinding* bindings;
	uint32_t binding_count;
} vulkan_material_create_info_t;

typedef struct vulkan_material_t
{ 
	//For event arguments
	renderer_t* renderer;
	void* self_reference; 	//self_reference

	//For recreating the graphics pipeline on render window resize
	vulkan_material_create_info_t create_info;

	//For now we will be using only one descriptor set and descriptor layout
	vulkan_descriptor_set_t* descriptor_set;
	vulkan_graphics_pipeline_t* graphics_pipeline;
} vulkan_material_t;

vulkan_material_t* vulkan_material_new();
vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info);
void vulkan_material_create_no_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material);
void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer);
void vulkan_material_release_resources(vulkan_material_t* material);
void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer);
void vulkan_material_push_constants(vulkan_material_t* material, renderer_t* renderer, void* bytes);
void vulkan_material_set_texture(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture);
void vulkan_material_set_uniform_buffer(vulkan_material_t* material, renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer);
