
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;
typedef struct render_window_t render_window_t;
typedef struct vulkan_image_t vulkan_image_t;
typedef struct vulkan_image_view_t vulkan_image_view_t; 

typedef struct vulkan_swapchain_create_info_t
{
	VkFormat image_format;
	VkFormat depth_format;
	VkColorSpaceKHR image_color_space;
	u32 image_count;
	VkExtent2D image_extent;
	VkSharingMode image_sharing_mode;
	VkPresentModeKHR present_mode;
	u32 queue_family_index_count;
	u32* queue_family_indices;
} vulkan_swapchain_create_info_t;

typedef struct vulkan_swapchain_t
{
	VkSwapchainKHR handle;
	vulkan_image_t* depth_image;
	vulkan_image_view_t* depth_image_view;
	VkImage* images;
	VkImageView* image_views;
	VkFramebuffer* framebuffers;
	VkImageView* framebuffer_image_views;
	VkExtent2D image_extent;
	u32 image_count;
	u32 current_image_index;
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
} vulkan_swapchain_t;

vulkan_swapchain_t* vulkan_swapchain_new();
vulkan_swapchain_t* vulkan_swapchain_create(renderer_t* renderer, vulkan_swapchain_create_info_t* create_info);
void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain, renderer_t* renderer);
void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain);
void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, renderer_t* renderer, vulkan_swapchain_create_info_t* create_info);
u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, renderer_t* renderer);

