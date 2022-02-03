
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef struct vulkan_image_create_info_t
{
	VkImageCreateFlags flags;						// VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT for cube map images
	VkImageType type;								// VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D or VK_IMAGE_TYPE_3D
	VkFormat format;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
	VkImageTiling tiling; 							// VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL
	VkImageLayout layout;	 						// VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED
	VkImageUsageFlags usage_mask; 					// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	VkMemoryPropertyFlags memory_properties_mask;  	// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPRETY_HOST_COHERENT_BIT 
	VkImageAspectFlags aspect_mask; 				// VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT
} vulkan_image_create_info_t;

typedef struct vulkan_image_t
{
	renderer_t* renderer;
	VkImage handle;
	VkDeviceMemory memory;
	VkImageType type;
	VkFormat format;
	VkImageAspectFlags aspect_mask;
	VkImageLayout layout;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
} vulkan_image_t;


// constructors and destructors
vulkan_image_t* vulkan_image_new();
vulkan_image_t* vulkan_image_create(renderer_t* renderer, vulkan_image_create_info_t* create_info);
void vulkan_image_create_no_alloc(renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* out_image);
void vulkan_image_destroy(vulkan_image_t* image);
void vulkan_image_release_resources(vulkan_image_t* image);

// logic functions
void vulkan_image_transition_layout_to(vulkan_image_t* image, VkImageLayout layout);
void vulkan_image_copy_from_buffer(vulkan_image_t* image, vulkan_buffer_t* buffer);
