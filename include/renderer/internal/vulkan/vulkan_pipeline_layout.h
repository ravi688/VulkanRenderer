
#pragma once

#include <vulkan/vulkan_wrapper.h>

typedef struct renderer_t renderer_t;

typedef struct vulkan_pipeline_layout_t
{
	VkPipelineLayout layout;
	/*
		Descriptor sets etc.
	 */
} vulkan_pipeline_layout_t;


vulkan_pipeline_layout_t* vulkan_pipeline_layout_new();
vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer);
void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_t* layout);
void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout, renderer_t* renderer);
void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* layout);

