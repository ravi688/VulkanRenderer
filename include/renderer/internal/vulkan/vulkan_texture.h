
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_image_t vulkan_image_t;

typedef struct vulkan_texture_create_info_t
{
	void* data;
	u32 width;
	u32 height;
	u8 channel_count;
} vulkan_texture_create_info_t;

typedef struct vulkan_texture_t
{
	vulkan_image_t* image;
	VkImageView image_view;
	VkSampler image_sampler;
	VkDeviceMemory image_memory;
} vulkan_texture_t;

vulkan_texture_t* vulkan_texture_create(renderer_t* renderer, vulkan_texture_create_info_t* create_info);
void vulkan_texture_destroy(vulkan_texture_t* texture, renderer_t* renderer);
void vulkan_texture_release_resources(vulkan_texture_t* texture);
