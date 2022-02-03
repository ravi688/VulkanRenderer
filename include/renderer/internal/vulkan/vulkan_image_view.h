
#pragma once


#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_image_t vulkan_image_t;

typedef enum vulkan_image_view_type_t
{
	VULKAN_IMAGE_VIEW_TYPE_1D = 0,
	VULKAN_IMAGE_VIEW_TYPE_2D,
	VULKAN_IMAGE_VIEW_TYPE_3D,
	VULKAN_IMAGE_VIEW_TYPE_CUBE,
	VULKAN_IMAGE_VIEW_TYPE_1D_ARRAY,
	VULKAN_IMAGE_VIEW_TYPE_2D_ARRAY,
	VULKAN_IMAGE_VIEW_TYPE_3D_ARRAY,
	VULKAN_IMAGE_VIEW_TYPE_CUBE_ARRAY
} vulkan_image_view_type_t;

typedef struct vulkan_image_view_t
{
	VkImageView handle;
	renderer_t* renderer;
	vulkan_image_view_type_t type;
	u32 layer_count;
} vulkan_image_view_t;


// constructors and destructors
vulkan_image_view_t* vulkan_image_view_new();
vulkan_image_view_t* vulkan_image_view_create(vulkan_image_t* image, vulkan_image_view_type_t type);
void vulkan_image_view_create_no_alloc(vulkan_image_t* image, vulkan_image_view_type_t type, vulkan_image_view_t* out_view);
void vulkan_image_view_destroy(vulkan_image_view_t* view);
void vulkan_image_view_release_resources(vulkan_image_view_t* view);
