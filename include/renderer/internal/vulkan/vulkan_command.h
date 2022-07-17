
#pragma once


#include <renderer/defines.h>
#include <vulkan/vulkan.h>

BEGIN_CPP_COMPATIBLE

RENDERER_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image,
		VkImageSubresourceRange* subresource,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask,
		VkAccessFlags dst_access_mask,
		VkPipelineStageFlags src_pipeline_stage,
		VkPipelineStageFlags dst_pipeline_stage);

END_CPP_COMPATIBLE
