/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: codegen_buffer.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/debug.h>
#include <string.h> 		// for memcpy
#include <stdlib.h>


static u64 get_possible_value(const void* bytes, u32 size)
{
	if((bytes == NULL) || (size == 0))
		return U64_MAX;
	switch(size)
	{
		case 1:
			return CAST_TO(u64, DREF_TO(u8, bytes));
		case 2:
			return CAST_TO(u64, DREF_TO(u16, bytes));
		case 4:
			return CAST_TO(u64, DREF_TO(u32, bytes));
		case 8:
			return DREF_TO(u64, bytes);
	}
	return U64_MAX;
}

/* NOTE: we can't just pass pointer to buf_* functions to binary_writer
	because they are macros and contain some other invisible parameters 
 */
static void writer_push(void* user_data, const void* bytes, u32 size)
{
	/* see: shader_compiler/source/utilities/binary_writer->c::mark(...) */
	if(size == 0)
		return;
	u32 index = buf_get_element_count(user_data);
	buf_push_pseudo(CAST_TO(BUFFER*, user_data), size);
	if(bytes != NULL)
		memcpy(buf_get_ptr_at(user_data, index), bytes, size);
	debug_log_info("[Codegen Buffer] %llu push, position: %u, length: %u, possible value: %llu", (u64)user_data, index, size, get_possible_value(bytes, size));
}

static void writer_insert(void* user_data, u32 index, const void* bytes, u32 size)
{
	buf_insert_pseudo(user_data, index, size);
	memcpy(buf_get_ptr_at(user_data, index), bytes, size);
	debug_log_info("[Codegen Buffer] %llu insert, position: %u, length: %u", (u64)user_data, index, size);
}

static void* writer_ptr(void* user_data)
{
	return buf_get_ptr(user_data);
}

static u32 writer_pos(void* user_data)
{
	return CAST_TO(u32, buf_get_element_count(user_data));
}

SC_API codegen_buffer_t* codegen_buffer_new()
{
	codegen_buffer_t* buffer = CAST_TO(codegen_buffer_t*, malloc(sizeof(codegen_buffer_t)));
	memset(buffer, 0, sizeof(codegen_buffer_t));
	return buffer;
}

SC_API codegen_buffer_t* codegen_buffer_create()
{
	codegen_buffer_t* buffer = codegen_buffer_new();
 	buffer->main = binary_writer_create((void*)BUFcreate(NULL, sizeof(u8), 0, 0), writer_push, writer_insert, writer_ptr, writer_pos);
	buffer->data = binary_writer_create((void*)BUFcreate(NULL, sizeof(u8), 0, 0), writer_push, writer_insert, writer_ptr, writer_pos);
	return buffer;
}

SC_API void codegen_buffer_destroy(codegen_buffer_t* buffer)
{
	buf_clear(CAST_TO(BUFFER*, buffer->main->user_data), NULL);
	buf_clear(CAST_TO(BUFFER*, buffer->data->user_data), NULL);
	buf_clear(buffer->flat, NULL);
}

SC_API void codegen_buffer_release_resources(codegen_buffer_t* buffer)
{
	buf_free(CAST_TO(BUFFER*, buffer->main->user_data));
	buf_free(CAST_TO(BUFFER*, buffer->data->user_data));
	buf_free(buffer->flat);
	free(buffer);
}

SC_API BUFFER* codegen_buffer_flatten(codegen_buffer_t* buffer)
{
	u32 main_size = buf_get_element_count(CAST_TO(BUFFER*, buffer->main->user_data));
	CAN_BE_UNUSED_VARIABLE BUFFER* mark_table = &buffer->main->mark_table;

	struct
	{
		BUFFER* mark_table;
		binary_writer_t* writer;
	} update_list[2] = { { &buffer->main->mark_table, buffer->main }, { &buffer->data->mark_table, buffer->data } };

	for(u32 j = 0; j < 2; j++)
	{
		/* increment all the pointers/offsets, to data section, by main_size */
		for(u32 i = MARK_ID_OFFSET; i < MARK_ID_OFFSET_MAX; i++)
		{
			buf_ucount_t index = buf_find_index_of(update_list[j].mark_table, &i, mark_id_comparer);
			if(index == BUF_INVALID_INDEX)
				continue;

			mark_entry_t* info = buf_get_ptr_at_typeof(update_list[j].mark_table, mark_entry_t, index);
			switch(info->type)
			{
				DEBUG_BLOCK (

				case MARK_TYPE_U8:
				case MARK_TYPE_U16:
					debug_log_warning("[Codegen Buffer] Writing offsets in the codegen buffer must be of type u32, not u16 or u8");
					break;
				)

				case MARK_TYPE_U32:
				{
					AUTO writer = update_list[j].writer;
					binary_writer_u32_set(writer, i, DREF_TO(u32, writer->get_ptr(writer->user_data) + info->pos) + main_size);
					break;
				}
				default:
					continue;
			}
		}
	}

	u32 data_size = buf_get_element_count(CAST_TO(BUFFER*, buffer->data->user_data));
	u32 flat_size = main_size + data_size;
	if(buffer->flat == NULL)
		buffer->flat = BUFcreate_new(NULL, sizeof(u8), flat_size, 0);

	buf_clear(buffer->flat, NULL);
	buf_push_pseudo(buffer->flat, flat_size);

	void* ptr = buf_get_ptr(buffer->flat);
	memcpy(ptr, buf_get_ptr(CAST_TO(BUFFER*, buffer->main->user_data)), main_size);
	memcpy(ptr + main_size, buf_get_ptr(CAST_TO(BUFFER*, buffer->data->user_data)), data_size);

	debug_log_info("[Codegen Buffer] data section size: %lu", data_size);
	debug_log_info("[Codegen Buffer] main section size: %lu", main_size);

	return buffer->flat;
}
