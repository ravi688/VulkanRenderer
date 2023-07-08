
#include <renderer/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/renderer.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator)
{
	vulkan_bitmap_glyph_atlas_texture_t* texture = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GLYPH_ATLAS_TEXTURE, vulkan_bitmap_glyph_atlas_texture_t);
	memzero(texture, vulkan_bitmap_glyph_atlas_texture_t);
	return texture;
}

RENDERER_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_create(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info)
{
	 vulkan_bitmap_glyph_atlas_texture_t* texture = vulkan_bitmap_glyph_atlas_texture_new(renderer->allocator);
	 vulkan_bitmap_glyph_atlas_texture_create_no_alloc(renderer, create_info, texture);
	return texture; 
}

RENDERER_API void vulkan_bitmap_glyph_atlas_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info, vulkan_bitmap_glyph_atlas_texture_t OUT texture)
{
	memzero(texture, vulkan_bitmap_glyph_atlas_texture_t);

	texture->renderer = renderer;

	/* create host buffered texture */
	vulkan_host_buffered_texture_create_info_t hbt_create_info = 
	{
		.width = create_info->width,
		.height = create_info->height,
		.depth = 1,
		.channel_count = 1,
		.type = VULKAN_TEXTURE_TYPE_ALBEDO,
		.usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED
	};
	vulkan_host_buffered_texture_create_no_alloc(renderer, &hbt_create_info, BASE(texture));
	vulkan_texture_set_usage_stage(VULKAN_TEXTURE(texture), VULKAN_TEXTURE_USAGE_STAGE_FINAL);

	/* create bitmap glyph pool */
	bitmap_glyph_pool_create_info_t bgp_create_info = 
	{
		.size = { create_info->width, create_info->height },
		.font = create_info->font,
		.buffer = vulkan_host_buffered_texture_get_host_buffer(BASE(texture))
	};
	bitmap_glyph_pool_create_no_alloc(RENDERER(renderer), &bgp_create_info, &texture->pool);
}

RENDERER_API void vulkan_bitmap_glyph_atlas_texture_destroy(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_host_buffered_texture_destroy(BASE(texture));
	bitmap_glyph_pool_destroy(&texture->pool);
}

RENDERER_API void vulkan_bitmap_glyph_atlas_texture_release_resources(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	memory_allocator_dealloc(texture->renderer->allocator, texture);
}

RENDERER_API void vulkan_bitmap_glyph_atlas_texture_commit(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_host_buffered_texture_commit(BASE(texture));
}
