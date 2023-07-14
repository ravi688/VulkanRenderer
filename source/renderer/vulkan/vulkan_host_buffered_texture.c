#include <renderer/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_new(memory_allocator_t* allocator)
{
	vulkan_host_buffered_texture_t* texture = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_HOST_BUFFERED_TEXTURE, vulkan_host_buffered_texture_t);
	memzero(texture, vulkan_host_buffered_texture_t);
	return texture;
}

RENDERER_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_create(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info)
{
	vulkan_host_buffered_texture_t* texture = vulkan_host_buffered_texture_new(renderer->allocator);
	vulkan_host_buffered_texture_create_no_alloc(renderer, create_info, texture);
	return texture;
}

RENDERER_API void vulkan_host_buffered_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info, vulkan_host_buffered_texture_t OUT texture)
{
	memzero(texture, vulkan_host_buffered_texture_t);
	texture->renderer = renderer;

	/* currently host buffered cube textures are not supported */
	_release_assert__((create_info->type & VULKAN_TEXTURE_TYPE_CUBE) == 0);

	/* create the texture */
	vulkan_texture_create_info_t _create_info =
	{
		.width = create_info->width,
		.height = create_info->height,
		.depth = create_info->depth,
		.channel_count = create_info->channel_count,
		.type = create_info->type,
		.initial_usage = VULKAN_TEXTURE_USAGE_TRANSFER_DST,
		.usage = create_info->usage,
		.final_usage = create_info->final_usage
	};
	vulkan_texture_create_no_alloc(renderer, &_create_info, BASE(texture));

	/* create the host buffered buffer */
	vulkan_host_buffered_buffer_create_info_t buffer_create_info = 
	{
		.stride = 1,
		.capacity = create_info->width * create_info->height,
		.vo_usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	};
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &texture->buffer);

	/* create 2d view for the above linear buffer */
	buffer2d_view_create_info_t view_create_info = 
	{
		.size = { create_info->width, create_info->height },
		.buffer = vulkan_host_buffered_buffer_get_host_buffer(&texture->buffer),
	};
	buffer2d_view_create_no_alloc(renderer->allocator, &view_create_info, &texture->view);
}

RENDERER_API void vulkan_host_buffered_texture_destroy(vulkan_host_buffered_texture_t* texture)
{
	vulkan_texture_destroy(BASE(texture));
	vulkan_host_buffered_buffer_destroy(&texture->buffer);
	buffer2d_view_destroy(&texture->view);
}

RENDERER_API void vulkan_host_buffered_texture_release_resources(vulkan_host_buffered_texture_t* texture)
{
	memory_allocator_dealloc(texture->renderer->allocator, texture);
}


RENDERER_API bool vulkan_host_buffered_texture_commit(vulkan_host_buffered_texture_t* texture, bool OUT is_resized)
{
	if(!texture->is_dirty)
		return false;

	texture->is_dirty = false;
	
	/* transfer the data from host to the staging device buffer via PCI-E */
	bool _is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&texture->buffer, &_is_resized);

	/* if the host side and device side linear buffers has been resized then resize the VkImage and corresponding VkDeviceMemory */
	if(_is_resized)
	{
		vulkan_texture_recreate_info_t recreate_info = 
		{
			.width = texture->view.size.width,
			.height = texture->view.size.height,
			.depth = BASE(texture)->depth,
			.channel_count = BASE(texture)->channel_count
		};
		vulkan_texture_recreate(BASE(texture), &recreate_info);
	}

	if(is_updated)
	{
		_debug_assert__(vulkan_texture_get_image_view_count(BASE(texture)) == 1);
		/* save the current usage stage */
		AUTO stage = vulkan_texture_get_usage_stage(BASE(texture));
		/* transition the image layout to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL */
		vulkan_texture_set_usage_stage(BASE(texture), VULKAN_TEXTURE_USAGE_STAGE_INITIAL);
		/* transfer the data from staging device buffer to the VkDeviceMemory of the texture  */
		vulkan_image_view_copy_from_buffer(vulkan_texture_get_image_views(BASE(texture)), vulkan_host_buffered_buffer_get_device_buffer(&texture->buffer));
		/* restore the usage stage */
		vulkan_texture_set_usage_stage(BASE(texture), stage);
	}

	if(is_resized != NULL)
		OUT is_resized = _is_resized;

	return is_updated;
}
