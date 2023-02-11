#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

RENDERER_API vulkan_image_t* vulkan_image_new(memory_allocator_t* allocator)
{
	vulkan_image_t* image = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_IMAGE, vulkan_image_t);
	memzero(image, vulkan_image_t);
	return image;
}

RENDERER_API void vulkan_image_create_no_alloc(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info, vulkan_image_t* image)
{
	assert(!((create_info->vo_type == VK_IMAGE_TYPE_2D) && (create_info->depth > 1)));
	_debug_assert__(create_info->depth != 0);

	memzero(image, vulkan_image_t);

	image->renderer = renderer;
	VkImageCreateInfo image_info =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = create_info->vo_flags,
		.imageType = create_info->vo_type,
		.extent = { .width = create_info->width, .height = create_info->height, .depth = create_info->depth },
		.mipLevels = 1,
		.arrayLayers = create_info->layer_count,
		.format = create_info->vo_format,
		.tiling = create_info->vo_tiling,
		.initialLayout = create_info->vo_layout,
		.usage = create_info->vo_usage_mask,
		.sharingMode = renderer->vo_sharing_mode,
		.samples = VK_SAMPLE_COUNT_1_BIT
	};
	vkCall(vkCreateImage(renderer->logical_device->vo_handle, &image_info, NULL, &image->vo_handle));
	
	// get memory requirements
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(renderer->logical_device->vo_handle, image->vo_handle, &memory_requirements);

	// allocate device memory
	VkMemoryAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = vulkan_physical_device_find_memory_type(renderer->physical_device, memory_requirements.memoryTypeBits, create_info->vo_memory_properties_mask)
	};
	vkCall(vkAllocateMemory(renderer->logical_device->vo_handle, &alloc_info, NULL, &image->vo_memory));
	vkCall(vkBindImageMemory(renderer->logical_device->vo_handle, image->vo_handle, image->vo_memory, 0));
	
	image->vo_type = create_info->vo_type;
	image->vo_format = create_info->vo_format;
	image->vo_aspect_mask = create_info->vo_aspect_mask;
	image->width = create_info->width;
	image->height = create_info->height;
	image->depth = create_info->depth;
	image->layer_count = create_info->layer_count;
}


RENDERER_API vulkan_image_t* vulkan_image_create(vulkan_renderer_t* renderer, vulkan_image_create_info_t* create_info)
{
	vulkan_image_t* image = vulkan_image_new(renderer->allocator);
	vulkan_image_create_no_alloc(renderer, create_info, image);
	return image;
}

RENDERER_API void vulkan_image_destroy(vulkan_image_t* image)
{
	_debug_assert__(image != NULL);
	vkDestroyImage(image->renderer->logical_device->vo_handle, image->vo_handle, NULL);
	vkFreeMemory(image->renderer->logical_device->vo_handle, image->vo_memory, NULL);
}

RENDERER_API void vulkan_image_release_resources(vulkan_image_t* image)
{
	// TODO
	// heap_free(image);
}
