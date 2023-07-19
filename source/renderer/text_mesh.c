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

#include <renderer/renderer.h>
#include <renderer/text_mesh.h>									// text_mesh_t
#include <renderer/glyph_mesh_pool.h> 							// glyph_mesh_pool_t
#include <renderer/mesh3d.h>									// mesh3d_t
#include <renderer/internal/vulkan/vulkan_mesh.h> 				// vulkan_mesh_create_and_vertex_buffer
#include <renderer/internal/vulkan/vulkan_instance_buffer.h> 	// vulkan_instance_buffer_t
#include <renderer/internal/vulkan/vulkan_command.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>
#include <renderer/dictionary.h>											// dictionary_t
#include <renderer/multi_buffer.h>										// mult_buffer_t
#include <string.h>												// strlen
#include <ctype.h> 												// isspace

#include <hpml/vec3.h>
#include <hpml/vec4.h>
#include <hpml/affine_transformation.h>

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(text_mesh)
#else
	static void check_pre_condition(text_mesh_t* text_mesh);
#endif /*GLOBAL_DEBUG*/

#define INSTANCE_BUFFER_STRIDE (sizeof(vec4_t) /* offset (vec3 + 1 padding) */ + sizeof(vec4_t) /* scale (vec3 + 1 padding) */+ sizeof(vec4_t) /* rotation (vec3 + 1 padding) */)

typedef struct text_mesh_string_t
{
	buf_ucount_t next_index;												// 	index of the next string in the buffer
	dictionary_t/*(u16, sub_buffer_handle_t)*/ glyph_sub_buffer_handles;	//	dictionary of sub_buffer handles in the multi_buffer of glyphs
	BUFFER str; 															//	buffer of characters to store the supplied string
	vec3_t position;													//	position
	vec3_t rotation;													//	euler rotation
	vec3_t scale;													//	scale
} text_mesh_string_t;

typedef struct text_mesh_t
{
	renderer_t* renderer;
	material_t* material;
	glyph_mesh_pool_t* pool;												// pool from which the mesh_t should be queried from for a particular glyph
	dictionary_t /*(u16, vulkan_instance_buffer_t)*/ instance_buffers;		// array of instance_buffer mapped to each unique glyph considering all the strings
	BUFFER /*text_mesh_string_t*/ strings;									// array of string (text_mesh_string_t) instances
	buf_ucount_t free_list;
	buf_ucount_t inuse_list;
} text_mesh_t;


static sub_buffer_handle_t get_sub_buffer_handle(multi_buffer_t* multi_buffer, dictionary_t* handles, u16 key);
static vulkan_instance_buffer_t* get_instance_buffer(renderer_t* renderer, dictionary_t* buffers, u16 key);

// constructors and destructors
RENDERER_API text_mesh_t* text_mesh_new(memory_allocator_t* allocator)
{
	text_mesh_t* mesh = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEXT_MESH, text_mesh_t);
	memzero(mesh, text_mesh_t);
	return mesh;
}

RENDERER_API text_mesh_t* text_mesh_create(renderer_t* renderer, glyph_mesh_pool_t* pool)
{
	_debug_assert__(pool != NULL);
	text_mesh_t* text_mesh = text_mesh_new(renderer->allocator);
	text_mesh->renderer = renderer;
	text_mesh->instance_buffers = dictionary_create(u16, vulkan_instance_buffer_t, 0, dictionary_key_comparer_u16);
	text_mesh->strings = buf_create(sizeof(text_mesh_string_t), 1, 0);
	text_mesh->pool = pool;
	text_mesh->free_list = BUF_INVALID_INDEX; 		// free list
	text_mesh->inuse_list = BUF_INVALID_INDEX; 		// inuse list
	return text_mesh;
}

RENDERER_API void text_mesh_destroy(text_mesh_t* text_mesh)
{
	dictionary_t* instance_buffers = &text_mesh->instance_buffers;
	BUFFER* strings = &text_mesh->strings;
	for(buf_ucount_t i = 0; i < dictionary_get_count(instance_buffers); i++)
		vulkan_instance_buffer_destroy(dictionary_get_value_ptr_at(instance_buffers, i));
	for(buf_ucount_t i = 0; i < buf_get_element_count(strings); i++)
		dictionary_clear(&((text_mesh_string_t*)buf_get_ptr_at(strings, i))->glyph_sub_buffer_handles);
	buf_clear(strings, NULL);
}

RENDERER_API void text_mesh_release_resources(text_mesh_t* text_mesh)
{
	dictionary_t* instance_buffers = &text_mesh->instance_buffers;
	BUFFER* strings = &text_mesh->strings;
	for(buf_ucount_t i = 0; i < dictionary_get_count(instance_buffers); i++)
		vulkan_instance_buffer_release_resources(dictionary_get_value_ptr_at(instance_buffers, i));
	for(buf_ucount_t i = 0; i < buf_get_element_count(strings); i++)
		dictionary_free(&((text_mesh_string_t*)buf_get_ptr_at(strings, i))->glyph_sub_buffer_handles);
	buf_free(strings);
	dictionary_free(instance_buffers);
	memory_allocator_dealloc(text_mesh->renderer->allocator, text_mesh);
}

// logic functions
RENDERER_API void text_mesh_draw(text_mesh_t* text_mesh)
{
	dictionary_t* instance_buffers = &text_mesh->instance_buffers;
	buf_ucount_t count = dictionary_get_count(instance_buffers);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		u16 glyph = *(u16*)dictionary_get_key_ptr_at(instance_buffers, i);
		vulkan_instance_buffer_t* instance_buffer = dictionary_get_value_ptr_at(instance_buffers, i);
		if(!vulkan_instance_buffer_has_device_buffer(instance_buffer))
			continue;
		mesh_t* mesh = glyph_mesh_pool_get_mesh(text_mesh->pool, glyph);
		vulkan_mesh_bind_all_vertex_buffers(mesh);
		u32 binding = 5;
		AUTO vk_renderer = VULKAN_RENDERER(text_mesh->renderer);
		vulkan_command_bind_vertex_buffers(vk_renderer->vo_command_buffers[vk_renderer->swapchain->current_image_index], &binding, &instance_buffer->device_buffer.vo_handle, 1);
		vulkan_mesh_draw_indexed_instanced_only(mesh, instance_buffer->device_buffer.count);
	}
}

RENDERER_API void text_mesh_set_material(text_mesh_t* text, material_t* material)
{
	text->material = material;
}

// constructors and destructors
RENDERER_API text_mesh_string_handle_t text_mesh_string_create(text_mesh_t* text_mesh)
{
	BUFFER* strings = &text_mesh->strings;
	text_mesh_string_t* string; 
	if(text_mesh->free_list != BUF_INVALID_INDEX)
	{	
		log_msg("Returning pre-allocated string\n");
		buf_ucount_t index = text_mesh->free_list;
		string = buf_get_ptr_at(strings, index);
		
		// remove from free list
		text_mesh->free_list = string->next_index;

		// add in inuse list
		string->next_index = text_mesh->inuse_list;
		text_mesh->inuse_list = index;
	}
	else
	{
		log_msg("Allocating new string\n");
		// WARNING: Can't use buf_push_pseudo here because it doesn't take care of offset bytes at the end of the internal buffer
		text_mesh_string_t _string;
		buf_push(strings, &_string);
		string = buf_peek_ptr(strings);
		string->glyph_sub_buffer_handles = dictionary_create(u16, buf_ucount_t, 0, dictionary_key_comparer_u16);
		string->str = buf_create(sizeof(char), 0, 0);
		string->position = vec3_zero();
		string->rotation = vec3_zero();
		string->scale = vec3(1, 1, 1);

		// add in inuse list
		string->next_index = text_mesh->inuse_list;
		text_mesh->inuse_list = buf_get_element_count(strings) - 1;
	}
	return text_mesh->inuse_list;
}

RENDERER_API void text_mesh_string_destroyH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	BUFFER* strings = &text_mesh->strings;
	buf_ucount_t index = text_mesh->inuse_list;
	text_mesh_string_t* prev_string = NULL;
	while((index != handle) && (index != BUF_INVALID_INDEX))
	{
		prev_string = buf_get_ptr_at(strings, index);
		index = prev_string->next_index;
	}
	_debug_assert__(index != BUF_INVALID_INDEX);			// failed to find a string with index "handle"

	text_mesh_string_t* string = buf_get_ptr_at(strings, index);
	
	// reset the string for re-use later
	dictionary_clear(&string->glyph_sub_buffer_handles);
	buf_clear(&string->str, NULL);
	string->position = vec3_zero();
	string->rotation = vec3_zero();
	string->scale = vec3(1, 1, 1);

	// remove from inuse list
	if(prev_string != NULL)
		prev_string->next_index = string->next_index;
	else
		text_mesh->inuse_list = BUF_INVALID_INDEX;
	
	// add to free list
	string->next_index = text_mesh->free_list;
	text_mesh->free_list = index;
}

// setters
RENDERER_API void text_mesh_string_setH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, const char* str)
{
	// TODO: ensure handle isn't in the free-list, meaning it has been already destroyed, this check should be in debug mode only
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	u32 len = strlen(str);
	dictionary_t* instance_buffers = &text_mesh->instance_buffers;

	// clear the sub buffers
	u32 handle_count = dictionary_get_count(&string->glyph_sub_buffer_handles);
	for(u32 i = 0; i < handle_count; i++)
	{
		u16 ch;
		dictionary_get_key_at(&string->glyph_sub_buffer_handles, i, &ch);
		multi_buffer_t* buffer = vulkan_instance_buffer_get_host_buffer(get_instance_buffer(text_mesh->renderer, instance_buffers, ch));
		sub_buffer_clear(buffer, get_sub_buffer_handle(buffer, &string->glyph_sub_buffer_handles, ch));
	}

	// re-write on the sub buffers
	float horizontal_pen = 0.0f;
	for(u32 i = 0; i < len; i++)
	{
		u16 ch = str[i];
		font_glyph_info_t info;
		font_get_glyph_info(glyph_mesh_pool_get_font(text_mesh->pool), ch, &info);
		if(isspace(ch))
		{
			horizontal_pen += info.advance_width;
			continue;
		}
		vulkan_instance_buffer_t* instance_buffer = get_instance_buffer(text_mesh->renderer, instance_buffers, ch);
		multi_buffer_t* buffer = vulkan_instance_buffer_get_host_buffer(instance_buffer);
		sub_buffer_handle_t handle = get_sub_buffer_handle(buffer, &string->glyph_sub_buffer_handles, ch);
		vec3_t offset = { 0, 0, horizontal_pen};
		offset = vec3_add(2, offset, string->position);
		char bytes[INSTANCE_BUFFER_STRIDE];

		/* copy the offset */
		vec4_t offset4 = { offset.x, offset.y, offset.z, 0 };
		memcopyv(bytes, &offset4, u8, 16);
		
		/* copy the scale */
		vec4_t scale4 = { string->scale.x, string->scale.y, string->scale.z, 0 };
		memcopyv(bytes + 16, &scale4, u8, 16);

		/* copy the rotation */
		vec4_t rotation4 = { string->rotation.x, string->rotation.y, string->rotation.z, 0 };
		memcopyv(bytes + 28, &rotation4, u8, 16);

		sub_buffer_push(buffer, handle, bytes);
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
		vulkan_instance_buffer_commit(get_instance_buffer(text_mesh->renderer, instance_buffers, ch), NULL);
	}
}

RENDERER_API void text_mesh_string_set_scaleH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t scale)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	string->scale = scale;
}

RENDERER_API void text_mesh_string_set_positionH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t position)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	string->position = position;
}

RENDERER_API void text_mesh_string_set_rotationH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t rotation)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	string->rotation = rotation;
}

RENDERER_API void text_mesh_string_set_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, mat4_t transform)
{
}

// getters
RENDERER_API const char* text_mesh_string_getH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	return NULL;
}
RENDERER_API vec3_t text_mesh_string_get_scaleH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	return string->scale;
}
RENDERER_API vec3_t text_mesh_string_get_positionH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	return string->position;
}
RENDERER_API vec3_t text_mesh_string_get_rotationH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	text_mesh_string_t* string = buf_get_ptr_at(&text_mesh->strings, handle);
	return string->rotation;
}
RENDERER_API mat4_t text_mesh_string_get_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle)
{
	return mat4_identity();
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

static vulkan_instance_buffer_t* get_instance_buffer(renderer_t* renderer, dictionary_t* buffers, u16 key)
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
		vulkan_instance_buffer_create(renderer->handle, &create_info, &buffer);
		dictionary_add(buffers, &key, &buffer);
		index = dictionary_get_count(buffers) - 1;
	}
	return dictionary_get_value_ptr_at(buffers, index);
}
