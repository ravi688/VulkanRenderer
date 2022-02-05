
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;

typedef struct vulkan_pipeline_layout_create_info_t
{
	/* For now, we will be using only one descriptor set layout per pipeline layout */
	VkDescriptorSetLayout vk_set_layout;
	VkPushConstantRange* push_constant_ranges;
	u32 push_constant_range_count;
} vulkan_pipeline_layout_create_info_t;

typedef struct vulkan_pipeline_layout_t
{
	VkPipelineLayout handle;
} vulkan_pipeline_layout_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_new();
RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info);
RENDERER_API void vulkan_pipeline_layout_create_no_alloc(renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t* pipeline_layout);
RENDERER_API void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer);
RENDERER_API void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* pipeline_layout);

RENDERER_API void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* pipeline_layout, renderer_t* renderer, VkShaderStageFlagBits stage_flags, u32 offset, u32 size, void* bytes);

END_CPP_COMPATIBLE
