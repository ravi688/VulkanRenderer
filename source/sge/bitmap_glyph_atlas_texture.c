/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_glyph_atlas_texture.c is a part of VulkanRenderer

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

#include <sge/bitmap_glyph_atlas_texture.h>
#include <sge/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <sge/sge.h> // VULKAN_RENDERER()

SGE_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator)
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

SGE_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_create(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info)
{
	vulkan_bitmap_glyph_atlas_texture_create_info_t vk_create_info = get_vk_create_info(create_info);
	return vulkan_bitmap_glyph_atlas_texture_create(VULKAN_RENDERER(renderer), &vk_create_info);
}

SGE_API void bitmap_glyph_atlas_texture_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info, bitmap_glyph_atlas_texture_t OUT texture)
{
	vulkan_bitmap_glyph_atlas_texture_create_info_t vk_create_info = get_vk_create_info(create_info);
	vulkan_bitmap_glyph_atlas_texture_create_no_alloc(VULKAN_RENDERER(renderer), &vk_create_info, texture);
}

SGE_API void bitmap_glyph_atlas_texture_destroy(bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_glyph_atlas_texture_destroy(texture);
}

SGE_API void bitmap_glyph_atlas_texture_release_resources(bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_glyph_atlas_texture_release_resources(texture);
}

SGE_API bitmap_glyph_pool_t* bitmap_glyph_atlas_texture_get_pool(bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_bitmap_glyph_atlas_texture_get_pool(texture);
}

SGE_API font_t* bitmap_glyph_atlas_texture_get_font(bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_bitmap_glyph_atlas_texture_get_font(texture);
}

SGE_API texture_t* bitmap_glyph_atlas_texture_get_texture(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	return vulkan_bitmap_glyph_atlas_texture_get_texture(texture);
}

SGE_API bool bitmap_glyph_atlas_texture_commit(bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized)
{
	return vulkan_bitmap_glyph_atlas_texture_commit(texture, is_resized);
}

SGE_API bool bitmap_glyph_atlas_texture_get_texcoord(bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord)
{
	return vulkan_bitmap_glyph_atlas_texture_get_texcoord(texture, unicode, texcoord);
}

#ifdef GLOBAL_DEBUG
SGE_API void bitmap_glyph_atlas_texture_dump(bitmap_glyph_atlas_texture_t* texture, const char* file_path)
{
	vulkan_bitmap_glyph_atlas_texture_dump(texture, file_path);
}
SGE_API void bitmap_glyph_atlas_texture_dump_bb(bitmap_glyph_atlas_texture_t* texture, const char* file_path)
{
	vulkan_bitmap_glyph_atlas_texture_dump_bb(texture, file_path);
}
#endif /* GLOBAL_DEBUG */
