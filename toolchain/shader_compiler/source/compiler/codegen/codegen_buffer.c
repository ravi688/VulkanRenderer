#include <shader_compiler/compiler/codegen/codegen_buffer.h>
#include <shader_compiler/debug.h>
#include <string.h> 		// for memcpy
#include <stdlib.h>


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
}

static void writer_insert(void* user_data, u32 index, const void* bytes, u32 size)
{
	buf_insert_pseudo(user_data, index, size);
	memcpy(buf_get_ptr_at(user_data, index), bytes, size);
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
	BUFFER* mark_table = &buffer->main->mark_table;

	/* increment all the pointers/offsets, to data section, by main_size */
	for(u32 i = MARK_ID_OFFSET; i < MARK_ID_OFFSET_MAX; i++)
	{
		buf_ucount_t index = buf_find_index_of(mark_table, &i, mark_id_comparer);
		if(index == BUF_INVALID_INDEX)
			continue;

		mark_entry_t* info = buf_get_ptr_at_typeof(mark_table, mark_entry_t, index);
		switch(info->type)
		{
			DEBUG_BLOCK (

			case MARK_TYPE_U16:
				debug_log_warning("[Codegen] Writing offsets in the codegen buffer must be of type u32, not u16");
				break;
			)

			case MARK_TYPE_U32:
				binary_writer_u32_set(buffer->main, i, info->pos + main_size);
			default:
				continue;
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

	return buffer->flat;
}
