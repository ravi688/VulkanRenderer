
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_vertex_info.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;


typedef struct vulkan_graphics_pipeline_create_info_t
{
	vulkan_shader_t** shaders;
	u32 shader_count;
	uint32_t vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
} vulkan_graphics_pipeline_create_info_t;

typedef struct vulkan_graphics_pipeline_t
{
	VkPipeline pipeline;
	vulkan_pipeline_layout_t* layout;
} vulkan_graphics_pipeline_t;

vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new();
vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info);
void vulkan_graphics_pipeline_create_no_alloc(renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t* pipeline);
void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer);
void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline);
void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, renderer_t* renderer);

