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
#include <common/id_generator.h> /* for id_generator_t */
#include <string.h> 		// for memcpy
#include <stdlib.h>


CAN_BE_UNUSED_FUNCTION static u64 get_possible_value(const void* bytes, u32 size)
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

SC_API codegen_buffer_t* codegen_buffer_new(com_allocation_callbacks_t* callbacks)
{
	codegen_buffer_t* buffer = CAST_TO(codegen_buffer_t*, com_call_allocate(callbacks, sizeof(codegen_buffer_t)));
	memset(buffer, 0, sizeof(codegen_buffer_t));
	buffer->callbacks = callbacks;
	return buffer;
}

static bool section_name_comparer(void* compare_key, void* key)
{
	return static_string_64_cmp(CAST_TO(static_string_64_t*, compare_key), CAST_TO(static_string_64_t*, key)) == 0;
}

typedef struct pointer_info_t
{
	/* address representing reference in any section */
	codegen_buffer_address_t address;
	/* mark id refering to the index at which the above address.offset has been stored in this section */
	u32 mark;
} pointer_info_t;

typedef buffer_t pointer_info_list_t;

typedef struct section_t
{
	/* generates in the scope of this section */
	id_generator_t id_generator;
	/* writes data into the section buffer */
	binary_writer_t* writer;
	/* list of offsets (pointers) in this section 
	 * Remember: mark ids which are offsets are different from those which are not! 
	 * it is lazily created, i.e. only if codegen_buffer_set_pointer is called */
	pointer_info_list_t* ptr_list;
} section_t;

SC_API codegen_buffer_t* codegen_buffer_create(com_allocation_callbacks_t* callbacks)
{
	codegen_buffer_t* buffer = codegen_buffer_new(callbacks);
	buffer->map = dictionary_create(static_string_64_t, section_t, 1, section_name_comparer);
	return buffer;
}

static INLINE_IF_RELEASE_MODE u32 get_section_count(codegen_buffer_t* buffer)
{
	return CAST_TO(u32, dictionary_get_count(&buffer->map));
}

static INLINE_IF_RELEASE_MODE section_t* get_section_from_index(codegen_buffer_t* buffer, u32 index)
{
	_com_assert(dictionary_get_count(&buffer->map) > index);
	return CAST_TO(section_t*, dictionary_get_value_ptr_at(&buffer->map, index));
}

SC_API void codegen_buffer_destroy(codegen_buffer_t* buffer)
{
	/* iterate over each section and destroy its writer and ptr list */
	u32 count = get_section_count(buffer);
	for(u32 i = 0; i < count; i++)
	{
		AUTO section = get_section_from_index(buffer, i);
		id_generator_destroy(&section->id_generator);
		buf_free(CAST_TO(BUFFER*, section->writer->user_data));
		binary_writer_destroy(section->writer);
		binary_writer_release_resources(section->writer);
		/* if ptr list has been created then free it */
		if(section->ptr_list != NULL)
			buf_free(section->ptr_list);
	}
	dictionary_clear(&buffer->map);
}

SC_API void codegen_buffer_release_resources(codegen_buffer_t* buffer)
{
	dictionary_free(&buffer->map);
	com_call_deallocate(buffer->callbacks, buffer);
}

static INLINE_IF_RELEASE_MODE buf_ucount_t get_section_index(codegen_buffer_t* buffer, const char* section_name)
{
	static_string_64_t str = static_string_64(section_name);
	return dictionary_find_index_of(&buffer->map, &str);
}

static INLINE_IF_RELEASE_MODE binary_writer_t* get_writer_from_index(codegen_buffer_t* buffer, u32 index)
{
	return get_section_from_index(buffer, index)->writer;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE section_t* get_section(codegen_buffer_t* buffer, const char* section_name)
{
	AUTO index = get_section_index(buffer, section_name);
	if(index == BUF_INVALID_INDEX)
		return NULL;
	else
		return get_section_from_index(buffer, index);
}

static binary_writer_t* get_writer(codegen_buffer_t* buffer, const char* section_name)
{
	AUTO index = get_section_index(buffer, section_name);
	if(index == BUF_INVALID_INDEX)
		return NULL;
	else
		return get_writer_from_index(buffer, index);
}

SC_API binary_writer_t* codegen_buffer_get_section(codegen_buffer_t* buffer, const char* section_name)
{
	AUTO result = get_writer(buffer, section_name);
	_com_assert(result != NULL);
	return result;
}

SC_API binary_writer_t* codegen_buffer_create_or_get_section(codegen_buffer_t* buffer, const char* section_name)
{
	AUTO result = get_writer(buffer, section_name);
	/* if section already exists then just return it */
	if(result != NULL)
		return result;
	/* otherwise create a new section */
	else
	{
		debug_log_info("Creating new section in codegen buffer named as \"%s\"", section_name);
		AUTO writer = binary_writer_create((void*)BUFcreate_with_callbacks(buffer->callbacks, NULL, sizeof(u8), 0, 0), writer_push, writer_insert, writer_ptr, writer_pos);
		AUTO str = static_string_64(section_name);
		section_t section = { .writer = writer, .id_generator = id_generator_create(0, buffer->callbacks) };
		dictionary_add(&buffer->map, &str, &section);
		return writer;
	}
}

SC_API void codegen_buffer_write(codegen_buffer_t* buffer, const char* section_name, void* bytes, u32 num_bytes)
{
	binary_writer_write(get_writer(buffer, section_name), bytes, num_bytes);
}

SC_API void codegen_buffer_write_pointer(codegen_buffer_t* buffer, const char* section_name, codegen_buffer_address_t address)
{
	AUTO addr = codegen_buffer_alloc_pointer(buffer, section_name);
	codegen_buffer_set_pointer(buffer, addr, address);
}

static codegen_buffer_address_t codegen_buffer_alloc(codegen_buffer_t* buffer, const char* section_name, void (*mark_fnptr)(binary_writer_t* writer, u32 mark_id))
{
	_com_assert(mark_fnptr != NULL);
	AUTO index = get_section_index(buffer, section_name);
	_com_assert(index != BUF_INVALID_INDEX);
	AUTO section = get_section_from_index(buffer, index);
	u32 id = id_generator_get(&section->id_generator);
	mark_fnptr(section->writer, id);
	return (codegen_buffer_address_t)
	{
		.section_id = index,
		.mark = id
	};	
}

SC_API void codegen_buffer_set_pointer(codegen_buffer_t* buffer, codegen_buffer_address_t addr, codegen_buffer_address_t address)
{
	pointer_info_t p_info = { .address = address, .mark = addr.mark };
	AUTO section = get_section_from_index(buffer, addr.section_id);
	if(section->ptr_list == NULL)
		section->ptr_list = BUFcreate_with_callbacks(buffer->callbacks, NULL, sizeof(pointer_info_t), 1, 0);
	buf_push(section->ptr_list, &p_info);
}


SC_API codegen_buffer_address_t codegen_buffer_alloc_unsized(codegen_buffer_t* buffer, const char* section_name)
{
	return codegen_buffer_alloc(buffer, section_name, binary_writer_mark);
}

SC_API void codegen_buffer_insert(codegen_buffer_t* buffer, codegen_buffer_address_t addr, const void* bytes, u32 num_bytes)
{
	binary_writer_insert(get_writer_from_index(buffer, addr.section_id), addr.mark, bytes, num_bytes);
}


SC_API void codegen_buffer_write_u32(codegen_buffer_t* buffer, const char* section_name, u32 value)
{
	binary_writer_u32(get_writer(buffer, section_name), value);
}

SC_API codegen_buffer_address_t codegen_buffer_alloc_u32(codegen_buffer_t* buffer, const char* section_name)
{
	return codegen_buffer_alloc(buffer, section_name, binary_writer_u32_mark);
}

SC_API void codegen_buffer_set_u32(codegen_buffer_t* buffer, codegen_buffer_address_t addr, u32 value)
{
	binary_writer_u32_set(get_writer_from_index(buffer, addr.section_id), addr.mark, value);
}


SC_API void codegen_buffer_write_u16(codegen_buffer_t* buffer, const char* section_name, u32 value)
{
	binary_writer_u16(get_writer(buffer, section_name), value);
}

SC_API codegen_buffer_address_t codegen_buffer_alloc_u16(codegen_buffer_t* buffer, const char* section_name)
{
	return codegen_buffer_alloc(buffer, section_name, binary_writer_u16_mark);
}

SC_API void codegen_buffer_set_u16(codegen_buffer_t* buffer, codegen_buffer_address_t addr, u16 value)
{
	binary_writer_u16_set(get_writer_from_index(buffer, addr.section_id), addr.mark, value);
}


SC_API void codegen_buffer_write_u8(codegen_buffer_t* buffer, const char* section_name, u32 value)
{
	binary_writer_u8(get_writer(buffer, section_name), value);
}

SC_API codegen_buffer_address_t codegen_buffer_alloc_u8(codegen_buffer_t* buffer, const char* section_name)
{
	return codegen_buffer_alloc(buffer, section_name, binary_writer_u8_mark);
}

SC_API void codegen_buffer_set_u8(codegen_buffer_t* buffer, codegen_buffer_address_t addr, u8 value)
{
	binary_writer_u8_set(get_writer_from_index(buffer, addr.section_id), addr.mark, value);
}

SC_API void codegen_buffer_write_stringn(codegen_buffer_t* buffer, const char* section_name, const char* str, u32 len)
{
	binary_writer_stringn(get_writer(buffer, section_name), str, len);
}

SC_API void codegen_buffer_write_string(codegen_buffer_t* buffer, const char* section_name, const char* str)
{
	binary_writer_string(get_writer(buffer, section_name), str);
}

static u32 get_absolute_address(codegen_buffer_t* buffer, codegen_buffer_address_t address)
{
	_com_assert(address.section_id < dictionary_get_count(&buffer->map));

	/* offset in the refered section */
	u32 ofst = binary_writer_pos_from_mark_id(get_writer_from_index(buffer, address.section_id), address.mark);

	/* add sizes of the previous section */
	for(u32 i = 0; i < address.section_id; i++)
		ofst += binary_writer_pos(get_writer_from_index(buffer, i));

	return ofst;
}

static u64 get_total_size(codegen_buffer_t* buffer)
{
	u64 size = 0;
	u32 count = get_section_count(buffer);
	for(u32 i = 0; i < count; i++)
		size += binary_writer_pos(get_writer_from_index(buffer, i));
	return size;
}

static void resolve_pointers(codegen_buffer_t* buffer)
{
	/* pseudo code:

		// brute force solution
		get_absolute_address(list, section_id, offset):
			addr = offset
			for i = 0 until less than section_id:
				addr += get_pos(list[i])
			ret addr
		resolve_pointers(list):
			for i = 0 until less than list.size():
				ptr_list = list[i].ptr_list
				for j = 0 until less than ptr_list.size():
					addr = get_absolute_address(ptr_list[j].section_id, ptr_list[j].offset)
					store_u32(list[i], ptr_list[j].this_offset, addr)

		// optimized solution
		// we may create an optimized solution but the time spent doesn't justify the improvement
	*/

	u32 section_count = get_section_count(buffer);
	for(u32 i = 0; i < section_count; i++)
	{
		AUTO section = get_section_from_index(buffer, i);
		AUTO ptr_list = section->ptr_list;
		if(ptr_list != NULL)
		{
			u32 ptr_count = buf_get_element_count(ptr_list);
			for(u32 j = 0; j < ptr_count; j++)
			{
				AUTO p_info = buf_get_ptr_at_typeof(ptr_list, pointer_info_t, j);
				u32 addr = get_absolute_address(buffer, p_info->address);
				binary_writer_u32_set(section->writer, p_info->mark, addr);
			}
		}
	}
}

static buffer_t* merge_buffers_into_one(codegen_buffer_t* buffer)
{
	/* get the total size need to accomodate every section */
	u32 flat_size = get_total_size(buffer);

	/* allocate the biggest buffer (the flat buffer) */
	buffer_t* flat_buffer = BUFcreate_with_callbacks(buffer->callbacks, NULL, sizeof(u8), flat_size, 0);

	u32 count = get_section_count(buffer);
	for(u32 i = 0; i < count; i++)
	{
		AUTO writer_buffer = CAST_TO(buffer_t*, get_writer_from_index(buffer, i)->user_data);
		debug_log_info("[Codegen Buffer] %s section size: %lu, offset: %lu", 
							dictionary_get_key_ptr_at(&buffer->map, i), 
							buf_get_element_count(writer_buffer),
							buf_get_element_count(flat_buffer));
		buf_pushv(flat_buffer, buf_get_ptr(writer_buffer), buf_get_element_count(writer_buffer));
	}
	return flat_buffer;
}

SC_API BUFFER* codegen_buffer_flatten(codegen_buffer_t* buffer)
{
	/* resolve pointers referencing to other sections etc. */
	resolve_pointers(buffer);
	/* then merge the section buffers in the order they have been created by calling codegen_buffer_create_or_get_section into one buffer */
	return merge_buffers_into_one(buffer);
}
