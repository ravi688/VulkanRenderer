
#pragma once

#include <vulkan/vulkan_wrapper.h>

typedef struct renderer_t renderer_t;

typedef struct vulkan_pipeline_layout_t
{
	VkPipelineLayout pipeline_layout;
	VkDescriptorSetLayout descriptor_set_layout;
} vulkan_pipeline_layout_t;


vulkan_pipeline_layout_t* vulkan_pipeline_layout_new();
vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer);
void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_t* pipeline_layout);
void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer);
void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* pipeline_layout);

void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer, void* bytes);
