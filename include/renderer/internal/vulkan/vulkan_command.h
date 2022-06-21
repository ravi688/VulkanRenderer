
#pragma once


#include <renderer/defines.h>
#include <vulkan/vulkan.h>

typedef enum vulkan_image_layout_transition_type_t
{
	VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_CUSTOM = 0,
	
	/* initial layout must be VK_IMAGE_LAYOUT_UNDEFINED --> VK_IMAGE_LAYOUT_COLOR_ATTACHMENT */
	VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_SWAPCHAIN_READY_FOR_RENDER,

	/* initial layout must be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT --> VK_IMAGE_LAYOUT_PRESENT_KHR */
	VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_SWAPCHAIN_READY_FOR_PRESENT,

	/* initial layout must be VK_IMAGE_LAYOUT_UNDEFINED --> VK_IMAGE_LAYOUT_COLOR_ATTACHMENT */
	VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_RENDER_TARGET_READY_FOR_RENDER,

	/* initial layout must be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT --> VK_IMAGE_LAYOUT_SHADER_READONLY_OPTIMAL */
	VULKAN_IMAGE_LAYOUT_TRANSITION_TYPE_RENDER_TARGET_READY_FOR_SAMPLE
} vulkan_image_layout_transition_type_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API void vulkan_command_image_layout_transition(VkCommandBuffer cb, VkImage image, vulkan_image_layout_transition_type_t transition_type, ...);


END_CPP_COMPATIBLE
