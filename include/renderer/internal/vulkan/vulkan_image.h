
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef struct vulkan_image_create_info_t
{
	VkImageCreateFlags vo_flags;						// VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT for cube map images
	VkImageType vo_type;								// VK_IMAGE_TYPE_1D, VK_IMAGE_TYPE_2D or VK_IMAGE_TYPE_3D
	VkFormat vo_format;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
	VkImageTiling vo_tiling; 							// VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL
	VkImageLayout vo_layout;	 						// VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED
	VkImageUsageFlags vo_usage_mask; 					// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	VkMemoryPropertyFlags vo_memory_properties_mask;  	// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPRETY_HOST_COHERENT_BIT 
	VkImageAspectFlags vo_aspect_mask; 				// VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT
} vulkan_image_create_info_t;

typedef struct vulkan_image_t
{
	vulkan_renderer_t* renderer;
	VkImage vo_handle;
	VkDeviceMemory vo_memory;
	VkImageType vo_type;
	VkFormat vo_format;
	VkImageAspectFlags vo_aspect_mask;
	u32 width;
	u32 height;
	u32 depth;
	u32 layer_count;
} vulkan_image_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_image_t* vulkan_image_new();
RENDERER_API vulkan_image_t* vulkan_image_create(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info);
RENDERER_API void vulkan_image_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* out_image);
RENDERER_API void vulkan_image_destroy(vulkan_image_t* image);
RENDERER_API void vulkan_image_release_resources(vulkan_image_t* image);

END_CPP_COMPATIBLE
