#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_result.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

#ifdef GLOBAL_DEBUG
	static void check_pre_condition(vulkan_image_view_t* view);
#else
#	define check_pre_condition(view)
#endif /* GLOBAL_DEBUG */

static VkImageViewType get_view_type(vulkan_image_view_type_t type);
static u32 get_layer_count(vulkan_image_view_type_t type);

RENDERER_API vulkan_image_view_t* vulkan_image_view_new()
{
	vulkan_image_view_t* view = heap_new(vulkan_image_view_t);
	memset(view, 0, sizeof(vulkan_image_view_t));
	return view;
}

RENDERER_API vulkan_image_view_t* vulkan_image_view_create(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info)
{
	vulkan_image_view_t* view = vulkan_image_view_new();
	vulkan_image_view_create_no_alloc(renderer, create_info, view);
	return view;
}

RENDERER_API void vulkan_image_view_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_view_create_info_t* create_info, vulkan_image_view_t OUT view)
{
	memzero(view, vulkan_image_view_t);

	view->renderer = renderer;
	VkImageViewCreateInfo view_create_info  =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = create_info->image->vo_handle,
		.viewType = get_view_type(create_info->view_type),
		.format = create_info->image->vo_format,
		.subresourceRange.aspectMask = create_info->image->vo_aspect_mask,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = get_layer_count(create_info->view_type)
	};
	view->layer_count = view_create_info.subresourceRange.layerCount;
	switch(create_info->image->vo_format)
	{
		case VK_FORMAT_R8G8B8A8_UNORM:
    	case VK_FORMAT_R8G8B8A8_SNORM:
    	case VK_FORMAT_R8G8B8A8_USCALED:
    	case VK_FORMAT_R8G8B8A8_SSCALED:
    	case VK_FORMAT_R8G8B8A8_UINT:
    	case VK_FORMAT_R8G8B8A8_SINT:
    	case VK_FORMAT_R8G8B8A8_SRGB:
    		view_create_info.components = (VkComponentMapping) { VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R };
    	break;
	}
	VkResult result = vkCreateImageView(view->renderer->logical_device->vo_handle, &view_create_info, NULL, &view->vo_handle);
	vulkan_result_assert_success(result);
}

RENDERER_API void vulkan_image_view_destroy(vulkan_image_view_t* view)
{
	check_pre_condition(view);
	vkDestroyImageView(view->renderer->logical_device->vo_handle, view->vo_handle, NULL);
}

RENDERER_API void vulkan_image_view_release_resources(vulkan_image_view_t* view)
{
	check_pre_condition(view);
	heap_free(view);
}

static VkImageViewType get_view_type(vulkan_image_view_type_t type)
{
	switch(type)
	{
    	case VULKAN_IMAGE_VIEW_TYPE_1D:
    		return VK_IMAGE_VIEW_TYPE_1D;
    	case VULKAN_IMAGE_VIEW_TYPE_2D:
    		return VK_IMAGE_VIEW_TYPE_2D;
    	case VULKAN_IMAGE_VIEW_TYPE_3D:
    		return VK_IMAGE_VIEW_TYPE_3D;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE:
    		return VK_IMAGE_VIEW_TYPE_CUBE;
    	case VULKAN_IMAGE_VIEW_TYPE_1D_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    	case VULKAN_IMAGE_VIEW_TYPE_2D_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE_ARRAY:
    		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    	default:
    		log_wrn("Vulkan image view type is invalid, using VK_IMAGE_VIEW_TYPE_2D\n");
    		return VK_IMAGE_VIEW_TYPE_2D;
	}
}

static u32 get_layer_count(vulkan_image_view_type_t type)
{
	switch(type)
	{
		case VULKAN_IMAGE_VIEW_TYPE_1D:
    	case VULKAN_IMAGE_VIEW_TYPE_2D:
    	case VULKAN_IMAGE_VIEW_TYPE_3D:
    		return 1;
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE:
    		return 6;
    	case VULKAN_IMAGE_VIEW_TYPE_1D_ARRAY:
    	case VULKAN_IMAGE_VIEW_TYPE_2D_ARRAY:
    	case VULKAN_IMAGE_VIEW_TYPE_CUBE_ARRAY:
    		log_wrn("Layer count isn't defined for VULKAN_IMAGE_VIEW_TYPE_*_ARRAY types, using 1\n");
    		return 1;
    	default:
    		log_wrn("Vulkan image view type is invalid, using VK_IMAGE_VIEW_TYPE_2D\n");
    		return VK_IMAGE_VIEW_TYPE_2D;
	}
}

#ifdef GLOBAL_DEBUG
static void check_pre_condition(vulkan_image_view_t* view)
{
	assert(view != NULL);
}
#endif /* GLOBAL_DEBUG */
