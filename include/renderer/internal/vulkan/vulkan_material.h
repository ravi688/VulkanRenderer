
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_vertex_info.h>

typedef struct vulkan_graphics_pipeline_t vulkan_graphics_pipeline_t;
typedef struct vulkan_shader_t vulkan_shader_t;

typedef struct vulkan_material_create_info_t
{
	vulkan_shader_t** shaders;
	u64 shader_count;
	vulkan_vertex_info_t vertex_info;
} vulkan_material_create_info_t;

typedef struct vulkan_material_t
{
	vulkan_graphics_pipeline_t* pipeline;
} vulkan_material_t;


vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info);
void vulkan_material_destroy(vulkan_material_t* material, renderer_t* renderer);
void vulkan_material_bind(vulkan_material_t* material, renderer_t* renderer);
