#include <shader_compiler/utilities/binary_writer.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>

#include <stdlib.h>
#include <string.h> 	// for memset


SC_API binary_writer_t* binary_writer_new()
{
	binary_writer_t* writer = (binary_writer_t*)malloc(sizeof(binary_writer_t));
	memset(writer, 0, sizeof(binary_writer_t));
	return writer;
}

typedef struct mark_info_t
{
	u32 pos;
	mark_type_t type;
} mark_info_t;

SC_API binary_writer_t* binary_writer_create(
												void* user_data,
												void (*push)(void* user_data, const void* bytes, u32 size),
												void (*insert)(void* user_data, u32 index, const void* bytes, u32 size),
												void* (*get_ptr)(void* user_data),
												u32 (*write_pos)(void* user_data))
{

	binary_writer_t* writer = binary_writer_new();
	writer->user_data = user_data;
	writer->push = push;
	writer->insert = insert;
	writer->get_ptr = get_ptr;
	writer->write_pos = write_pos;
	writer->mark_table = dictionary_create(u32, mark_info_t, 1, dictionary_key_comparer_u32);
	return writer;
}

SC_API void binary_writer_destroy(binary_writer_t* writer)
{
	dictionary_clear(&writer->mark_table);
}

SC_API void binary_writer_release_resources(binary_writer_t* writer)
{
	dictionary_free(&writer->mark_table);
	free(writer);
}


SC_API void binary_writer_string(binary_writer_t* writer, const char* str)
{
	/* write the string along with its null character, assuming the string is already null terminated */
	writer->push(writer->user_data, str, strlen(str) + 1);
}

SC_API void binary_writer_stringn(binary_writer_t* writer, const char* str, u32 len)
{
	/* write the string with length 'len', assuming the str is not null terminated */
	writer->push(writer->user_data, str, len);
	/* since the string is not null terminated, we need to push a null character at the of the written string */
	char null_char = 0;
	writer->push(writer->user_data, &null_char, 1);
}

SC_API void binary_writer_u8(binary_writer_t* writer, u8 v)
{
	writer->push(writer->user_data, &v, sizeof(u8));
}

SC_API void binary_writer_u16(binary_writer_t* writer, u16 v)
{
	writer->push(writer->user_data, &v, sizeof(u16));
}

SC_API void binary_writer_u32(binary_writer_t* writer, u32 v)
{
	writer->push(writer->user_data, &v, sizeof(u32));
}

SC_API void binary_writer_u64(binary_writer_t* writer, u64 v)
{
	writer->push(writer->user_data, &v, sizeof(u64));
}

SC_API void binary_writer_s8(binary_writer_t* writer, s8 v)
{
	writer->push(writer->user_data, &v, sizeof(s8));
}

SC_API void binary_writer_s16(binary_writer_t* writer, s16 v)
{
	writer->push(writer->user_data, &v, sizeof(s16));
}

SC_API void binary_writer_s32(binary_writer_t* writer, s32 v)
{
	writer->push(writer->user_data, &v, sizeof(s32));
}

SC_API void binary_writer_s64(binary_writer_t* writer, s64 v)
{
	writer->push(writer->user_data, &v, sizeof(s64));
}

SC_API void binary_writer_f32(binary_writer_t* writer, f32 v)
{
	writer->push(writer->user_data, &v, sizeof(f32));
}

SC_API void binary_writer_f64(binary_writer_t* writer, f64 v)
{
	writer->push(writer->user_data, &v, sizeof(f64));
}

SC_API u32 binary_writer_pos(binary_writer_t* writer)
{
	return CAST_TO(u32, writer->write_pos(writer->user_data));
}

static u32 _sizeof(mark_type_t type)
{
	switch(type)
	{
		case MARK_TYPE_U16:
			return 2;
		case MARK_TYPE_U32:
			return 4;
		case MARK_TYPE_UNDEFINED:
		default:
			return 0;
	}
	return type;
}

/* marks the current write position with mark ID 'mark_id' and for the data of type 'type' */
static void mark(binary_writer_t* writer, u32 mark_id, mark_type_t type)
{
	/* get the current write position */
	u32 curr_write_pos = writer->write_pos(writer->user_data);
	/* check if the the current write position has already been marked with someother mark IDs */
	buf_ucount_t index = dictionary_find_index_of(&writer->mark_table, &mark_id);
	if(index != BUF_INVALID_INDEX)
	{
		debug_log_error("[Binary Writer] The write position %ul has already been marked with mark ID %ul but you are still trying to remark it with mark id: %ul",
						curr_write_pos, (u32)index, mark_id);
		return;
	}

	/* add the mark info into the mark table */
	mark_info_t info = { curr_write_pos, type };
	dictionary_push(&writer->mark_table, &mark_id, &info);

	/* 	allocate some bytes to accomodate the data to be written later; 
		NOTE: passing NULL to the data ptr only allocates memory
		NOTE: if type == MARK_TYPE_UNDEFINED, _sizeof returns 0, in turn push will not write
			  anything in the memory buffer
	*/
	writer->push(writer->user_data, NULL, _sizeof(type));
}

/* sets or inserts a number of bytes from the marked write position with markID 'mark_id' */
static void set_or_insert(binary_writer_t* writer, u32 mark_id, const void* bytes, u32 size)
{
	/* check if the mark ID exists in the mark table */
	buf_ucount_t index = dictionary_find_index_of(&writer->mark_table, &mark_id);
	if(index == BUF_INVALID_INDEX)
	{
		debug_log_error("[Binary Writer] There is no such mark exists with mark ID %ul", mark_id);
		return;
	}
	/* get the mark_info_t for the mark ID */
	mark_info_t* data = CAST_TO(mark_info_t*, dictionary_get_value_ptr_at(&writer->mark_table, index));
	switch(data->type)
	{
		/* if marked for variable number of bytes */
		case MARK_TYPE_UNDEFINED:
			writer->insert(writer->user_data, index, bytes, size);
			break;
		/* if marked for fixed number of bytes */
		default:
			{
				u32 _size = _sizeof(data->type);
				assert(_size == size);
				/* get the pointer to the internal memory buffer */
				void* ptr = writer->get_ptr(writer->user_data);
				/* copy the data from the marked positon */
				memcpy(ptr + index, bytes, _size);
			}
			break;
	}
}

SC_API void binary_writer_u16_mark(binary_writer_t* writer, u32 mark_id)
{
	mark(writer, mark_id, MARK_TYPE_U16);
}

SC_API void binary_writer_u16_set(binary_writer_t* writer, u32 mark_id, u16 v)
{
	set_or_insert(writer, mark_id, &v, sizeof(u16));
}

SC_API void binary_writer_u32_mark(binary_writer_t* writer, u32 mark_id)
{
	mark(writer, mark_id, MARK_TYPE_U32);
}

SC_API void binary_writer_u32_set(binary_writer_t* writer, u32 mark_id, u32 v)
{
	set_or_insert(writer, mark_id, &v, sizeof(u32));
}

SC_API void binary_writer_mark(binary_writer_t* writer, u32 mark_id)
{
	mark(writer, mark_id, MARK_TYPE_UNDEFINED);
}

SC_API void binary_writer_insert(binary_writer_t* writer, u32 mark_id, const void* bytes, u32 size)
{
	set_or_insert(writer, mark_id, bytes, size);
}
