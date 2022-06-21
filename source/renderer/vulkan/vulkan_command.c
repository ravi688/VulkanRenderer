
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/assert.h>

#include <stdarg.h>

RENDERER_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image, vulkan_image_layout_transition_type_t transition_type, ...)
{
	VkImageMemoryBarrier barrier = 
	{ 
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0
	};
	VkPipelineStageFlags src_pipeline_stage;
	VkPipelineStageFlags dst_pipeline_stage;
	switch(transition_type)
	{
		case VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_SWAPCHAIN_READY_FOR_RENDER :
			barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			src_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
		case VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_SWAPCHAIN_READY_FOR_PRESENT :
			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			src_pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dst_pipeline_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
		case VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_RENDER_TARGET_READY_FOR_RENDER :
			barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			src_pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dst_pipeline_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
		case VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_RENDER_TARGET_READY_FOR_SAMPLE :
			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			src_pipeline_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dst_pipeline_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;
		case VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_CUSTOM:
			va_list args;
			va_start(args, transition_type);
			barrier.oldLayout = va_arg(args, VkImageLayout);
			barrier.newLayout = va_arg(args, VkImageLayout);
			barrier.subresourceRange.aspectMask = va_arg(args, VkImageAspectFlagBits);
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = va_arg(args, VkAccessFlags);
			barrier.dstAccessMask = va_arg(args, VkAccessFlags);
			src_pipeline_stage = va_arg(args, VkPipelineStageFlags);
			dst_pipeline_stage = va_arg(args, VkPipelineStageFlags);
			va_end(args);
		break;
		default:
			LOG_FETAL_ERR("Unsupport image layout transition type: %u\n", transition_type);
	}
	vkCmdPipelineBarrier(cb, src_pipeline_stage, dst_pipeline_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
}
