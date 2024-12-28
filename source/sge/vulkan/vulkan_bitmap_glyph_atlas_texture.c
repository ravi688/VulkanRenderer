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
	VULKAN_OBJECT_MEMZERO(texture, vulkan_bitmap_glyph_atlas_texture_t);

	texture->renderer = renderer;

	/* create host buffered texture */
	vulkan_host_buffered_texture_create_info_t hbt_create_info =
	{
		.flags = SGE_VULKAN_FLAG_NEAREST_FILTERING,
		.width = create_info->width,
		.height = create_info->height,
		.depth = 1,
		.channel_count = 1,
		.type = VULKAN_TEXTURE_TYPE_ALBEDO,
		.usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED
	};
	VULKAN_OBJECT_INIT(BASE(texture), VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	VULKAN_OBJECT_SET_BASE(texture, true);
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

	// _debug_assert__(glsl_sizeof(vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t) == sizeof(vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t));

	/* create GTC buffer to store texture coordinates for each unique glyph */
	vulkan_host_buffered_buffer_create_info_t buffer_create_info =
	{
		/* stride to the next element of type vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t */
		.stride = STRIDE_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T_ARRAY,
		/* initially we can store texcoord coordinates of upto 127 characters, so there will be no chance of buffer resize until 512 */
		.capacity = 127,
		/* this buffer will be indexed with glyph_render_data_t.indx */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	VULKAN_OBJECT_INIT(&texture->glyph_texcoord_buffer, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &texture->glyph_texcoord_buffer);

	texture->glyph_texcoord_index_table = dictionary_create(glyph_instance_id_t, u32, 127, glyph_instance_id_equal_to);

	texture->on_resize_event = event_create(renderer->allocator, texture PARAM_IF_DEBUG("Vulkan-Glyph-Atlas-Texture-Resize"));
	texture->on_gtc_buffer_resize_event = event_create(renderer->allocator, texture PARAM_IF_DEBUG("Vulkan-Glyph-Texcoord-Buffer-Resize"));

	_debug_assert__(renderer->is_bitmap_text);

	/* for the first time, write the new VkBuffer/VkImageView references to the descriptor at binding VULKAN_DESCRIPTOR_BINDING_BGA_TEXTURE in the global set */
	AUTO bga_texture = vulkan_bitmap_glyph_atlas_texture_get_texture(texture);
	vulkan_descriptor_set_write_texture(vulkan_renderer_get_global_set(texture->renderer), VULKAN_DESCRIPTOR_BINDING_BGA_TEXTURE, bga_texture);

	/* for the first time, write the new VkBuffer reference to the descriptor at binding VULKAN_DESCRIPTOR_BINDING_GTC_BUFFER in the global set */
	AUTO gtc_buffer = vulkan_host_buffered_buffer_get_device_buffer(&texture->glyph_texcoord_buffer);
	vulkan_descriptor_set_write_uniform_buffer(vulkan_renderer_get_global_set(texture->renderer), VULKAN_DESCRIPTOR_BINDING_GTC_BUFFER, gtc_buffer);
}

SGE_API void vulkan_bitmap_glyph_atlas_texture_destroy(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_host_buffered_texture_destroy(BASE(texture));
	bitmap_glyph_pool_destroy(&texture->pool);
	/* destroy the GTC buffer */
	vulkan_host_buffered_buffer_destroy(&texture->glyph_texcoord_buffer);
	dictionary_clear(&texture->glyph_texcoord_index_table);
	event_destroy(texture->on_resize_event);
	event_destroy(texture->on_gtc_buffer_resize_event);
}

SGE_API void vulkan_bitmap_glyph_atlas_texture_release_resources(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	dictionary_free(&texture->glyph_texcoord_index_table);
	event_release_resources(texture->on_resize_event);
	event_release_resources(texture->on_gtc_buffer_resize_event);
	bitmap_glyph_pool_release_resources(&texture->pool);
	vulkan_host_buffered_buffer_release_resources(&texture->glyph_texcoord_buffer);
	vulkan_host_buffered_texture_release_resources(BASE(texture));
	if(VULKAN_OBJECT_IS_INTERNAL(texture))
		memory_allocator_dealloc(texture->renderer->allocator, texture);
}

SGE_API u32 vulkan_bitmap_glyph_atlas_texture_get_or_create_glyph(vulkan_bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode)
{
	glyph_instance_id_t id =
	{
		.unicode = unicode,
		.font_id = vulkan_bitmap_glyph_atlas_texture_get_font(texture)->id
	};
	/* if the BGA texture already contains the character texture coordinates then just return it. */
	if(vulkan_bitmap_glyph_atlas_texture_contains_texcoord(texture, unicode))
	{
		u32 index = U32_MAX;
		dictionary_get_value(&texture->glyph_texcoord_index_table, &id, &index);
		return index;
	}

	/* rasterize the glyph with code as 'unicode' and get the texture coordinate of it in the BGA texture */
	glyph_texcoord_t texcoord;
	vulkan_bitmap_glyph_atlas_texture_get_texcoord(texture, unicode, &texcoord);

	/* convert the glyph_texcoord_t to vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t */
	vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t glsl_texcoord =
	{
		.tltc = { texcoord.tltc.x, texcoord.tltc.y },
		.trtc = { texcoord.trtc.x, texcoord.trtc.y },
		.brtc = { texcoord.brtc.x, texcoord.brtc.y },
		.bltc = { texcoord.bltc.x, texcoord.bltc.y }
	};

	/* add the texture coordinate into the GTC Buffer */
	buffer_t* buffer = vulkan_host_buffered_buffer_get_host_buffer(&texture->glyph_texcoord_buffer);
	u32 index = buf_get_element_count(buffer);
	buf_push_n(buffer, &glsl_texcoord, SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T);

	/* add the index of the texture coordinate into GTI table for faster checking if the character has already rasterized */
	dictionary_add(&texture->glyph_texcoord_index_table, &id, &index);

	return index;
}

SGE_API vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t vulkan_bitmap_glyph_atlas_texture_get_texcoord_from_index(vulkan_bitmap_glyph_atlas_texture_t* texture, u32 index)
{
	AUTO host_buffer = vulkan_host_buffered_buffer_get_host_buffer_readonly(&texture->glyph_texcoord_buffer);
	_debug_assert__(index < buf_get_element_count(host_buffer));
	vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t texcoords = { };
	buf_get_at_s(host_buffer, index, &texcoords);
	return texcoords;
}

static void repopulate_updated_gtc_host_buffer(vulkan_bitmap_glyph_atlas_texture_t* texture)
{
	/* update gtc buffer as the texture coordinate space has been transformed because of the new texture size */
	buffer_t* gtc_buffer = vulkan_host_buffered_buffer_get_host_buffer(&texture->glyph_texcoord_buffer);
	u32 count = buf_get_element_count(gtc_buffer);
	_debug_assert__(count == dictionary_get_count(&texture->glyph_texcoord_index_table));
	for(u32 i = 0; i < count; i++)
	{
		/* get the index of the GTC of this ascii in the GTC buffer */
		u32 index;
		dictionary_get_value_at(&texture->glyph_texcoord_index_table, i, &index);
		glyph_instance_id_t key;
		dictionary_get_key_at(&texture->glyph_texcoord_index_table, i, &key);

		_debug_assert__(vulkan_bitmap_glyph_atlas_texture_contains_texcoord2(texture, key));

		/* update the GTC at the index */
		glyph_texcoord_t texcoord;
		bool result = vulkan_bitmap_glyph_atlas_texture_try_get_texcoord(texture, key, &texcoord);
		_debug_assert__(result);
		vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t glsl_texcoord =
		{
			.tltc = { texcoord.tltc.x, texcoord.tltc.y },
			.trtc = { texcoord.trtc.x, texcoord.trtc.y },
			.brtc = { texcoord.brtc.x, texcoord.brtc.y },
			.bltc = { texcoord.bltc.x, texcoord.bltc.y }
		};
		buf_set_at(gtc_buffer, index, &glsl_texcoord);
	}
}

SGE_API bool vulkan_bitmap_glyph_atlas_texture_commit(vulkan_bitmap_glyph_atlas_texture_t* texture, bool OUT is_texture_resized)
{
	bool is_resized = false;
	bool is_bga_texture_updated = vulkan_host_buffered_texture_commit(BASE(texture), &is_resized);
	if(is_resized)
	{
		if(is_texture_resized != NULL)
			OUT is_texture_resized = true;
		/* query all the relocated glyph texcoords (because of texture resize) and update them again on the host side */
		repopulate_updated_gtc_host_buffer(texture);

		/* make sure bitmap text feature has been requested by the client application */
		_debug_assert__(texture->renderer->is_bitmap_text);

		/* write the new VkBuffer/VkImageView references to the descriptor at binding VULKAN_DESCRIPTOR_BINDING_BGA_TEXTURE in the global set */
		AUTO bga_texture = vulkan_bitmap_glyph_atlas_texture_get_texture(texture);
		vulkan_descriptor_set_write_texture(vulkan_renderer_get_global_set(texture->renderer), VULKAN_DESCRIPTOR_BINDING_BGA_TEXTURE, bga_texture);
	
		event_publish(texture->on_resize_event);
	}

	/* update the device side GTC buffer */
	is_resized = false;
	bool is_gtc_buffer_updated = vulkan_host_buffered_buffer_commit(&texture->glyph_texcoord_buffer, &is_resized);

	/* if material is not NULL then update GTC buffer uniform interface block descriptor */
	if(is_resized)
	{
		/* make sure bitmap text feature has been requested by the client application */
		_debug_assert__(texture->renderer->is_bitmap_text);

		/* write the new VkBuffer reference to the descriptor at binding VULKAN_DESCRIPTOR_BINDING_GTC_BUFFER in the global set */
		AUTO gtc_buffer = vulkan_host_buffered_buffer_get_device_buffer(&texture->glyph_texcoord_buffer);
		vulkan_descriptor_set_write_uniform_buffer(vulkan_renderer_get_global_set(texture->renderer), VULKAN_DESCRIPTOR_BINDING_GTC_BUFFER, gtc_buffer);

		event_publish(texture->on_gtc_buffer_resize_event);
	}
	return is_gtc_buffer_updated || is_bga_texture_updated;
}

SGE_API bool vulkan_bitmap_glyph_atlas_texture_get_texcoord(vulkan_bitmap_glyph_atlas_texture_t* texture, pair_t(utf32_t, u32) unicode, glyph_texcoord_t OUT texcoord)
{
	return bitmap_glyph_pool_get_texcoord(&texture->pool, unicode, texcoord, NULL);
}

SGE_API bool vulkan_bitmap_glyph_atlas_texture_try_get_texcoord(vulkan_bitmap_glyph_atlas_texture_t* texture, glyph_instance_id_t id, glyph_texcoord_t OUT texcoord)
{
	return bitmap_glyph_pool_try_get_texcoord(&texture->pool, id, texcoord);
}