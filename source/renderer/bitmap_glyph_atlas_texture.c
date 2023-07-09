#include <renderer/bitmap_glyph_atlas_texture.h>
#include <renderer/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <renderer/renderer.h> // VULKAN_RENDERER()

RENDERER_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator)
{
	return vulkan_bitmap_glyph_atlas_texture_new(allocator);
}

static vulkan_bitmap_glyph_atlas_texture_create_info_t get_vk_create_info(bitmap_glyph_atlas_texture_create_info_t* create_info)
{
	return (vulkan_bitmap_glyph_atlas_texture_create_info_t)
	{
		.width = create_info->width,
		.height = create_info->height,
		.font = create_info->font
	};
}

RENDERER_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_create(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info)
{
	vulkan_bitmap_glyph_atlas_texture_create_info_t vk_create_info = get_vk_create_info(create_info);
	return vulkan_bitmap_glyph_atlas_texture_create(VULKAN_RENDERER(renderer), &vk_create_info);
}

RENDERER_API void bitmap_glyph_atlas_texture_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info, bitmap_glyph_atlas_texture_t OUT texture)
{
	vulkan_bitmap_glyph_atlas_texture_create_info_t vk_create_info = get_vk_create_info(create_info);
	vulkan_bitmap_glyph_atlas_texture_create_no_alloc(VULKAN_RENDERER(renderer), &vk_create_info, texture);
}

RENDERER_API void bitmap_glyph_atlas_texture_destroy(bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_glyph_atlas_texture_destroy(texture);	
}

RENDERER_API void bitmap_glyph_atlas_texture_release_resources(bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_glyph_atlas_texture_release_resources(texture);
}

RENDERER_API bitmap_glyph_pool_t* bitmap_glyph_atlas_texture_get_pool(bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_bitmap_glyph_atlas_texture_get_pool(texture);
}

RENDERER_API font_t* bitmap_glyph_atlas_texture_get_font(bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_bitmap_glyph_atlas_texture_get_font(texture);
}

RENDERER_API bool bitmap_glyph_atlas_texture_commit(bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized)
{
	return vulkan_bitmap_glyph_atlas_texture_commit(texture, is_resized);
}

RENDERER_API bool bitmap_glyph_atlas_texture_get_texcoord(bitmap_glyph_atlas_texture_t* texture, utf32_t unicode, glyph_texcoord_t OUT texcoord)
{
	return vulkan_bitmap_glyph_atlas_texture_get_texcoord(texture, unicode, texcoord);
}

#ifdef GLOBAL_DEBUG
RENDERER_API void bitmap_glyph_atlas_texture_dump(bitmap_glyph_atlas_texture_t* texture, const char* file_path)
{
	vulkan_bitmap_glyph_atlas_texture_dump(texture, file_path);
}
#endif /* GLOBAL_DEBUG */
