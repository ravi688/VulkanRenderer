
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_attachment_t* vulkan_attachment_new()
{
	vulkan_attachment_t* attachment = heap_new(vulkan_attachment_t);
	memset(attachment, 0, sizeof(vulkan_attachment_t));
	return attachment;
}

RENDERER_API vulkan_attachment_t* vulkan_attachment_create(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info)
{
	vulkan_attachment_t* attachment = vulkan_attachment_new();
	vulkan_attachment_create_no_alloc(renderer, create_info, attachment);
	return attachment;
}

static VkFormat get_image_format_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_FORMAT_D32_SFLOAT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			LOG_FETAL_ERROR("VULKAN_ATTACHMENT_TYPE_STENCIL isn't supported yet!\n");
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			LOG_FETAL_ERROR("VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL isn't supported yet!\n");
		default:
			LOG_FETAL_ERROR("Unsupported attachment type %u for any of the image format\n", type);
	}
	return VK_FORMAT_UNDEFINED;
}

static VkImageUsageFlags get_image_usage_from_format(VkFormat format, vulkan_attachment_next_pass_usage_t usage)
{
	VkImageUsageFlags flags = 0;
	switch(format)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			break;
		case VK_FORMAT_D32_SFLOAT:
			flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default:
			LOG_FETAL_ERROR("Unsupported format %u for any of the image usages\n", format);
	}
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT) ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0;
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED) ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
	return flags;
}

static VkImageUsageFlags get_image_usage_from_attachment_type(vulkan_attachment_type_t type, vulkan_attachment_next_pass_usage_t usage)
{
	VkImageUsageFlags flags = 0;
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			 flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			 break;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default:
			LOG_FETAL_ERROR("Unsupported attachment type %u for any of the image usages\n", type);
	}
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT) ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT : 0;
	flags |= (usage & VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED) ? VK_IMAGE_USAGE_SAMPLED_BIT : 0;
	return flags;
}

static VkImageAspectFlags get_image_aspect_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			return VK_IMAGE_ASPECT_STENCIL_BIT;
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			return VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT
		default:
			LOG_FETAL_ERROR("Unsupported attachment type : %u\n", type);
	}
	return 0;
}

static VkImageAspectFlags get_image_aspect_from_format(VkFormat format)
{
	switch(type)
	{
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		default:
			LOG_FETAL_ERROR("Unsupported format %u for any of the image aspect flags\n", format);
	}
	return 0;
}

RENDERER_API void vulkan_attachment_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info, vulkan_attachment_t OUT attachment)
{
	attachment->renderer = renderer;

	// create image for this attachment
	vulkan_image_create_info_t image_create_info = 
	{
		.type = VK_IMAGE_TYPE_2D,
		.format = create_info->format ? create_info->format : get_image_format_from_attachment_type(create_info->attachment_type),
		.width = create_info->width,
		.height = create_info->height,
		.depth = 1,
		.layer_count = 1,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.layout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage_mask = create_info->format ? get_image_usage_from_format(create_info->format, create_info->usage) : get_image_usage_from_attachment_type(create_info->attachment_type, create_info->usage),
		.memory_properties_mask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		.aspect_mask = create_info->format ? get_image_aspect_from_format(create_info->format) : get_image_aspect_from_attachment_type(create_info->attachment_type)
	};
	vulkan_image_create_no_alloc(renderer, &image_create_info, &attachment->image);

	// create image view for this attachment
	vulkan_image_view_create_no_alloc(renderer, &attachment->image, VULKAN_IMAGE_VIEW_TYPE_2D, &attachment->image_view);
}

RENDERER_API void vulkan_attachment_destroy(vulkan_attachment_t* attachment)
{
	vulkan_image_destroy(&attachment->image);
	vulkan_image_view_destroy(&attachment->image_view);
}

RENDERER_API void vulkan_attachment_release_resources(vulkan_attachment_t* attachment)
{
	heap_free(attachment);
}

RENDERER_API vulkan_image_t* vulkan_attachment_get_image(vulkan_attachment_t* attachment)
{
	return &attachment->image;
}

RENDERER_API vulkan_image_view_t* vulkan_attachment_get_image_view(vulkan_attachment_t* attachment)
{
	return &attachment->image_view;
}

