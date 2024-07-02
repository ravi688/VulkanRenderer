/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_bitmap_glyph_atlas_texture.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <sge/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/sge.h>
#include <sge/memory_allocator.h>

SGE_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator)
{
	vulkan_bitmap_glyph_atlas_texture_t* texture = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GLYPH_ATLAS_TEXTURE, vulkan_bitmap_glyph_atlas_texture_t);
	memzero(texture, vulkan_bitmap_glyph_atlas_texture_t);
	VULKAN_OBJECT_INIT(texture, VULKAN_OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return texture;
}

SGE_API vulkan_bitmap_glyph_atlas_texture_t* vulkan_bitmap_glyph_atlas_texture_create(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info)
{
	 vulkan_bitmap_glyph_atlas_texture_t* texture = vulkan_bitmap_glyph_atlas_texture_new(renderer->allocator);
	 vulkan_bitmap_glyph_atlas_texture_create_no_alloc(renderer, create_info, texture);
	return texture;
}

SGE_API void vulkan_bitmap_glyph_atlas_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_create_info_t* create_info, vulkan_bitmap_glyph_atlas_texture_t OUT texture)
{
	VULKAN_OBJECT_SET_BASE(texture, true);
	VULKAN_OBJECT_MEMZERO(texture, vulkan_bitmap_glyph_atlas_texture_t);

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
	VULKAN_OBJECT_INIT(BASE(texture), VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_host_buffered_texture_create_no_alloc(renderer, &hbt_create_info, BASE(texture));
	vulkan_texture_set_usage_stage(VULKAN_TEXTURE(texture), VULKAN_TEXTURE_USAGE_STAGE_FINAL);

	/* create bitmap glyph pool */
	bitmap_glyph_pool_create_info_t bgp_create_info =
	{
		.size = { create_info->width, create_info->height },
		.font = create_info->font,
		.buffer = vulkan_host_buffered_texture_get_host_buffer(BASE(texture)),
		.view = vulkan_host_buffered_texture_get_view(BASE(texture))
	};
	bitmap_glyph_pool_create_no_alloc_ext(RENDERER(renderer), &bgp_create_info, &texture->pool);

	texture->on_resize_event = event_create(renderer->allocator, texture PARAM_IF_DEBUG("Vulkan-Glyph-Atlas-Texture-Resize"));
}

SGE_API void vulkan_bitmap_glyph_atlas_texture_destroy(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_host_buffered_texture_destroy(BASE(texture));
	bitmap_glyph_pool_destroy(&texture->pool);
	event_destroy(texture->on_resize_event);
}

SGE_API void vulkan_bitmap_glyph_atlas_texture_release_resources(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	event_release_resources(texture->on_resize_event);
	bitmap_glyph_pool_release_resources(&texture->pool);
	vulkan_host_buffered_texture_release_resources(BASE(texture));
	if(VULKAN_OBJECT_IS_INTERNAL(texture))
		memory_allocator_dealloc(texture->renderer->allocator, texture);
}

SGE_API bool vulkan_bitmap_glyph_atlas_texture_commit(vulkan_bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized)
{
	bool _is_resized = false;
	bool result = vulkan_host_buffered_texture_commit(BASE(texture), &_is_resized);
	if(_is_resized)
	{
		event_publish(texture->on_resize_event);
		if(is_resized != NULL)
			OUT is_resized = _is_resized;
	}
	return result;
}
