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

#include <sge/internal/vulkan/vulkan_bitmap_text.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <sge/internal/vulkan/vulkan_material.h>
#include <sge/render_window.h>
#include <sge/memory_allocator.h>
#include <sge/system/display.h> // display_get_dpi()
#include <sge/string.h> // strlen_safe
#include <sge/string_builder.h>
#include <ctype.h> // isgraph

#define NUM_SPACES_PER_TAB 4

SGE_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator)
{
	vulkan_bitmap_text_t* text = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_BITMAP_TEXT, vulkan_bitmap_text_t);
	memzero(text, vulkan_bitmap_text_t);
	VULKAN_OBJECT_INIT(text, VULKAN_OBJECT_TYPE_BITMAP_TEXT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return text;
}

SGE_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info)
{
	vulkan_bitmap_text_t* text = vulkan_bitmap_text_new(renderer->allocator);
	vulkan_bitmap_text_create_no_alloc(renderer, create_info, text);
	return text;
}

static void on_bga_texture_update(void* publisher, void* handler_data)
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
		// vulkan_material_set_texture(text->material, "bga", VULKAN_TEXTURE(publisher));
		AUTO tex_size = vulkan_bitmap_glyph_atlas_texture_get_size(text->texture);
		vulkan_material_set_uvec2(text->material, "parameters.tex_size", (uvec2_t) { tex_size.width, tex_size.height });
	}
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

SGE_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info, vulkan_bitmap_text_t OUT text)
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
	text->font = create_info->font ? create_info->font : vulkan_bitmap_glyph_atlas_texture_get_font(create_info->texture);
	if(!text->font)
		debug_log_warning("You haven't assigned 'font' field a valid pointer to font_t object in vulkan_bitmap_text_create_info_t, and bga texture doesn't contain valid font pointer either");
	text->point_size = font_get_char_size(text->font);

	/* setup BGA texture */
	_debug_assert__(create_info->texture != NULL);
	text->glyph_atlas_texture = create_info->texture;

	/* subscribe to the on resize texture event */
	event_subscription_create_info_t subscription =
	{
		.handler = EVENT_HANDLER(on_bga_texture_update),
		/* call the handler as soon as the event occurs */
		.wait_for = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_NOTHING_BIT,
		.handler_data = (void*)text
	};
	text->bga_texture_update_handle = event_subscribe(vulkan_bitmap_glyph_atlas_texture_get_on_resize_event(text->texture), &subscription);

	/* create GRD buffer */
	vulkan_instance_buffer_create_info_t grd_buffer_create_info =
	{
		.stride = STRIDE_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T_ARRAY,
		/* initially we can store render information of upto 512 characters, so there will be no chance of buffer resize until 512 */
		.capacity = 512
	};
	VULKAN_OBJECT_INIT(&text->glyph_render_data_buffer, VULKAN_OBJECT_TYPE_INSTANCE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_instance_buffer_create(renderer, &grd_buffer_create_info, &text->glyph_render_data_buffer);

	// _debug_assert__(glsl_sizeof(_mat4_t) == sizeof(_mat4_t));

	/* create TST buffer to store string transform matrices */
	vulkan_host_buffered_buffer_create_info_t buffer_create_info =
	{
		.stride = sizeof(_mat4_t),
		/* initially we can have upto 2 string transform matrices without buffer resize */
		.capacity = 2,
		/* this buffer will be indexed with glyph_render_data_t.stid */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	VULKAN_OBJECT_INIT(&text->text_string_transform_buffer, VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text->text_string_transform_buffer);

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

SGE_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text)
{
	event_unsubscribe(text->texture->on_resize_event, text->bga_texture_update_handle);
	buf_clear(&text->glyph_layout_data_buffer, NULL);

	/* destroy text strings */
	u32 text_string_count = buf_get_element_count(&text->text_strings);
	for(u32 i = 0; i < text_string_count; i++)
	{
		AUTO text_string = buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i);
		buf_free(&text_string->chars);
		buf_free(&text_string->glyph_offsets);
		buf_free(&text_string->index_mappings);
		buf_free(&text_string->color_ranges);
	}
	buf_clear(&text->text_strings, NULL);

	/* destroy the GRD buffer */
	vulkan_instance_buffer_destroy(&text->glyph_render_data_buffer);;

	/* destroy the TST buffer */
	vulkan_host_buffered_buffer_destroy(&text->text_string_transform_buffer);

	vulkan_mesh_destroy(&text->quad_mesh);
}

SGE_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text)
{
	vulkan_mesh_release_resources(&text->quad_mesh);
	buf_free(&text->glyph_layout_data_buffer);
	buf_free(&text->text_strings);
	vulkan_instance_buffer_release_resources(&text->glyph_render_data_buffer);
	vulkan_host_buffered_buffer_release_resources(&text->text_string_transform_buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(text))
		memory_allocator_dealloc(text->renderer->allocator, text);
}

SGE_API void vulkan_bitmap_text_draw(vulkan_bitmap_text_t* text)
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

SGE_API vulkan_bitmap_text_string_handle_t vulkan_bitmap_text_string_create(vulkan_bitmap_text_t* text)
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
		/* vulkan_bitmap_text_string_get_lengthH function returns text_string->chars's element count - 1
		 * therefore, the number of characters in 'chars' must be greater than or equal to 1 (including the null character) */
		buf_push_null(&text_string->chars);
		text_string->glyph_offsets = memory_allocator_buf_new(text->renderer->allocator, f32);
		text_string->index_mappings = memory_allocator_buf_new(text->renderer->allocator, u32);
		text_string->color_ranges = memory_allocator_buf_new(text->renderer->allocator, char_attr_color_range_t);
		text_string->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		text_string->is_active = true;
		buf_push_pseudo(tst_buffer, 1);
	}

	_debug_assert__(text_string != NULL);

	/* set the default values if created for the first time or recycled. */
	text_string->handle = handle;
	text_string->render_data_handle = multi_buffer_sub_buffer_create(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), 32);
	text_string->rect.offset = offset3d(0, 0, 0);
	text_string->rect.extent = extent2d(500, 300);
	text_string->transform = mat4_identity();
	text_string->point_size = text->point_size;
	text_string->font = text->font;

	/* add transform of this string to the TST buffer */
	mat4_t transform = mat4_transpose(text_string->transform); // though we don't need this transpose as the matrix will be identity here.
	_mat4_t glsl_transform = get_glsl_mat4_from_mat4(&transform);
	_debug_assert__(sizeof(buf_ucount_t) == sizeof(handle));
	buf_set_at(tst_buffer, handle, &glsl_transform);

	/* update the device side buffer of the text string transform buffer (tst buffer) */

	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text->text_string_transform_buffer, &is_resized);

	/* if material is not NULL then update material's TSTBuffer uniform interface block descriptor */
	if(text->material != NULL)
	{
		AUTO tst_device_buffer = vulkan_host_buffered_buffer_get_device_buffer(&text->text_string_transform_buffer);
		/* new string transform has been added, hence update the trasform count */
		// if(is_updated)
			// vulkan_material_set_array_size(text->material, "TSTBuffer.transforms", vulkan_buffer_get_count(tst_device_buffer));
		/* if the tst device buffer has been resized (meaning it is recreated) then rewrite the descriptors */
		if(is_resized)
			vulkan_material_set_buffer(text->material, "TSTBuffer", tst_device_buffer);
	}

	/* add 'handle to the beginning of the inuse list */
	text_string->next = text->inuse_list;
	text->inuse_list = handle;

	return handle;
}

SGE_API void vulkan_bitmap_text_string_destroyH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	if(text->inuse_list == BUF_INVALID_INDEX)
	{
		debug_log_error("Invalid operation (text string destroy), bitmap text's inuse linked list is empty");
		return;
	}

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
	debug_assert__(_handle != BUF_INVALID_INDEX, "You're trying to destroy a bitmap text string which doesn't even exist in bitmap text");

	AUTO text_string = buf_get_ptr_at_typeof(text_strings, vulkan_bitmap_text_string_t, handle);

	/* reset this string to be reused later */
	multi_buffer_sub_buffer_destroy(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), text_string->render_data_handle);
	text_string->render_data_handle = SUB_BUFFER_HANDLE_INVALID;
	buf_clear(&text_string->chars, NULL);
	buf_clear(&text_string->glyph_offsets, NULL);
	buf_clear(&text_string->index_mappings, NULL);
	buf_clear(&text_string->color_ranges, NULL);
	text_string->rect.offset = offset3d(0.0f, 0.0f, 0.0f);
	text_string->rect.extent = extent2d(500, 300);
	text_string->transform = mat4_identity();

	/* remove from the inuse list */
	if(prev_text_string != NULL)
		prev_text_string->next = text_string->next;
	else
		text->inuse_list = text_string->next;

	/* add to the free list */
	text_string->next = text->free_list;
	text->free_list = handle;

	/* update GPU (device) side buffer of the glyph render data buffer if it was marked dirty */
	vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);
}

/* setters */
static void text_string_set(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, const char* data)
{
	/* clear the render data for this text string */
	multi_buffer_sub_buffer_clear(vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer), text_string->render_data_handle);
	/* clear the previous glyph offsets for this text string */
	/* TODO: it should be buf_clear_fast() */
	buf_clear(&text_string->glyph_offsets, NULL);
	/* TODO: it should be buf_clear_fast() */
	buf_clear(&text_string->index_mappings, NULL);

	/* if this text string is not active then no need to render characters of this text string. */
	if(!text_string->is_active)
	{
		f32 horizontal_pen = 0;
		// glyph_offset for the pseudo (virtual) glyph at the end of a line
		buf_push(&text_string->glyph_offsets, &horizontal_pen);
		/* update GPU (device) side buffer if it was marked dirty */
		vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);
		return;
	}

	font_t* font = text_string->font;

	/* override the point size for this font */
	u32 saved_point_size = font_get_char_size(font);
	font_set_char_size(font, text_string->point_size);

	/* build the precusor glyph layout data for each character requested */
	/* offset added to y coordinate, to adjust the anchor of the text to the top-left corner */
	f32 anchor_offset_y = font_get_font_units_to_pixels(font, font_get_ascender(font), display_get_dpi().y);

	u32 len = strlen_safe(data);

	vulkan_bitmap_text_glyph_info_t glyph_infos[len];
	u32 texcoord_indices[len];

	#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
		string_builder_t* builder = string_builder_create(text->renderer->allocator, 1024);
		string_builder_clear(builder);
		string_builder_append(builder, "TextString: %s, size: %u, texcoord list:\n{\n", data, text_string->point_size);
		string_builder_increment_indentation(builder);
	#endif /* DBG_VULKAN_BITMAP_TEXT_STRING_SETH */

	/* backup the current font used by bga texture so that we could restore it later */
	font_t* prev_font = vulkan_bitmap_glyph_atlas_texture_get_font(text->texture);
	/* now we are safe to modify the font used by bga texture. */
	vulkan_bitmap_glyph_atlas_texture_set_font(text->texture, font);

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
			/* NOTE: calling vulkan_bitmap_glyph_atlas_texture_get_or_create_glyph() may or may not mark the device side memory/buffer dirty (oudated) */
			texcoord_indices[i] = vulkan_bitmap_glyph_atlas_texture_get_or_create_glyph(text->texture, make_pair_t(utf32_t, u32) { ch, text_string->point_size });
			texcoords = vulkan_bitmap_glyph_atlas_texture_get_texcoord_from_index(text->texture, texcoord_indices[i]);
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

		/* offset of the left side of current glyph */
		f32 ls_offset = horizontal_pen + info.bearing_x;

		glyph_infos[i] = (vulkan_bitmap_text_glyph_info_t)
		{
			.unicode = ch,
			.rect = { offset3d(ls_offset + info.width * 0.5f,
							   info.height * 0.5f + (info.bearing_y - info.height) - anchor_offset_y,
							   0.0f),
					  extent3d((texcoords.trtc.x - texcoords.tltc.x) * tex_size.width,
					  		   (texcoords.bltc.y - texcoords.tltc.y) * tex_size.height,
							   0.0f)
					}
		};

		buf_push(&text_string->glyph_offsets, &ls_offset);

		/* for some reason the free type library always outputs advance_width as if the character is ' ' (space), even for '\t' (tab).
		 * therefore, we can just multiply the advance_width by 4 (NUM_SPACES_PER_TAB) to achieve the same thing.
		 */
		if(ch == '\t')
			info.advance_width *= NUM_SPACES_PER_TAB;

		horizontal_pen += info.advance_width;

		#if DBG_ENABLED(VULKAN_BITMAP_TEXT_STRING_SETH)
			buffer_t* buffer = vulkan_bitmap_glyph_atlas_texture_get_gtc_host_buffer_readonly(text->texture);
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

	/* restore the font earlier used by bga texture */
	vulkan_bitmap_glyph_atlas_texture_set_font(text->texture, prev_font);

	// glyph_offset for the pseudo (virtual) glyph at the end of a line
	buf_push(&text_string->glyph_offsets, &horizontal_pen);

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

	multi_buffer_t* grd_host_buffer = vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer);

	for(u32 i = 0; i < final_count; i++)
	{
		AUTO glyph_data = is_changed ? buf_get_ptr_at_typeof(&text->glyph_layout_data_buffer, vulkan_bitmap_text_glyph_layout_data_t, i) : NULL;
		u32 index = is_changed ? glyph_data->index : i;

		AUTO offset = vec3_add(2, glyph_infos[index].rect.offset, (is_changed ? glyph_data->offset : vec3_zero()));
	
		f32 ls_offset = 0;
		buf_get_at(&text_string->glyph_offsets, i, &ls_offset);
		ls_offset += (is_changed ? glyph_data->offset.x : 0);
		buf_set_at(&text_string->glyph_offsets, i, &ls_offset);

		/* skip the whitespaces (for which we don't have any physical texture coord information) */
		if(texcoord_indices[index] == U32_MAX)
		{
			u32 pad_value = U32_MAX;
			buf_push(&text_string->index_mappings, &pad_value);
			continue;
		}

		// _debug_assert__(info.bitmap_top == info.bearing_y);
		vulkan_bitmap_text_glsl_glyph_render_data_t data =
		{
			.ofst = { offset.x, offset.y, offset.z },
			.indx = texcoord_indices[index],
			/* TODO: Remove this as we don't need this level of flexibility */
			.rotn = { ((i%3) == 0) ? 1 : 0, ((i%3) == 1) ? 1 : 0, ((i%3) == 2) ? 1 : 0 },
			.stid = U64_TO_U32(text_string->handle),
			/* TODO: use exisiting color_ranges (if any) to assign color here, otherwise use the default text_string->color
			 * How?
			 * 		you have the index of this character into text_string->chars array
			 * 		use that index to find the color_range which contains it, and get the color of that color_range. */
			.colr = text_string->color
		};

		u32 mapped_index = sub_buffer_get_count(grd_host_buffer, text_string->render_data_handle);
		buf_push(&text_string->index_mappings, &mapped_index);

		multi_buffer_sub_buffer_push_n(grd_host_buffer, 
										text_string->render_data_handle, 
										(void*)&data,
										SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T);
	}

	/* clear the glyph layout data buffer for the next set call */
	buf_clear(&text->glyph_layout_data_buffer, NULL);

	/* update GPU (device) side buffer if it was marked dirty */
	vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);

	/* update the GPU (device) side buffers if they were marked dirty */
	// vulkan_bitmap_glyph_atlas_texture_commit(text->texture, NULL);
}

static void text_string_set_char_attr_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, const char_attr_color_range_t* ranges, u32 range_count);

static void text_string_refresh(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string)
{
	text_string_set(text, text_string, CAST_TO(const char*, buf_get_ptr(&text_string->chars)));

	// reapply the color ranges as the above call to text_string_set() uses the default color set via vulkan_bitmap_text_string_set_color()
	u32 range_count = buf_get_element_count(&text_string->color_ranges);
	if(range_count > 0)
		text_string_set_char_attr_color(text, text_string, buf_get_ptr_at_typeof(&text_string->color_ranges, char_attr_color_range_t, 0), range_count);
}

static void text_string_set_point_size(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, u32 point_size)
{
	text_string->point_size = point_size;
	if(buf_get_element_count(&text_string->chars) > 0)
		text_string_refresh(text, text_string);
}

SGE_API void vulkan_bitmap_text_set_glyph_layout_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_layout_handler_t handler, void* user_data)
{
	text->glyph_layout_handler = (vulkan_bitmap_text_glyph_layout_handler_void_ptr_pair_t) { handler, user_data };
}

SGE_API void vulkan_bitmap_text_set_glyph_strikethrough_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_strikethrough_handler_t handler, void* user_data)
{
	text->glyph_strikethrough_handler = (vulkan_bitmap_text_glyph_strikethrough_handler_void_ptr_pair_t) { handler, user_data };
}

SGE_API void vulkan_bitmap_text_set_glyph_underline_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_underline_handler_t handler, void* user_data)
{
	text->glyph_underline_handler = (vulkan_bitmap_text_glyph_underline_handler_void_ptr_pair_t) { handler, user_data };
}

SGE_API void vulkan_bitmap_text_set_point_size(vulkan_bitmap_text_t* text, u32 point_size)
{
	text->point_size = point_size;
	buf_ucount_t count = buf_get_element_count(&text->text_strings);
	for(buf_ucount_t i = 0; i < count; i++)
		text_string_set_point_size(text, buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i), point_size);
}

SGE_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material)
{
	text->material = material;

	set_render_space_type(text, text->render_space_type);
	set_render_surface_type(text, text->render_surface_type);

	// vulkan_material_set_texture(material, "bga", VULKAN_TEXTURE(text->texture));
	AUTO tex_size = vulkan_bitmap_glyph_atlas_texture_get_size(text->texture);
	vulkan_material_set_uvec2(material, "parameters.tex_size", (uvec2_t) { tex_size.width, tex_size.height });

	/* NOTE: There is no need to set the array size to non-zero 
	* 		because we do not create the VkBuffer object on the material side, and we also do not use the size
	* 		information of the buffer on the material side or anywhere else, anyway.
	*		The sole purpose of declaring the TSTBuffer array in the .v3dshader file is that it automatically
	* 		creates a descriptor for the buffer and we can just write to that descriptor with its string name
	* 		at a much easier convenience. 
	* WARNING: Also, if we do set array size here, then it will modify the shader's internal information which would
	* 			violate the immutability of shader_t objects. */
	// vulkan_material_set_array_size(material, "GTCBuffer.texcoords", buf_get_element_count(vulkan_host_buffered_buffer_get_host_buffer_readonly(&text->glyph_texcoord_buffer)));
	// vulkan_material_set_array_size(material, "TSTBuffer.transforms", buf_get_element_count(vulkan_host_buffered_buffer_get_host_buffer_readonly(&text->text_string_transform_buffer)));

	// vulkan_material_set_buffer(material, "GTCBuffer", vulkan_host_buffered_buffer_get_device_buffer(&text->glyph_texcoord_buffer));
	vulkan_material_set_buffer(material, "TSTBuffer", vulkan_host_buffered_buffer_get_device_buffer(&text->text_string_transform_buffer));
}

SGE_API void vulkan_bitmap_text_set_render_space_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_space_type_t space_type)
{
	if(text->render_space_type == space_type)
		return;
	text->render_space_type = space_type;

	/* if there is no material assigned then do nothing, and save the data (as above) to later apply the changes (whenver the material becomes available) */
	if(text->material == NULL)
		return;

	set_render_space_type(text, space_type);
}

SGE_API void vulkan_bitmap_text_set_render_surface_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_surface_type_t surface_type)
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

SGE_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, const char* string)
{
	vulkan_bitmap_text_string_t* text_string = get_text_stringH(text, handle);

	/* we need to clear the color ranges buffer as new char array is supplied by the user and now it is now stale */
	buf_clear(&text_string->color_ranges, NULL);

	/* clear the previous string characters */
	buf_clear(&text_string->chars, NULL);
	/* write the new characters */
	buf_push_string(&text_string->chars, string);
	/* null character at the end of the string, as we are using strlen inside text_string_set */
	buf_push_null(&text_string->chars);

	text_string_set(text, text_string, string);
}

SGE_API void vulkan_bitmap_text_string_set_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 point_size)
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

static void text_string_set_font(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, font_t* font)
{
	text_string->font = font;
	if(buf_get_element_count(&text_string->chars) > 0)
		text_string_refresh(text, text_string);
}

SGE_API void vulkan_bitmap_text_string_set_fontH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, font_t* font)
{
	text_string_set_font(text, get_text_stringH(text, handle), font);
}

SGE_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, mat4_t transform)
{
	mat4_move(get_text_stringH(text, handle)->transform, transform);
	mat4_move(transform, mat4_transpose(transform));
	_mat4_t glsl_transform = get_glsl_mat4_from_mat4(&transform);
	buf_set_at(vulkan_host_buffered_buffer_get_host_buffer(&text->text_string_transform_buffer), handle, &glsl_transform);
	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text->text_string_transform_buffer, &is_resized);
	_debug_assert__(is_updated == true);
	/* we are just updating the data, so no resize can be expected */
	_debug_assert__(is_resized == false);
}

/* NOTE: after calling this you must call vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL); to reflect changes on the GPU side */
static void text_string_set_color_range(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, u32 begin, u32 end, color_t color)
{
	/* if this text string is not active, then no color attributes should be applied as the GRD data isn't avaiable for this text string in the buffer. */
	if(!text_string->is_active)
		return;

	const char* chars = CAST_TO(const char*, buf_get_ptr(&text_string->chars));
	const u32* index_mappings = CAST_TO(const u32*, buf_get_ptr(&text_string->index_mappings));
	multi_buffer_t* host_grd_buffer = vulkan_instance_buffer_get_host_buffer(&text->glyph_render_data_buffer);
	u32 char_count = buf_get_element_count(&text_string->chars);
	_debug_assert__(char_count > 0);
	for(u32 j = begin; j < u32_min(end, char_count - 1); ++j)
	{
		AUTO index = index_mappings[j];
		/* skip the whitespaces (for which we don't have any physical texture coord information) */
		if(!isgraph(chars[j]))
		{
			_debug_assert__(index == U32_MAX);
			continue;
		}

		_debug_assert__(index != U32_MAX);

		vulkan_bitmap_text_glsl_glyph_render_data_t* data = sub_buffer_get_ptr_at(host_grd_buffer, text_string->render_data_handle, index);
		data->colr = vec4(color.r, color.g, color.b, color.a);
	}	
}

SGE_API	void vulkan_bitmap_text_string_set_activeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, bool is_active)
{
	vulkan_bitmap_text_string_t* text_string = get_text_stringH(text, handle);
	text_string->is_active = is_active;
	if(!is_active)
		text_string_set(text, text_string, "");
	else
		text_string_refresh(text, text_string);
}

SGE_API void vulkan_bitmap_text_string_set_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, color_t color)
{
	vulkan_bitmap_text_string_t* text_string = get_text_stringH(text, handle);
	text_string->color = vec4(color.r, color.g, color.b, color.a);

	/* update colors on the CPU (host) side buffer */
	AUTO char_count = buf_get_element_count(&text_string->chars);
	text_string_set_color_range(text, text_string, 0, char_count, color);

	/* update GPU (device) side buffer if it was marked dirty */
	vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);
}

SGE_API void vulkan_bitmap_text_set_font(vulkan_bitmap_text_t* text, font_t* font)
{
	text->font = font;
}

SGE_API void vulkan_bitmap_text_set_font_update_all(vulkan_bitmap_text_t* text, font_t* font)
{
	vulkan_bitmap_text_set_font(text, font);
	buf_for_each_element_ptr(vulkan_bitmap_text_string_t, text_string, &text->text_strings)
		text_string_set_font(text, text_string, font);
}

static void text_string_set_char_attr_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_t* text_string, const char_attr_color_range_t* ranges, u32 range_count)
{
	/* if this text string is not active then no need to apply any color attributes */
	if(!text_string->is_active)
		return;
DEBUG_BLOCK
(
	AUTO char_count = buf_get_element_count(&text_string->chars);
	char_count = (char_count > 0) ? (char_count - 1) : 0;
);
	for(u32 i = 0; i < range_count; i++)
	{
		const char_attr_color_range_t range = ranges[i];

		_debug_assert__(range.end <= char_count);
		_debug_assert__(range.begin <= range.end);
		text_string_set_color_range(text, text_string, range.begin, range.end, range.color);
	}
	/* update GPU (device) side buffer if it was marked dirty */
	vulkan_instance_buffer_commit(&text->glyph_render_data_buffer, NULL);
}

SGE_API void vulkan_bitmap_text_string_set_char_attr_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, const char_attr_color_range_t* ranges, const u32 range_count)
{
	vulkan_bitmap_text_string_t* text_string = get_text_stringH(text, handle);

	text_string_set_char_attr_color(text, text_string, ranges, range_count);

	/* TODO: this should be buf_clear_fast */
	buf_clear(&text_string->color_ranges, NULL);
	buf_pushv(&text_string->color_ranges, CAST_TO(void*, ranges), range_count);
}

/* getters */
SGE_API u32 vulkan_bitmap_text_get_point_size(vulkan_bitmap_text_t* text)
{
	return text->point_size;
}

SGE_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return CAST_TO(const char*, buf_get_ptr(&get_text_stringH(text, handle)->chars));
}

SGE_API u32 vulkan_bitmap_text_string_get_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->point_size;
}

SGE_API font_t* vulkan_bitmap_text_string_get_fontH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->font;
}

SGE_API u32 vulkan_bitmap_text_string_get_lengthH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	AUTO count = buf_get_element_count(&get_text_stringH(text, handle)->chars);
	_debug_assert__(count >= 1);
	// do not count the null character at the end
	return count - 1u;
}

SGE_API f32 vulkan_bitmap_text_string_get_zcoord_from_glyph_index(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 index)
{
	AUTO buf = &get_text_stringH(text, handle)->glyph_offsets;
	AUTO count = buf_get_element_count(buf);
	if(count == 0)
		return 0;
	else if(index >= count)
		return *buf_get_ptr_at_typeof(buf, f32, count - 1);
	return *buf_get_ptr_at_typeof(buf, f32, index);
}

static INLINE_IF_RELEASE_MODE f32 f32_abs(const f32 v1, const f32 v2) { f32 d = v2 - v1; return (d < 0) ? -d : d;  }

/* searches for a nearest value in the sorted float array 
 * for testing, see: unit_tests/nearest_search.cpp 
 * it has an expectation of being more efficient than linear nearest search */
static s32 binary_nearest_search_f32(const f32* arr, s32 len, f32 val)
{
    if(len == 0)
    	return -1;
    if(val >= arr[len - 1])
    	return len - 1;
    if(val <= arr[0])
    	return 0;
    f32 rd = f32_abs(arr[len - 1], val);
    f32 ld = f32_abs(arr[0], val);
    s32 dir = (ld < rd) ? 1 : -1;
    s32 i = (dir == 1) ? 0 : (len - 1);
    f32 p_d = (dir == 1) ? ld : rd;
    f32 m_d = p_d;
    s32 m_i = i;
    s32 step_size = (len - 1) / 2;
    while(true)
    {
    	i += dir * step_size;
    	if((i < 0) || (i >= len))
    	{
    		dir *= -1;
    		continue;
    	}
    	f32 d = f32_abs(arr[i], val);
    	if(d > p_d)
    	{
    		dir *= -1;
    		step_size /= 2;
    	}
    	p_d = d;
    	if(m_d > p_d)
    	{
    		m_d = p_d;
    		m_i = i;
    	}
    	if(step_size == 0)
    		break;
    }
    return m_i;
}

SGE_API u32 vulkan_bitmap_text_string_get_glyph_index_from_zcoord(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, f32 zcoord)
{
	AUTO buf = &get_text_stringH(text, handle)->glyph_offsets;
	if(buf_get_element_count(buf) == 0)
		return 0;
	const f32* floats = CAST_TO(const f32*, buf_get_ptr(buf));
	u32 index = binary_nearest_search_f32(floats, buf_get_element_count(buf), zcoord);
	_assert(index < buf_get_element_count(buf));
	return index;
}

SGE_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->transform;
}

SGE_API bool vulkan_bitmap_text_string_is_activeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle)
{
	return get_text_stringH(text, handle)->is_active;
}

SGE_API font_t* vulkan_bitmap_text_get_font(vulkan_bitmap_text_t* text)
{
	return text->font;
}