
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_types.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;


typedef struct vulkan_graphics_pipeline_create_info_t
{
	vulkan_shader_t* shader;
	uint32_t vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
	uint32_t push_constant_range_count;
	VkPushConstantRange* push_constant_ranges;
	VkDescriptorSetLayout vk_set_layout;
} vulkan_graphics_pipeline_create_info_t;

typedef struct vulkan_graphics_pipeline_t
{
	VkPipeline pipeline;
	vulkan_pipeline_layout_t* pipeline_layout;
} vulkan_graphics_pipeline_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new();
RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info);
RENDERER_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t* pipeline);
RENDERER_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline, vulkan_renderer_t* renderer);
RENDERER_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline);
RENDERER_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline, vulkan_renderer_t* renderer);


END_CPP_COMPATIBLE
