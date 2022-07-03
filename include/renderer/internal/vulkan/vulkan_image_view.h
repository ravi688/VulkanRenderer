
#pragma once


#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_image_t vulkan_image_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

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

typedef struct vulkan_image_view_create_info_t
{
	VkImageLayout vo_layout;	 // layout of the image in the subresourceRange at the time of this view creation
	vulkan_image_t* image;
	vulkan_image_view_type_t view_type;
	u32 base_mip_level;
	u32 level_count;
	u32 base_array_layer;

	/* this could be left zero, layer count will be automatically deduced from the view type 
	   for example: VULKAN_IMAGE_VIEW_TYPE_2D = 1
	   				VULKAN_IMAGE_VIEW_TYPE_CUBE = 6
	 */
	u32 layer_count;
} vulkan_image_view_create_info_t;


typedef struct vulkan_image_view_t
{
	vulkan_renderer_t* renderer;
	VkImageView vo_handle;
	vulkan_image_t* image;
	vulkan_image_view_type_t type;
	VkImageSubresourceRange vo_subresource_range;
	VkImageLayout vo_layout;
} vulkan_image_view_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_image_view_t* vulkan_image_view_new();
RENDERER_API vulkan_image_view_t* vulkan_image_view_create(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info);
RENDERER_API void vulkan_image_view_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info, vulkan_image_view_t OUT view);
RENDERER_API void vulkan_image_view_destroy(vulkan_image_view_t* view);
RENDERER_API void vulkan_image_view_release_resources(vulkan_image_view_t* view);

/* logic functions */
#define VULKAN_IMAGE_VIEW_DONT_CARE_OLD_LAYOUT 1
RENDERER_API void vulkan_image_view_transition_layout_to(vulkan_image_view_t* view, u32 flags, VkImageLayout layout);
RENDERER_API void vulkan_image_view_upload_data(vulkan_image_view_t* view, void* data);
RENDERER_API void vulkan_image_view_copy_from_buffer(vulkan_image_view_t* view, vulkan_buffer_t* buffer);

END_CPP_COMPATIBLE
