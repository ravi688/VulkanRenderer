
#pragma once

#include <vulkan/vulkan_wrapper.h>

typedef struct renderer_t renderer_t;

typedef struct vulkan_buffer_create_info_t
{
	void* data;
	uint32_t stride;
	uint32_t count;
	VkBufferUsageFlags usage_flags;
	VkSharingMode sharing_mode;
	VkMemoryPropertyFlags memory_property_flags;
} vulkan_buffer_create_info_t;

typedef struct vulkan_buffer_t
{
	VkBuffer handle;
	VkDeviceMemory memory;
	uint32_t stride;
	uint32_t count;
} vulkan_buffer_t;

void vulkan_buffer_init(vulkan_buffer_t* buffer);
vulkan_buffer_t* vulkan_buffer_new();
vulkan_buffer_t* vulkan_buffer_create(renderer_t* renderer, vulkan_buffer_create_info_t* create_info);
void vulkan_buffer_create_no_alloc(renderer_t* renderer, vulkan_buffer_create_info_t* create_info, vulkan_buffer_t* buffer);
void vulkan_buffer_destroy(vulkan_buffer_t* buffer, renderer_t* renderer);
void vulkan_buffer_release_resources(vulkan_buffer_t* buffer);
