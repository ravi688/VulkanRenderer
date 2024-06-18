/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_bitmap_text.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_bitmap_text.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/system/display.h> // display_get_dpi()
#include <renderer/string.h> // strlen_safe
#include <renderer/string_builder.h>
#include <ctype.h> // isgraph

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator)
{
	vulkan_bitmap_text_t* text = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_BITMAP_TEXT, vulkan_bitmap_text_t);
	memzero(text, vulkan_bitmap_text_t);
	VULKAN_OBJECT_INIT(text, VULKAN_OBJECT_TYPE_BITMAP_TEXT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return text;
}

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info)
{
	vulkan_bitmap_text_t* text = vulkan_bitmap_text_new(renderer->allocator);
	vulkan_bitmap_text_create_no_alloc(renderer, create_info, text);
	return text;
}

static void update_bga_and_gtc_buffer(void* publisher, void* handler_data)
{
	AUTO text = VULKAN_BITMAP_TEXT(handler_data);

	#if DBG_ENABLED(BUFFER2D_RESIZE)
		AUTO builder = text->renderer->debug_log_builder;
		debug_log_info("BGA Resize, Rect mappings: %s\n", string_builder_get_str(builder));
	#endif /* DBG_BUFFER2D_RESIZE */

	#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
		string_builder_clear(builder);
	#endif /* DBG_VULKAN_BITMAP_TEXT_STRING_SETH */

	/* update bga texture */
	if(text->material != NULL)
	{
		vulkan_material_set_texture(text->material, "bga", VULKAN_TEXTURE(publisher));
		AUTO tex_size = vulkan_bitmap_glyph_atlas_texture_get_size(text->texture);
		vulkan_material_set_uvec2(text->material, "parameters.tex_size", (uvec2_t) { tex_size.width, tex_size.height });
	}

	/* update gtc buffer as the texture coordinate space has been transformed because of the new texture size */
	buffer_t* gtc_buffer = vulkan_host_buffered_buffer_get_host_buffer(&text->glyph_texcoord_buffer);
	u32 count = buf_get_element_count(gtc_buffer);
	_debug_assert__(count == dictionary_get_count(&text->glyph_texcoord_index_table));
	for(u32 i = 0; i < count; i++)
	{
		/* get the index of the GTC of this ascii in the GTC buffer */
		u32 index;
		dictionary_get_value_at(&text->glyph_texcoord_index_table, i, &index);
		pair_t(utf32_t, u32) key;
		dictionary_get_key_at(&text->glyph_texcoord_index_table, i, &key);

		_debug_assert__(vulkan_bitmap_glyph_atlas_texture_contains_texcoord(text->texture, key));

		/* update the GTC at the index */
		glyph_texcoord_t texcoord;
		vulkan_bitmap_glyph_atlas_texture_get_texcoord(text->texture, key, &texcoord);
		vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t glsl_texcoord =
		{
			.tltc = { texcoord.tltc.x, texcoord.tltc.y },
			.trtc = { texcoord.trtc.x, texcoord.trtc.y },
			.brtc = { texcoord.brtc.x, texcoord.brtc.y },
			.bltc = { texcoord.bltc.x, texcoord.bltc.y }
		};
		buf_set_at(gtc_buffer, index, &glsl_texcoord);
	}
	bool is_resized = false;
	_release_assert__(vulkan_host_buffered_buffer_commit(&text->glyph_texcoord_buffer, &is_resized));
	_debug_assert__(is_resized == false);
}

static bool default_glyph_layout_handler(vulkan_bitmap_text_glyph_layout_data_buffer_t* output, const vulkan_bitmap_text_glyph_layout_handler_input_data_t* input, void* user_data)
{
	u32 count = input->glyph_count;
	for(u32 i = 0; i < count; i++)
	{
		vulkan_bitmap_text_glyph_layout_data_t data =
		{
			.index = i,
			.offset = { 0, 0, 0 },
			.color = ICOLOR4_GREY,
			.is_bold = false,
			.is_italic = false,
			.is_strikethrough = false,
			.is_underline = false,
			.idoc = NULL
		};
		buf_push(output, &data);
	}
	return true;
}

RENDERER_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info, vulkan_bitmap_text_t OUT text)
{
	VULKAN_OBJECT_MEMZERO(text, vulkan_bitmap_text_t);
	text->renderer = renderer;

	text->text_strings = memory_allocator_buf_new(renderer->allocator, vulkan_bitmap_text_string_t);
	text->glyph_layout_data_buffer = memory_allocator_buf_create(renderer->allocator, sizeof(vulkan_bitmap_text_glyph_layout_data_t), 128, 0);
	text->glyph_layout_handler = (vulkan_bitmap_text_glyph_layout_handler_void_ptr_pair_t) { default_glyph_layout_handler, NULL };
	/* one might wonder free text->free_list should be 0 at the beginning?
	 * in this case no, because we don't even have the list of objects yet (whether in use or free) */
	text->free_list = BUF_INVALID_INDEX;
	text->inuse_list = BUF_INVALID_INDEX;
	text->render_space_type = VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_2D;
	text->render_surface_type = VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA;
	text->point_size = font_get_char_size(vulkan_bitmap_glyph_atlas_texture_get_font(create_info->texture));

	/* setup BGA texture */
	_debug_assert__(create_info->texture != NULL);
	text->glyph_atlas_texture = create_info->texture;

	/* subscribe to the on resize texture event */
	event_subscription_create_info_t subscription =
	{
		.handler = EVENT_HANDLER(update_bga_and_gtc_buffer),
		/* call the handler as soon as the event occurs */
		.wait_for = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_NOTHING_BIT,
		.handler_data = (void*)text
	};
	text->bga_texture_update_handle = event_subscribe(text->texture->on_resize_event, &subscription);

	/* create GRD buffer */
	vulkan_instance_buffer_create_info_t grd_buffer_create_info =
	{
		.stride = STRIDE_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T_ARRAY,
		/* initially we can store render information of upto 512 characters, so there will be no chance of buffer resize until 512 */
		.capacity = 512
	};
	VULKAN_OBJECT_INIT(&text->glyph_render_data_buffer, VULKAN_OBJECT_TYPE_INSTANCE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_instance_buffer_create(renderer, &grd_buffer_create_info, &text->glyph_render_data_buffer);

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
	VULKAN_OBJECT_INIT(&text->glyph_texcoord_buffer, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text->glyph_texcoord_buffer);

	// _debug_assert__(glsl_sizeof(_mat4_t) == sizeof(_mat4_t));

	/* create TST buffer to store string transform matrices */
	buffer_create_info = (vulkan_host_buffered_buffer_create_info_t)
	{
		.stride = sizeof(_mat4_t),
		/* initially we can have upto 2 string transform matrices without buffer resize */
		.capacity = 2,
		/* this buffer will be indexed with glyph_render_data_t.stid */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	VULKAN_OBJECT_INIT(&text->text_string_transform_buffer, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text->text_string_transform_buffer);

	text->glyph_texcoord_index_table = dictionary_create(pair_t(utf32_t, u32), u32, 127, utf32_u32_equal_to);

	/* setup the quad mesh */

	/* create vulkan_mesh_t object */

	/* per-vertex vertex buffer */
	vec4_t quad_vertices[] =
	{
		{ -0.5f,  0.5f, 0, 1.0f },
		{  0.5f,  0.5f, 0, 1.0f },
		{  0.5f, -0.5f, 0, 1.0f },
		{ -0.5f, -0.5f, 0, 1.0f }
	};
	vulkan_vertex_buffer_create_info_t vb_create_info =
	{
		.data = quad_vertices,
		.stride = sizeof(vec4_t),
		.count = SIZEOF_ARRAY(quad_vertices),
		.binding = 0
	};

	/* index buffer */
	u16 quad_indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	vulkan_mesh_create_info_t quad_create_info =
	{
		.vertex_buffer_infos = &vb_create_info,
		.vertex_buffer_info_count = 1,
		.index_buffer_info = { .data = quad_indices, .count = SIZEOF_ARRAY(quad_indices), .index_type = VK_INDEX_TYPE_UINT16 }
	};
	VULKAN_OBJECT_INIT(&text->quad_mesh, VULKAN_OBJECT_TYPE_MESH, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_mesh_create_no_alloc(renderer, &quad_create_info, &text->quad_mesh);

	/* add per-instance vertex buffer */
	vulkan_mesh_add_vertex_buffer(&text->quad_mesh, vulkan_instance_buffer_get_device_buffer(&text->glyph_render_data_buffer), 5);
}

RENDERER_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text)
{
	event_unsubscribe(text->texture->on_resize_event, text->bga_texture_update_handle);
	buf_clear(&text->glyph_layout_data_buffer, NULL);

	/* destroy text strings */
	u32 text_string_count = buf_get_element_count(&text->text_strings);
	for(u32 i = 0; i < text_string_count; i++)
		buf_free(&buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i)->chars);
	buf_clear(&text->text_strings, NULL);

	/* destroy the GRD buffer */
	vulkan_instance_buffer_destroy(&text->glyph_render_data_buffer);;

	/* destroy the TST buffer */
	vulkan_host_buffered_buffer_destroy(&text->text_string_transform_buffer);

	/* destroy the GTC buffer */
	vulkan_host_buffered_buffer_destroy(&text->glyph_texcoord_buffer);

	dictionary_clear(&text->glyph_texcoord_index_table);

	vulkan_mesh_destroy(&text->quad_mesh);
}

RENDERER_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text)
{
	dictionary_free(&text->glyph_texcoord_index_table);
	vulkan_mesh_release_resources(&text->quad_mesh);
	buf_free(&text->glyph_layout_data_buffer);
	buf_free(&text->text_strings);
	vulkan_instance_buffer_release_resources(&text->glyph_render_data_buffer);
	vulkan_host_buffered_buffer_release_resources(&text->text_string_transform_buffer);
	vulkan_host_buffered_buffer_release_resources(&text->glyph_texcoord_buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(text))
		memory_allocator_dealloc(text->renderer->allocator, text);
}

RENDERER_API void vulkan_bitmap_text_draw(vulkan_bitmap_text_t* text)
{
	AUTO count = vulkan_buffer_get_count(vulkan_instance_buffer_get_device_buffer(&text->glyph_render_data_buffer));
	if(count > 0)
		vulkan_mesh_draw_indexed_instanced(&text->quad_mesh, count);
}

static void set_render_space_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_space_type_t space_type)
{
	switch(space_type)
	{
		case VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_2D:
			vulkan_material_set_int(text->material, "parameters.space_type", 0);
			break;
		case VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_3D:
			vulkan_material_set_int(text->material, "parameters.space_type", 1);
			break;
		default:
			debug_log_error("Invalid vulkan bitmap text space type: %lu", space_type);
			break;
	}
}

static void set_render_surface_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_surface_type_t surface_type)
{
	switch(surface_type)
	{
		case VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA:
			vulkan_material_set_int(text->material, "parameters.surface_type", 1);
			break;
		case VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN:
			vulkan_material_set_int(text->material, "parameters.surface_type", 0);
			break;
		default:
			debug_log_error("Invalid vulkan bitmap text render surface type: %lu", surface_type);
			break;
	}
}

static _mat4_t get_glsl_mat4_from_mat4(const mat4_t* const mat)
{
	return (_mat4_t)
	{
		.r1 = { mat->r1[0], mat->r1[1], mat->r1[2], mat->r1[3] },
		.r2 = { mat->r2[0], mat->r2[1], mat->r2[2], mat->r2[3] },
		.r3 = { mat->r3[0], mat->r3[1], mat->r3[2], mat->r3[3] },
		.r4 = { mat->r4[0], mat->r4[1], mat->r4[2], mat->r4[3] }
	};
}

RENDERER_API vulkan_bitmap_text_string_handle_t vulkan_bitmap_text_string_create(vulkan_bitmap_text_t* text)
{
	vulkan_bitmap_text_string_handle_t handle;
	vulkan_bitmap_text_string_t* text_string = NULL;
	buffer_t* tst_buffer = vulkan_host_buffered_buffer_get_host_buffer(&text->text_string_transform_buffer);
	if(text->free_list != BUF_INVALID_INDEX)
	{
		handle = text->free_list;
		text_string = buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, text->free_list);

		/* remove 'handle' from the beginning of the free list */
		text->free_list = text_string->next;
	}
	else
	{
		/* create a new string */
		handle = buf_get_element_count(&text->text_strings);
		buf_push_pseudo(&text->text_strings, 1);
		text_string = buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, handle);
		text_string->chars = memory_allocator_buf_new(text->renderer->allocator, s8);
		buf_push_pseudo(tst_buffer, 1);
	}

	_debug_assert__(text_string != NULL);

	text_string->handle = handle;
	text_string->render_data_handle = multi_buffer_sub_buffer_create(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), 32);
	text_string->rect.offset = offset3d(0, 0, 0);
	text_string->rect.extent = extent2d(500, 300);
	text_string->transform = mat4_identity();
	text_string->point_size = text->point_size;

	/* add transform of this string to the TST buffer */
	mat4_t transform = mat4_transpose(text_string->transform); // though we don't need this transpose as the matrix will be identity here.
	_mat4_t glsl_transform = get_glsl_mat4_from_mat4(&transform);
	_debug_assert__(sizeof(buf_ucount_t) == sizeof(handle));
	buf_set_at(tst_buffer, handle, &glsl_transform);

	/* update the device side buffer */

	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text->text_string_transform_buffer, &is_resized);

	/* if material is not NULL then update material's TSTBuffer uniform interface block descriptor */
	if(text->material != NULL)
	{
		AUTO tst_device_buffer = vulkan_host_buffered_buffer_get_device_buffer(&text->text_string_transform_buffer);
		/* new string transform has been added, hence update the trasform count */
		if(is_updated)
			vulkan_material_set_array_size(text->material, "TSTBuffer.transforms", vulkan_buffer_get_count(tst_device_buffer));
		/* if the tst device buffer has been resized (meaning it is recreated) then rewrite the descriptors */
		if(is_resized)
			vulkan_material_set_buffer(text->material, "TSTBuffer", tst_device_buffer);
	}

	/* add 'handle to the beginning of the inuse list */
	text_string->next = text->inuse_list;
	text->inuse_list = handle;

	return handle;
}

RENDERER_API void vulkan_bitmap_text_string_destroyH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	vulkan_bitmap_text_string_list_t* text_strings = &text->text_strings;

	/* find the string with handle 'handle' in the inuse list */
	vulkan_bitmap_text_string_t* prev_text_string = NULL;
	vulkan_bitmap_text_string_handle_t _handle = text->inuse_list;
	while((_handle != handle) && (_handle != BUF_INVALID_INDEX))
	{
		prev_text_string = buf_get_ptr_at_typeof(text_strings, vulkan_bitmap_text_string_t, _handle);
		_handle = prev_text_string->next;
	}

	/* fails if the string with handle 'handle doesn't exists */
	_debug_assert__(_handle != BUF_INVALID_INDEX);

	AUTO text_string = buf_get_ptr_at_typeof(text_strings, vulkan_bitmap_text_string_t, handle);

	/* reset this string to be reused later */
	multi_buffer_sub_buffer_destroy(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), text_string->render_data_handle);
	text_string->render_data_handle = SUB_BUFFER_HANDLE_INVALID;
	buf_clear(&text_string->chars, NULL);
	text_string->rect.offset = offset3d(0.0f, 0.0f, 0.0f);
	text_string->rect.extent = extent2d(500, 300);
	text_string->transform = mat4_identity();

	/* remove from the inuse list */
	if(prev_text_string != NULL)
		prev_text_string->next = text_string->next;
	else
		text->inuse_list = BUF_INVALID_INDEX;

	/* add to the free list */
	text_string->next = text->free_list;
	text->free_list = handle;
}

static u32 get_or_create_glyph_texture_coordinate(vulkan_bitmap_text_t* text, pair_t(utf32_t, u32) unicode)
{
	/* if the BGA texture already contains the character texture coordinates then just return it. */
	if(vulkan_bitmap_glyph_atlas_texture_contains_texcoord(text->texture, unicode))
	{
		u32 index = U32_MAX;
		dictionary_get_value(&text->glyph_texcoord_index_table, &unicode, &index);
		return index;
	}

	/* rasterize the glyph with code as 'unicode' and get the texture coordinate of it in the BGA texture */
	glyph_texcoord_t texcoord;
	vulkan_bitmap_glyph_atlas_texture_get_texcoord(text->texture, unicode, &texcoord);

	/* convert the glyph_texcoord_t to vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t */
	vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t glsl_texcoord =
	{
		.tltc = { texcoord.tltc.x, texcoord.tltc.y },
		.trtc = { texcoord.trtc.x, texcoord.trtc.y },
		.brtc = { texcoord.brtc.x, texcoord.brtc.y },
		.bltc = { texcoord.bltc.x, texcoord.bltc.y }
	};

	/* add the texture coordinate into the GTC Buffer */
	buffer_t* buffer = vulkan_host_buffered_buffer_get_host_buffer(&text->glyph_texcoord_buffer);
	u32 index = buf_get_element_count(buffer);
	buf_push_n(buffer, &glsl_texcoord, SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T);

	/* add the index of the texture coordinate into GTI table for faster checking if the character has already rasterized */
	dictionary_add(&text->glyph_texcoord_index_table, &unicode, &index);

	return index;
}

/* setters */
static void text_string_set(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, const char* data)
{
	/* clear the render data for this text string */
	multi_buffer_sub_buffer_clear(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), text_string->render_data_handle);

	font_t* font = vulkan_bitmap_text_get_font(text);

	/* override the point size for this font */
	u32 saved_point_size = font_get_char_size(font);
	font_set_char_size(font, text_string->point_size);

	/* build the precusor glyph layout data for each character requested */
	/* offset added to y coordinate, to adjust the anchor of the text to the top-left corner */
	f32 anchor_offset_y = get_pixels_from_point_size((font_get_ascender(font) / font_get_units_per_em(font)) * font_get_char_size(font), display_get_dpi().y);

	u32 len = strlen_safe(data);

	vulkan_bitmap_text_glyph_info_t glyph_infos[len];
	u32 texcoord_indices[len];

	#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
		string_builder_t* builder = string_builder_create(text->renderer->allocator, 1024);
		string_builder_clear(builder);
		string_builder_append(builder, "TextString: %s, size: %u, texcoord list:\n{\n", data, text_string->point_size);
		string_builder_increment_indentation(builder);
	#endif /* DBG_VULKAN_BITMAP_TEXT_STRING_SETH */

	f32 horizontal_pen = 0;
	iextent2d_t tex_size = vulkan_bitmap_glyph_atlas_texture_get_size(text->texture);
	for(u32 i = 0; i < len; i++)
	{
		font_glyph_info_t info;
		utf32_t ch = data[i];
		font_get_glyph_info2(font, ch, &info);

		vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t texcoords;
		if(info.is_graph)
		{
			texcoord_indices[i] = get_or_create_glyph_texture_coordinate(text, make_pair(utf32_t, u32) { ch, text_string->point_size });
			buf_get_at_s(vulkan_host_buffered_buffer_get_host_buffer(&text->glyph_texcoord_buffer), texcoord_indices[i], &texcoords);
		}
		else
		{
			texcoord_indices[i] = U32_MAX;
			f32 xcoord = info.width / tex_size.width; // get_pixels_from_point_size(info.width, display_get_dpi().x) / tex_size.width;
			f32 ycoord = info.height / tex_size.height; // get_pixels_from_point_size(info.height, display_get_dpi().y) / tex_size.height;
			texcoords = (vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t)
			{
				/* tltc */
				{ 0.0f, 0.0f },
				/* trtc */
				{ xcoord, 0.0f },
				/* brtc */
				{ 0.0f, ycoord },
				/* bltc */
				{ xcoord, ycoord }
			};
		}

		glyph_infos[i] = (vulkan_bitmap_text_glyph_info_t)
		{
			.unicode = ch,
			.rect = { offset3d(horizontal_pen + info.width * 0.5f + info.bearing_x,
							   info.height * 0.5f + (info.bearing_y - info.height) - anchor_offset_y,
							   0.0f),
					  extent3d((texcoords.trtc.x - texcoords.tltc.x) * tex_size.width,
					  		   (texcoords.bltc.y - texcoords.tltc.y) * tex_size.height,
							   0.0f)
					}
		};
		horizontal_pen += info.advance_width;

		#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
			buffer_t* buffer = vulkan_host_buffered_buffer_get_host_buffer(&text->glyph_texcoord_buffer);
			AUTO texcoord = buf_get_ptr_at_typeof(buffer, vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t, data.indx);
			string_builder_append(builder, "%c =\n", CAST_TO(char, ch));
			string_builder_append(builder, "{\n");
			string_builder_increment_indentation(builder);
				string_builder_append(builder, "|(%.3f, %.3f), (%.3f, %.3f)|\n", texcoord->tltc.x, texcoord->tltc.y, texcoord->trtc.x, texcoord->trtc.y);
				string_builder_append(builder, "|(%.3f, %.3f), (%.3f, %.3f)}\n", texcoord->bltc.x, texcoord->bltc.y, texcoord->brtc.x, texcoord->brtc.y);
			string_builder_decrement_indentation(builder);
			string_builder_append(builder, "}\n");
		#endif /* DBG_VULKAN_BITMAP_TEXT_STRING_SETH */
	}

	#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
		string_builder_decrement_indentation(builder);
		string_builder_append(builder, "}\n");
		string_builder_append_null(builder);
		debug_log_info("%s", string_builder_get_str(builder));
		string_builder_destroy(builder);
	#endif /* DBG_VULKAN_BITMAP_TEXT_STRING_SETH */

	/* restore the point size for this font */
	font_set_char_size(font, saved_point_size);

	/* call the layout handler */

	_debug_assert__(text->glyph_layout_handler.first != NULL);
	const vulkan_bitmap_text_glyph_layout_handler_input_data_t input_data =
	{
		.glyphs = glyph_infos,
		.glyph_count = len, /* whitespaces are also included */
		.anchor_pos = mat4_get_position(text_string->transform).xyz /* vec4 -> vec3 */
	};
	/* NOTE: no need to clear the glyph_layout_data_buffer as it is cleared after every set call */
	bool is_changed = (text->glyph_layout_handler.first)(&text->glyph_layout_data_buffer, &input_data, text->glyph_layout_handler.second);


	/* rewrite on the sub buffers */

	/* final (post-processed) glyph counts */
	u32 final_count = is_changed ? buf_get_element_count(&text->glyph_layout_data_buffer) : len;

	for(u32 i = 0; i < final_count; i++)
	{
		AUTO glyph_data = is_changed ? buf_get_ptr_at_typeof(&text->glyph_layout_data_buffer, vulkan_bitmap_text_glyph_layout_data_t, i) : NULL;
		u32 index = is_changed ? glyph_data->index : i;

		/* skip the whitespaces (for which we don't have any physical texture coord information) */
		if(texcoord_indices[index] == U32_MAX)
			continue;

		AUTO offset = vec3_add(2, glyph_infos[index].rect.offset, (is_changed ? glyph_data->offset : vec3_zero()));
		// _debug_assert__(info.bitmap_top == info.bearing_y);
		vulkan_bitmap_text_glsl_glyph_render_data_t data =
		{
			.ofst = { offset.x, offset.y, offset.z },
			.indx = texcoord_indices[index],
			/* TODO: Remove this as we don't need this level of flexibility */
			.rotn = { ((i%3) == 0) ? 1 : 0, ((i%3) == 1) ? 1 : 0, ((i%3) == 2) ? 1 : 0 },
			.stid = U64_TO_U32(text_string->handle),
			/* TODO: Remove this as we don't need this level of flexibility */
			.scal = { 1, 1, 1 }
		};

		multi_buffer_sub_buffer_push_n(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), 
										text_string->render_data_handle, 
										(void*)&data,
										SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T);
	}

	/* clear the glyph layout data buffer for the next set call */
	buf_clear(&text->glyph_layout_data_buffer, NULL);

	vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);

	/* update the device side GTC buffer */
	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text->glyph_texcoord_buffer, &is_resized);

	/* if material is not NULL then update GTC buffer uniform interface block descriptor */
	if(text->material != NULL)
	{
		AUTO gtc_device_buffer = vulkan_host_buffered_buffer_get_device_buffer(&text->glyph_texcoord_buffer);
		/* if the new glyphs has been added into the GTC buffer then update the GTC size in the material */
		if(is_updated)
			vulkan_material_set_array_size(text->material, "GTCBuffer.texcoords", vulkan_buffer_get_count(gtc_device_buffer));
		/* if the GTC device buffer has been resized (meaning it is recreated), then rewrite descriptors */
		if(is_resized)
			vulkan_material_set_buffer(text->material, "GTCBuffer", gtc_device_buffer);
	}
}

static void text_string_set_point_size(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, u32 point_size)
{
	text_string->point_size = point_size;
	text_string_set(text, text_string, CAST_TO(const char*, buf_get_ptr(&text_string->chars)));
}

RENDERER_API void vulkan_bitmap_text_set_glyph_layout_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_layout_handler_t handler, void* user_data)
{
	text->glyph_layout_handler = (vulkan_bitmap_text_glyph_layout_handler_void_ptr_pair_t) { handler, user_data };
}

RENDERER_API void vulkan_bitmap_text_set_glyph_strikethrough_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_strikethrough_handler_t handler, void* user_data)
{
	text->glyph_strikethrough_handler = (vulkan_bitmap_text_glyph_strikethrough_handler_void_ptr_pair_t) { handler, user_data };
}

RENDERER_API void vulkan_bitmap_text_set_glyph_underline_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_underline_handler_t handler, void* user_data)
{
	text->glyph_underline_handler = (vulkan_bitmap_text_glyph_underline_handler_void_ptr_pair_t) { handler, user_data };
}

RENDERER_API void vulkan_bitmap_text_set_point_size(vulkan_bitmap_text_t* text, u32 point_size)
{
	text->point_size = point_size;
	buf_ucount_t count = buf_get_element_count(&text->text_strings);
	for(buf_ucount_t i = 0; i < count; i++)
		text_string_set_point_size(text, buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i), point_size);
}

RENDERER_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material)
{
	text->material = material;

	set_render_space_type(text, text->render_space_type);
	set_render_surface_type(text, text->render_surface_type);

	vulkan_material_set_texture(material, "bga", VULKAN_TEXTURE(text->texture));
	AUTO tex_size = vulkan_bitmap_glyph_atlas_texture_get_size(text->texture);
	vulkan_material_set_uvec2(material, "parameters.tex_size", (uvec2_t) { tex_size.width, tex_size.height });

	vulkan_material_set_array_size(material, "GTCBuffer.texcoords", buf_get_element_count(vulkan_host_buffered_buffer_get_host_buffer(&text->glyph_texcoord_buffer)));
	vulkan_material_set_array_size(material, "TSTBuffer.transforms", buf_get_element_count(vulkan_host_buffered_buffer_get_host_buffer(&text->text_string_transform_buffer)));

	vulkan_material_set_buffer(material, "GTCBuffer", vulkan_host_buffered_buffer_get_device_buffer(&text->glyph_texcoord_buffer));
	vulkan_material_set_buffer(material, "TSTBuffer", vulkan_host_buffered_buffer_get_device_buffer(&text->text_string_transform_buffer));
}

RENDERER_API void vulkan_bitmap_text_set_render_space_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_space_type_t space_type)
{
	if(text->render_space_type == space_type)
		return;
	text->render_space_type = space_type;

	/* if there is no material assigned then do nothing, and save the data (as above) to later apply the changes (whenver the material becomes available) */
	if(text->material == NULL)
		return;

	set_render_space_type(text, space_type);
}

RENDERER_API void vulkan_bitmap_text_set_render_surface_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_surface_type_t surface_type)
{
	if(text->render_surface_type == surface_type)
		return;
	text->render_surface_type = surface_type;

	/* if there is no material assigned then do nothing, and save the data (as above) to later apply the changes */
	if(text->material == NULL)
		return;

	set_render_surface_type(text, surface_type);
}

static vulkan_bitmap_text_string_t* get_text_stringH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, handle);
}

RENDERER_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, const char* string)
{
	vulkan_bitmap_text_string_t* text_string = get_text_stringH(text, handle);

	/* clear the previous string characters */
	buf_clear(&text_string->chars, NULL);
	/* write the new characters */
	buf_push_string(&text_string->chars, string);
	/* null character at the end of the string, as we are using strlen inside text_string_set */
	buf_push_null(&text_string->chars);

	text_string_set(text, text_string, string);
}

RENDERER_API void vulkan_bitmap_text_string_set_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 point_size)
{
	text_string_set_point_size(text, get_text_stringH(text, handle), point_size);

	/* check if all the text strings have the same point size as 'point_size' */
	bool is_same = true;
	buf_ucount_t count = buf_get_element_count(&text->text_strings);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		if(buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i)->point_size != point_size)
		{
			is_same = false;
			break;
		}
	}

	/* if yes, then update the bitmap_text's point size */
	if(is_same)
	{
		text->point_size = point_size;
		/* Memory optimization:
		 * we may also prefer deleting the earlier rasterized glyphs which were rasterized with different point
		 * size values (not equal to the current 'point_size') from teh BGA texture to make space for more glyphs. */
	}
}

RENDERER_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, mat4_t transform)
{
	mat4_move(get_text_stringH(text, handle)->transform, transform);
	mat4_move(transform, mat4_transpose(transform));
	_mat4_t glsl_transform = get_glsl_mat4_from_mat4(&transform);
	buf_set_at(vulkan_host_buffered_buffer_get_host_buffer(&text->text_string_transform_buffer), handle, &glsl_transform);
	bool is_resized = false;
	vulkan_host_buffered_buffer_commit(&text->text_string_transform_buffer, &is_resized);
	/* we are just updating the data, so no resize can be expected */
	_debug_assert__(is_resized == false);
}

/* getters */
RENDERER_API u32 vulkan_bitmap_text_get_point_size(vulkan_bitmap_text_t* text)
{
	return text->point_size;
}

RENDERER_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return CAST_TO(const char*, buf_get_ptr(&get_text_stringH(text, handle)->chars));
}

RENDERER_API u32 vulkan_bitmap_text_string_get_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->point_size;
}

RENDERER_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->transform;
}

RENDERER_API font_t* vulkan_bitmap_text_get_font(vulkan_bitmap_text_t* text)
{
	_debug_assert__(text->texture != NULL);
	return vulkan_bitmap_glyph_atlas_texture_get_font(text->texture);
}