/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: text_mesh.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_text_mesh.h>
#include <renderer/internal/vulkan/vulkan_mesh.h> 				// vulkan_mesh_create_and_vertex_buffer
#include <renderer/internal/vulkan/vulkan_instance_buffer.h> 	// vulkan_instance_buffer_t
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/internal/vulkan/vulkan_glyph_mesh_pool.h> 	// vulkan_glyph_mesh_pool_t
#include <renderer/internal/vulkan/vulkan_material.h>			// vulkan_material_set_int
#include <renderer/mesh3d.h>									// mesh3d_t
#include <renderer/assert.h>
#include <renderer/font.h>
#include <renderer/memory_allocator.h>
#include <string.h>												// strlen
#include <ctype.h> 												// isspace

#include <hpml/affine_transformation.h>

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(text_mesh)
#else
	static void check_pre_condition(vulkan_text_mesh_t* text_mesh);
#endif /*GLOBAL_DEBUG*/

#define INSTANCE_BUFFER_STRIDE glsl_sizeof(vulkan_text_mesh_glsl_glyph_render_data_t)

static sub_buffer_handle_t get_sub_buffer_handle(multi_buffer_t* multi_buffer, dictionary_t* handles, u16 key);
static vulkan_instance_buffer_t* get_instance_buffer(vulkan_renderer_t* renderer, dictionary_t* buffers, u16 key);

// constructors and destructors
RENDERER_API vulkan_text_mesh_t* vulkan_text_mesh_new(memory_allocator_t* allocator)
{
	vulkan_text_mesh_t* mesh = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_TEXT_MESH, vulkan_text_mesh_t);
	memzero(mesh, vulkan_text_mesh_t);
	return mesh;
}

RENDERER_API vulkan_text_mesh_t* vulkan_text_mesh_create(vulkan_renderer_t* renderer, vulkan_glyph_mesh_pool_t* pool)
{
	_debug_assert__(pool != NULL);
	vulkan_text_mesh_t* text_mesh = vulkan_text_mesh_new(renderer->allocator);
	text_mesh->renderer = renderer;
	text_mesh->glyph_render_data_buffers = dictionary_create(u16, vulkan_instance_buffer_t, 0, dictionary_key_comparer_u16);
	text_mesh->strings = buf_create(sizeof(vulkan_text_mesh_string_t), 1, 0);
	text_mesh->pool = pool;
	text_mesh->free_list = BUF_INVALID_INDEX; 		// free list
	text_mesh->inuse_list = BUF_INVALID_INDEX; 		// inuse list
	text_mesh->render_space_type = VULKAN_TEXT_MESH_RENDER_SPACE_TYPE_2D;
	text_mesh->render_surface_type = VULKAN_TEXT_MESH_RENDER_SURFACE_TYPE_CAMERA;
	text_mesh->point_size = font_get_char_size(vulkan_glyph_mesh_pool_get_font(pool));

	_debug_assert__(glsl_sizeof(glsl_mat4_t) == sizeof(glsl_mat4_t));

	/* create TST buffer to store string transform matrices */
	vulkan_host_buffered_buffer_create_info_t buffer_create_info =
	{
		.stride = glsl_sizeof(glsl_mat4_t),
		/* initially we can have upto 2 string transform matrices without buffer resize */
		.capacity = 2,
		/* this buffer will be indexed with glyph_render_data_t.stid */
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	vulkan_host_buffered_buffer_create_no_alloc(renderer, &buffer_create_info, &text_mesh->text_string_transform_buffer);

	return text_mesh;
}

RENDERER_API void vulkan_text_mesh_destroy(vulkan_text_mesh_t* text_mesh)
{
	dictionary_t* glyph_render_data_buffers = &text_mesh->glyph_render_data_buffers;
	BUFFER* strings = &text_mesh->strings;
	for(buf_ucount_t i = 0; i < dictionary_get_count(glyph_render_data_buffers); i++)
		vulkan_instance_buffer_destroy(dictionary_get_value_ptr_at(glyph_render_data_buffers, i));
	for(buf_ucount_t i = 0; i < buf_get_element_count(strings); i++)
		dictionary_clear(&((vulkan_text_mesh_string_t*)buf_get_ptr_at(strings, i))->glyph_sub_buffer_handles);
	buf_clear(strings, NULL);
	vulkan_host_buffered_buffer_destroy(&text_mesh->text_string_transform_buffer);
}

RENDERER_API void vulkan_text_mesh_release_resources(vulkan_text_mesh_t* text_mesh)
{
	dictionary_t* glyph_render_data_buffers = &text_mesh->glyph_render_data_buffers;
	BUFFER* strings = &text_mesh->strings;
	for(buf_ucount_t i = 0; i < dictionary_get_count(glyph_render_data_buffers); i++)
		vulkan_instance_buffer_release_resources(dictionary_get_value_ptr_at(glyph_render_data_buffers, i));
	for(buf_ucount_t i = 0; i < buf_get_element_count(strings); i++)
		dictionary_free(&((vulkan_text_mesh_string_t*)buf_get_ptr_at(strings, i))->glyph_sub_buffer_handles);
	buf_free(strings);
	dictionary_free(glyph_render_data_buffers);
	memory_allocator_dealloc(text_mesh->renderer->allocator, text_mesh);
}

// logic functions
RENDERER_API void vulkan_text_mesh_draw(vulkan_text_mesh_t* text_mesh)
{
	dictionary_t* glyph_render_data_buffers = &text_mesh->glyph_render_data_buffers;
	buf_ucount_t count = dictionary_get_count(glyph_render_data_buffers);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		u16 glyph = *(u16*)dictionary_get_key_ptr_at(glyph_render_data_buffers, i);
		vulkan_instance_buffer_t* instance_buffer = dictionary_get_value_ptr_at(glyph_render_data_buffers, i);
		if(!vulkan_instance_buffer_has_device_buffer(instance_buffer))
			continue;
		vulkan_mesh_t* mesh = vulkan_glyph_mesh_pool_get_mesh(text_mesh->pool, glyph);
		vulkan_mesh_bind_all_vertex_buffers(mesh);
		u32 binding = 5;
		vulkan_command_bind_vertex_buffers(text_mesh->renderer->vo_command_buffers[text_mesh->renderer->swapchain->current_image_index], &binding, &instance_buffer->device_buffer.vo_handle, 1);
		vulkan_mesh_draw_indexed_instanced_only(mesh, instance_buffer->device_buffer.count);
	}
}

static glsl_mat4_t get_glsl_mat4_from_mat4(const mat4_t* const mat)
{
	return (glsl_mat4_t)
	{
		.r0 = { mat->r1[0], mat->r1[1], mat->r1[2], mat->r1[3] },
		.r1 = { mat->r2[0], mat->r2[1], mat->r2[2], mat->r2[3] },
		.r2 = { mat->r3[0], mat->r3[1], mat->r3[2], mat->r3[3] },
		.r3 = { mat->r4[0], mat->r4[1], mat->r4[2], mat->r4[3] }
	};
}

static void update_host_side_tst_buffer_for_text_string(vulkan_text_mesh_t* text, vulkan_text_mesh_string_t* string)
{
	mat4_t transform = mat4_transpose(string->transform);
	glsl_mat4_t glsl_transform = get_glsl_mat4_from_mat4(&transform);
	/* the last row of 4x4 RTS matrix always remains unused - i.e it's not a perspective projection matrix where depth has to be taken care */
	glsl_transform.m33 = DREF(REINTERPRET_CAST(glsl_float_t*, &string->point_size));
	buf_set_at(vulkan_host_buffered_buffer_get_host_buffer(&text->text_string_transform_buffer), string->handle, &glsl_transform);
}

// constructors and destructors
RENDERER_API vulkan_text_mesh_string_handle_t vulkan_text_mesh_string_create(vulkan_text_mesh_t* text_mesh)
{
	BUFFER* strings = &text_mesh->strings;
	vulkan_text_mesh_string_t* string;
	vulkan_text_mesh_string_handle_t handle;
	buffer_t* tst_buffer = vulkan_host_buffered_buffer_get_host_buffer(&text_mesh->text_string_transform_buffer);
	if(text_mesh->free_list != BUF_INVALID_INDEX)
	{
		log_msg("Returning pre-allocated string\n");
		handle = text_mesh->free_list;
		string = buf_get_ptr_at(strings, handle);

		// remove from free list
		text_mesh->free_list = string->next_handle;
	}
	else
	{
		log_msg("Allocating new string\n");
		// WARNING: Can't use buf_push_pseudo here because it doesn't take care of offset bytes at the end of the internal buffer
		vulkan_text_mesh_string_t _string;
		buf_push(strings, &_string);
		string = buf_peek_ptr(strings);
		string->glyph_sub_buffer_handles = dictionary_create(u16, buf_ucount_t, 0, dictionary_key_comparer_u16);
		string->str = buf_create(sizeof(char), 0, 0);

		buf_push_pseudo(tst_buffer, 1);

		handle = buf_get_element_count(strings) - 1;
	}

	// add in inuse list
	string->next_handle = text_mesh->inuse_list;
	text_mesh->inuse_list = handle;

	string->handle = handle;
	string->transform = mat4_identity();
	string->point_size = text_mesh->point_size;

	/* update transform of this string to the TST buffer */
	update_host_side_tst_buffer_for_text_string(text_mesh, string);

	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text_mesh->text_string_transform_buffer, &is_resized);

	/* if material is not NULL then update material's TSTBuffer uniform interface block descriptor */
	if(text_mesh->material != NULL)
	{
		AUTO tst_device_buffer = vulkan_host_buffered_buffer_get_device_buffer(&text_mesh->text_string_transform_buffer);
		/* new string transform has been added, hence update the trasform count */
		if(is_updated)
			vulkan_material_set_array_size(text_mesh->material, "TSTBuffer.transforms", vulkan_buffer_get_count(tst_device_buffer));
		/* if the tst device buffer has been resized (meaning it is recreated) then rewrite the descriptors */
		if(is_resized)
			vulkan_material_set_buffer(text_mesh->material, "TSTBuffer", tst_device_buffer);
	}
	return handle;
}

RENDERER_API void vulkan_text_mesh_string_destroyH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle)
{
	BUFFER* strings = &text_mesh->strings;
	buf_ucount_t index = text_mesh->inuse_list;
	vulkan_text_mesh_string_t* prev_string = NULL;
	while((index != handle) && (index != BUF_INVALID_INDEX))
	{
		prev_string = buf_get_ptr_at(strings, index);
		index = prev_string->next_handle;
	}
	_debug_assert__(index != BUF_INVALID_INDEX);			// failed to find a string with index "handle"

	vulkan_text_mesh_string_t* string = buf_get_ptr_at_typeof(strings, vulkan_text_mesh_string_t, index);

	// reset the string for re-use later
	dictionary_clear(&string->glyph_sub_buffer_handles);
	buf_clear(&string->str, NULL);
	string->transform = mat4_identity();

	// remove from inuse list
	if(prev_string != NULL)
		prev_string->next_handle = string->next_handle;
	else
		text_mesh->inuse_list = BUF_INVALID_INDEX;

	// add to free list
	string->next_handle = text_mesh->free_list;
	text_mesh->free_list = index;
}

static void set_render_space_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_space_type_t space_type)
{
	switch(space_type)
	{
		case VULKAN_TEXT_MESH_RENDER_SPACE_TYPE_2D:
			vulkan_material_set_int(text->material, "parameters.space_type", 0);
			break;
		case VULKAN_TEXT_MESH_RENDER_SPACE_TYPE_3D:
			vulkan_material_set_int(text->material, "parameters.space_type", 1);
			break;
		default:
			debug_log_error("Invalid vulkan text mesh space type: %lu", space_type);
			break;
	}
}

static void set_render_surface_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_surface_type_t surface_type)
{
	switch(surface_type)
	{
		case VULKAN_TEXT_MESH_RENDER_SURFACE_TYPE_CAMERA:
			vulkan_material_set_int(text->material, "parameters.surface_type", 1);
			break;
		case VULKAN_TEXT_MESH_RENDER_SURFACE_TYPE_SCREEN:
			vulkan_material_set_int(text->material, "parameters.surface_type", 0);
			break;
		default:
			debug_log_error("Invalid vulkan text mesh render surface type: %lu", surface_type);
			break;
	}
}

// setters
static void commit_tst_buffer_expect_no_resize(vulkan_text_mesh_t* text_mesh)
{
	bool is_resized = false;
	bool is_updated = vulkan_host_buffered_buffer_commit(&text_mesh->text_string_transform_buffer, &is_resized);
	_debug_assert__((!is_resized) && is_updated);
}

RENDERER_API void vulkan_text_mesh_set_point_size(vulkan_text_mesh_t* text, u32 point_size)
{
	text->point_size = point_size;

	u32 count = buf_get_element_count(&text->strings);
	for(u32 i = 0; i < count; i++)
	{
		AUTO string = buf_get_ptr_at_typeof(&text->strings, vulkan_text_mesh_string_t, i);
		string->point_size = text->point_size;
		update_host_side_tst_buffer_for_text_string(text, string);
	}

	/* transfer the updated data to the gpu buffer */
	commit_tst_buffer_expect_no_resize(text);
}

RENDERER_API void vulkan_text_mesh_set_material(vulkan_text_mesh_t* text, vulkan_material_t* material)
{
	text->material = material;

	set_render_space_type(text, text->render_space_type);
	set_render_surface_type(text, text->render_surface_type);

	vulkan_material_set_buffer(material, "TSTBuffer", vulkan_host_buffered_buffer_get_device_buffer(&text->text_string_transform_buffer));
}

RENDERER_API void vulkan_text_mesh_set_render_space_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_space_type_t space_type)
{
	if(text->render_space_type == space_type)
		return;
	text->render_space_type = space_type;

	/* if there is no material assigned then do nothing, and save the data (as above) to later apply the changes (whenver the material becomes available) */
	if(text->material == NULL)
		return;

	set_render_space_type(text, space_type);
}

RENDERER_API void vulkan_text_mesh_set_render_surface_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_surface_type_t surface_type)
{
	if(text->render_surface_type == surface_type)
		return;
	text->render_surface_type = surface_type;

	/* if there is no material assigned then do nothing, and save the data (as above) to later apply the changes */
	if(text->material == NULL)
		return;

	set_render_surface_type(text, surface_type);
}

static vulkan_text_mesh_string_t* get_text_stringH(vulkan_text_mesh_t* text, vulkan_text_mesh_string_handle_t handle)
{
	return buf_get_ptr_at_typeof(&text->strings, vulkan_text_mesh_string_t, handle);
}

RENDERER_API void vulkan_text_mesh_string_setH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, const char* str)
{
	// TODO: ensure handle isn't in the free-list, meaning it has been already destroyed, this check should be in debug mode only
	vulkan_text_mesh_string_t* string = get_text_stringH(text_mesh, handle);
	u32 len = strlen(str);
	dictionary_t* glyph_render_data_buffers = &text_mesh->glyph_render_data_buffers;

	// clear the sub buffers
	u32 handle_count = dictionary_get_count(&string->glyph_sub_buffer_handles);
	for(u32 i = 0; i < handle_count; i++)
	{
		u16 ch;
		dictionary_get_key_at(&string->glyph_sub_buffer_handles, i, &ch);
		multi_buffer_t* buffer = vulkan_instance_buffer_get_host_buffer(get_instance_buffer(text_mesh->renderer, glyph_render_data_buffers, ch));
		sub_buffer_clear(buffer, get_sub_buffer_handle(buffer, &string->glyph_sub_buffer_handles, ch));
	}

	// re-write on the sub buffers
	float horizontal_pen = 0.0f;
	for(u32 i = 0; i < len; i++)
	{
		u16 ch = str[i];
		font_glyph_info_t info;
		font_get_glyph_info(vulkan_glyph_mesh_pool_get_font(text_mesh->pool), ch, &info);
		if(isspace(ch))
		{
			horizontal_pen += info.advance_width;
			continue;
		}
		vulkan_instance_buffer_t* instance_buffer = get_instance_buffer(text_mesh->renderer, glyph_render_data_buffers, ch);
		multi_buffer_t* buffer = vulkan_instance_buffer_get_host_buffer(instance_buffer);
		sub_buffer_handle_t handle = get_sub_buffer_handle(buffer, &string->glyph_sub_buffer_handles, ch);

		vulkan_text_mesh_glsl_glyph_render_data_t data =
		{
			.ofst = { 0.0f, 0.0f, horizontal_pen },
			.stid = U64_TO_U32(string->handle)
		};
		sub_buffer_push(buffer, handle, CAST_TO(void*, &data));
		horizontal_pen += info.advance_width;

		vulkan_instance_buffer_commit(instance_buffer, NULL);
	}

	/* update the device side buffers if not updated in the above loop */

	/* sub buffer handle count might have been increased due to new glyphs */
	handle_count = dictionary_get_count(&string->glyph_sub_buffer_handles);
	for(u32 i = 0; i < handle_count; i++)
	{
		u16 ch;
		dictionary_get_key_at(&string->glyph_sub_buffer_handles, i, &ch);
		vulkan_instance_buffer_commit(get_instance_buffer(text_mesh->renderer, glyph_render_data_buffers, ch), NULL);
	}
}

RENDERER_API void vulkan_text_mesh_string_set_point_sizeH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, u32 point_size)
{
	AUTO string = get_text_stringH(text_mesh, handle);
	string->point_size = point_size;

	update_host_side_tst_buffer_for_text_string(text_mesh, string);

	/* transfer the updated data to the GPU buffer */
	commit_tst_buffer_expect_no_resize(text_mesh);

	/* check if all the text strings have the same point size as 'point_size' */
	bool is_same = true;
	buf_ucount_t count = buf_get_element_count(&text_mesh->strings);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		if(buf_get_ptr_at_typeof(&text_mesh->strings, vulkan_text_mesh_string_t, i)->point_size != point_size)
		{
			is_same = false;
			break;
		}
	}

	/* if yes, then update the text_mesh's point size */
	if(is_same)
		text_mesh->point_size = point_size;
}

RENDERER_API void vulkan_text_mesh_string_set_transformH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, mat4_t transform)
{
	AUTO string = get_text_stringH(text_mesh, handle);
	mat4_move(string->transform, transform);
	update_host_side_tst_buffer_for_text_string(text_mesh, string);
	/* transfer the updated data to the GPU buffer */
	commit_tst_buffer_expect_no_resize(text_mesh);
}

// getters
RENDERER_API u32 vulkan_text_mesh_get_point_size(vulkan_text_mesh_t* text_mesh)
{
	return text_mesh->point_size;
}

RENDERER_API const char* vulkan_text_mesh_string_getH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle)
{
	return CAST_TO(const char*, buf_get_ptr(&get_text_stringH(text_mesh, handle)->str));
}

RENDERER_API u32 vulkan_text_mesh_string_get_point_sizeH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle)
{
	return get_text_stringH(text_mesh, handle)->point_size;
}

RENDERER_API mat4_t vulkan_text_mesh_string_get_transformH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle)
{
	return get_text_stringH(text_mesh, handle)->transform;
}

static sub_buffer_handle_t get_sub_buffer_handle(multi_buffer_t* multi_buffer, dictionary_t* handles, u16 key)
{
	buf_ucount_t index = dictionary_find_index_of(handles, &key);
	if(index == BUF_INVALID_INDEX)
	{
		// create new
		sub_buffer_handle_t handle = sub_buffer_create(multi_buffer, 32);
		dictionary_add(handles, &key, &handle);
		index = dictionary_get_count(handles) - 1;
	}
	return *(sub_buffer_handle_t*)dictionary_get_value_ptr_at(handles, index);
}

static vulkan_instance_buffer_t* get_instance_buffer(vulkan_renderer_t* renderer, dictionary_t* buffers, u16 key)
{
	buf_ucount_t index = dictionary_find_index_of(buffers, &key);
	if(index == BUF_INVALID_INDEX)
	{
		// create new
		vulkan_instance_buffer_t buffer;
		vulkan_instance_buffer_create_info_t create_info =
		{
			.stride = INSTANCE_BUFFER_STRIDE,
			.capacity = 10,
		};
		vulkan_instance_buffer_create(renderer, &create_info, &buffer);
		dictionary_add(buffers, &key, &buffer);
		index = dictionary_get_count(buffers) - 1;
	}
	return dictionary_get_value_ptr_at(buffers, index);
}

RENDERER_API font_t* vulkan_text_mesh_get_font(vulkan_text_mesh_t* text_mesh)
{
	return vulkan_glyph_mesh_pool_get_font(text_mesh->pool);
}
