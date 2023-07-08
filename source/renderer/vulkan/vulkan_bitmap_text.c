
#include <renderer/internal/vulkan/vulkan_bitmap_text.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator)
{
	vulkan_bitmap_text_t* text = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_BITMAP_TEXT, vulkan_bitmap_text_t);
	memzero(text, vulkan_bitmap_text_t);
	return text;
}

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_t* bga_texture)
{
	vulkan_bitmap_text_t* text = vulkan_bitmap_text_new(renderer->allocator);
	vulkan_bitmap_text_create_no_alloc(renderer, bga_texture, text);
	return text;
}

RENDERER_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_t* bga_texture, vulkan_bitmap_text_t OUT text)
{
	memzero(text, vulkan_bitmap_text_t);
	text->renderer = renderer;
	text->text_strings = buf_new(vulkan_bitmap_text_string_t);
	text->free_list = BUF_INVALID_INDEX;
	text->inuse_list = BUF_INVALID_INDEX;

	_debug_assert__(bga_texture != NULL);
	text->glyph_atlas_texture = bga_texture;
	
	/* create dictionary to store GRD buffers */
	text->glyph_render_data_buffers = dictionary_create(u16, vulkan_bitmap_glyph_render_data_buffer_t, 128, dictionary_key_comparer_u16);
	
	/* create TST buffer to store string transform matrices */
	vulkan_host_buffered_buffer_create_info_t buffer_create_info = 
	{
		.stride = glsl_sizeof(glsl_mat4_t),
		.capacity = 2,
		/* this buffer will be indexed with glyph_render_data_t.stid */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text->text_string_transform_buffer);

	/* create GTC buffer to store texture coordinates for each unique glyph */
	buffer_create_info = (vulkan_host_buffered_buffer_create_info_t)
	{
		.stride = sizeof(glyph_texcoord_t),
		.capacity = 32,
		/* this buffer will be indexed with glyph_render_data_t.indx */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text->glyph_texcoord_buffer);
}

RENDERER_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text)
{
	/* destroy the BGA Texture */
	vulkan_bitmap_glyph_atlas_texture_destroy(text->glyph_atlas_texture);

	/* destroy the subpbuffer handles buffer */
	u32 count = buf_get_element_count(&text->text_strings);
	for(u32 i = 0; i < count; i++)
	{
		AUTO string = buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, i);
		dictionary_free(&string->glyph_sub_buffer_handles);
	}
	buf_clear(&text->text_strings, NULL);

	/* destroy the GRD buffers */
	count = dictionary_get_count(&text->glyph_render_data_buffers);
	for(u32 i = 0; i < count; i++)
	{
		AUTO buffer = CAST_TO(vulkan_instance_buffer_t*, dictionary_get_value_ptr_at(&text->glyph_render_data_buffers, i));
		vulkan_instance_buffer_destroy(buffer);
		vulkan_instance_buffer_release_resources(buffer);
	}
	dictionary_clear(&text->glyph_render_data_buffers);

	/* destroy the TST buffer */
	vulkan_host_buffered_buffer_destroy(&text->text_string_transform_buffer);

	/* destroy the GTC buffer */
	vulkan_host_buffered_buffer_destroy(&text->glyph_texcoord_buffer);
}

RENDERER_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text)
{
	vulkan_bitmap_glyph_atlas_texture_release_resources(text->glyph_atlas_texture);
	buf_free(&text->text_strings);
	dictionary_free(&text->glyph_render_data_buffers);
	vulkan_host_buffered_buffer_release_resources(&text->text_string_transform_buffer);
	vulkan_host_buffered_buffer_release_resources(&text->glyph_texcoord_buffer);
}

RENDERER_API vulkan_bitmap_text_string_handle_t vulkan_bitmap_text_string_create(vulkan_bitmap_text_t* text)
{
	vulkan_bitmap_text_string_handle_t handle;
	vulkan_bitmap_text_string_t* text_string = NULL;
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
		text_string->chars = buf_new(s8);
		text_string->glyph_sub_buffer_handles = dictionary_create(u16, sub_buffer_handle_t, 2, dictionary_key_comparer_u16);
	}

	_debug_assert__(text_string != NULL);
	
	text_string->rect.offset = offset2d(0, 0);
	text_string->rect.extent = extent2d(500, 300);
	text_string->transform = mat4_identity();
	
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
	dictionary_clear(&text_string->glyph_sub_buffer_handles);
	buf_clear(&text_string->chars, NULL);
	text_string->rect.offset = offset2d(0.0f, 0.0f);
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

static vulkan_bitmap_glyph_render_data_buffer_t* get_or_create_glyph_render_data_buffer(vulkan_bitmap_text_t* text, u16 glyph)
{
	buf_ucount_t index = dictionary_find_index_of(&text->glyph_render_data_buffers, &glyph);
	if(index == BUF_INVALID_INDEX)
	{
		// create new
		vulkan_instance_buffer_t buffer;
		vulkan_instance_buffer_create_info_t create_info = 
		{
			.stride = glsl_sizeof(glsl_glyph_render_data_t),
			.capacity = 10,
		};
		vulkan_instance_buffer_create(text->renderer, &create_info, &buffer);
		dictionary_add(&text->glyph_render_data_buffers, &glyph, &buffer);
		index = dictionary_get_count(&text->glyph_render_data_buffers) - 1;
	}
	return CAST_TO(vulkan_bitmap_glyph_render_data_buffer_t*, dictionary_get_value_ptr_at(&text->glyph_render_data_buffers, index));
}

static sub_buffer_handle_t get_or_create_glyph_sub_buffer(multi_buffer_t* multi_buffer, vulkan_bitmap_glyph_sub_buffer_handle_map_t* handles, u16 glyph)
{
	buf_ucount_t index = dictionary_find_index_of(handles, &glyph);
	if(index == BUF_INVALID_INDEX)
	{
		// create new
		sub_buffer_handle_t handle = sub_buffer_create(multi_buffer, 32);
		dictionary_add(handles, &glyph, &handle);
		index = dictionary_get_count(handles) - 1;
	}
	return *(sub_buffer_handle_t*)dictionary_get_value_ptr_at(handles, index);
}

/* setters */
RENDERER_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, const char* string)
{
	AUTO text_string = buf_get_ptr_at_typeof(&text->text_strings, vulkan_bitmap_text_string_t, handle);

	/* clear the glyph sub buffers */
	u32 sub_buffer_count = dictionary_get_count(&text_string->glyph_sub_buffer_handles);
	for(u32 i = 0; i < sub_buffer_count; i++)
	{
		u16 ch;
		dictionary_get_key_at(&text_string->glyph_sub_buffer_handles, i, &ch);
		multi_buffer_t* buffer = &(get_or_create_glyph_render_data_buffer(text, ch)->host_buffer);
		sub_buffer_clear(buffer, get_or_create_glyph_sub_buffer(buffer, &text_string->glyph_sub_buffer_handles, ch));
	}

	/* rewrite on the sub buffers */
	// u32 len = strlen(str);
	// float horizontal_pen = 0.0f;
	// for(u32 i = 0; i < len; i++)
	// {
	// 	u16 ch = str[i];
	// 	font_glyph_info_t info;
	// 	font_get_glyph_info(glyph_mesh_pool_get_font(text->pool), ch, &info);
	// 	if(isspace(ch))
	// 	{
	// 		horizontal_pen += info.advance_width;
	// 		continue;
	// 	}
	// 	vulkan_instance_buffer_t* instance_buffer = get_instance_buffer(text->renderer, instance_buffers, ch);
	// 	multi_buffer_t* buffer = &instance_buffer->host_buffer;
	// 	sub_buffer_handle_t handle = get_sub_buffer_handle(buffer, &string->glyph_sub_buffer_handles, ch);
	// 	vec3_t offset = { 0, 0, horizontal_pen};
	// 	offset = vec3_add(2, offset, string->position);
	// 	char bytes[INSTANCE_BUFFER_STRIDE];

	// 	/* copy the offset */
	// 	vec4_t offset4 = { offset.x, offset.y, offset.z, 0 };
	// 	memcopyv(bytes, &offset4, u8, 16);
		
	// 	/* copy the scale */
	// 	vec4_t scale4 = { string->scale.x, string->scale.y, string->scale.z, 0 };
	// 	memcopyv(bytes + 16, &scale4, u8, 16);

	// 	/* copy the rotation */
	// 	vec4_t rotation4 = { string->rotation.x, string->rotation.y, string->rotation.z, 0 };
	// 	memcopyv(bytes + 28, &rotation4, u8, 16);

	// 	sub_buffer_push(buffer, handle, bytes);
	// 	horizontal_pen += info.advance_width;
	// }
}

RENDERER_API void vulkan_bitmap_text_string_set_scaleH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, vec3_t scale)
{

}

RENDERER_API void vulkan_bitmap_text_string_set_positionH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, vec3_t position)
{

}

RENDERER_API void vulkan_bitmap_text_string_set_rotationH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, vec3_t rotation)
{

}

RENDERER_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle, mat4_t transform)
{

}


/* setters */
RENDERER_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{

}

RENDERER_API vec3_t vulkan_bitmap_text_string_get_scaleH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return vec3_zero();
}

RENDERER_API vec3_t vulkan_bitmap_text_string_get_positionH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return vec3_zero();
}

RENDERER_API vec3_t vulkan_bitmap_text_string_get_rotationH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return vec3_zero();
}

RENDERER_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text,  vulkan_bitmap_text_string_handle_t handle)
{
	return mat4_identity();
}

