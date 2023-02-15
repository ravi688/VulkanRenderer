
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/assert.h>

#include <stdarg.h>
#include <renderer/alloc.h>	 // memcopyv

RENDERER_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image,
		VkImageSubresourceRange* subresource,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask,
		VkAccessFlags dst_access_mask,
		VkPipelineStageFlags src_pipeline_stage,
		VkPipelineStageFlags dst_pipeline_stage)
{
	VkImageMemoryBarrier barrier = 
	{ 
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcAccessMask = src_access_mask,
		.dstAccessMask = dst_access_mask
	};
	memcopy(&barrier.subresourceRange, subresource, VkImageSubresourceRange);
	vkCmdPipelineBarrier(cb, src_pipeline_stage, dst_pipeline_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
}
