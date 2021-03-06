
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_pipeline_layout_create_info_t
{
	/* list of vulkan descriptor set layouts  */
	VkDescriptorSetLayout* vo_set_layouts;
	u32 set_layout_count;

	/* list of vulkan push constant ranges */
	VkPushConstantRange* vo_push_constant_ranges;
	u32 push_constant_range_count;
} vulkan_pipeline_layout_create_info_t;

typedef struct vulkan_pipeline_layout_t
{
	vulkan_renderer_t* renderer;
	VkPipelineLayout vo_handle;
} vulkan_pipeline_layout_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_new();
RENDERER_API vulkan_pipeline_layout_t* vulkan_pipeline_layout_create(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info);
RENDERER_API void vulkan_pipeline_layout_create_no_alloc(vulkan_renderer_t* renderer, vulkan_pipeline_layout_create_info_t* create_info, vulkan_pipeline_layout_t OUT layout);
RENDERER_API void vulkan_pipeline_layout_destroy(vulkan_pipeline_layout_t* layout);
RENDERER_API void vulkan_pipeline_layout_release_resources(vulkan_pipeline_layout_t* layout);

RENDERER_API void vulkan_pipeline_layout_push_constants(vulkan_pipeline_layout_t* layout, VkShaderStageFlagBits stage_flags, u32 offset, u32 size, void* bytes);

END_CPP_COMPATIBLE
