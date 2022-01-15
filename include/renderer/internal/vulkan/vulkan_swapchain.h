
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;
typedef struct render_window_t render_window_t;
typedef struct vulkan_image_t vulkan_image_t;

typedef struct vulkan_swapchain_t
{
	VkSwapchainKHR swapchain;
	VkSurfaceKHR surface;
	vulkan_image_t* depth_image;
	VkImageView depth_image_view;
	VkImage* images;
	VkImageView* image_views;
	VkFramebuffer* framebuffers;
	VkImageView* framebuffer_image_views;
	render_window_t* window;
	u32 image_count;
	u32 current_image_index;
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
} vulkan_swapchain_t;

vulkan_swapchain_t* vulkan_swapchain_new();
vulkan_swapchain_t* vulkan_swapchain_create(render_window_t* window, renderer_t* renderer);
void vulkan_swapchain_destroy(vulkan_swapchain_t* swapchain, renderer_t* renderer);
void vulkan_swapchain_release_resources(vulkan_swapchain_t* swapchain);
void vulkan_swapchain_refresh(vulkan_swapchain_t* swapchain, renderer_t* renderer);
u32 vulkan_swapchain_acquire_next_image(vulkan_swapchain_t* swapchain, renderer_t* renderer);

