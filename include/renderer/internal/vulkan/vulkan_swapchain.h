
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_swapchain_create_info_t
{
	VkFormat vo_image_format;
	VkColorSpaceKHR vo_image_color_space;
	u32 image_count;
	VkExtent2D vo_image_extent;
	VkPresentModeKHR vo_present_mode;
	u32 queue_family_index_count;
	u32* queue_family_indices;
} vulkan_swapchain_create_info_t;

typedef struct vulkan_swapchain_t
{
	vulkan_renderer_t* renderer;
	VkSwapchainKHR vo_handle;
	VkImage* vo_images;
	VkImageView* vo_image_views;
	VkExtent2D vo_image_extent;
	u32 image_count;
	u32 current_image_index;
	VkSemaphore vo_image_available_semaphore;
	VkSemaphore vo_render_finished_semaphore;
} vulkan_swapchain_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_swapchain_t* vulkan_swapchain_new();
RENDERER_API vulkan_swapchain_t* vulkan_swapchain_create(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info);
RENDERER_API void vulkan_swapchain_create_no_alloc(vulkan_renderer_t* renderer, vulkan_swapchain_create_info_t* create_info, vulkan_swapchain_t OUT swapchain);
RENDERER_API void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain);
RENDERER_API void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain);
RENDERER_API void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, vulkan_swapchain_create_info_t* create_info);
RENDERER_API u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain);

END_CPP_COMPATIBLE
