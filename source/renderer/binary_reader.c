#include <renderer/binary_reader.h>
#include <renderer/memory_allocator.h>

#include <string.h>			// for strlen

RENDERER_API binary_reader_t* binary_reader_new()
{
	binary_reader_t* reader = heap_new(binary_reader_t);
	memzero(reader, binary_reader_t);
	return reader;
}

RENDERER_API binary_reader_t* binary_reader_create(const void* data, u32 size)
{
	binary_reader_t* reader = binary_reader_new();
	reader->data = data;
	reader->size = size;
	reader->read_cursor = 0;
	reader->state_stack = buf_new(u32);
	return reader;
}

RENDERER_API void binary_reader_destroy(binary_reader_t* reader)
{
	reader->data = NULL;
	reader->size = 0;
	reader->read_cursor = 0;
	buf_clear(&reader->state_stack, NULL);
}

RENDERER_API void binary_reader_release_resources(binary_reader_t* reader)
{
	buf_free(&reader->state_stack);
	heap_free(reader);
}


RENDERER_API u8 binary_reader_u8(binary_reader_t* reader)
{
	return reader->data[reader->read_cursor++];
}

RENDERER_API u16 binary_reader_u16(binary_reader_t* reader)
{
	u16 value = DEREF_TO(u16, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(u16);
	return value;
}

RENDERER_API u32 binary_reader_u32(binary_reader_t* reader)
{
	u32 value = DEREF_TO(u32, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(u32);
	return value;
}

RENDERER_API u64 binary_reader_u64(binary_reader_t* reader)
{
	u64 value = DEREF_TO(u64, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(u64);
	return value;
}

RENDERER_API s8 binary_reader_s8(binary_reader_t* reader)
{
	s8 value = DEREF_TO(s8, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(s8);
	return value;
}

RENDERER_API s16 binary_reader_s16(binary_reader_t* reader)
{
	s16 value = DEREF_TO(s16, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(s16);
	return value;
}

RENDERER_API s32 binary_reader_s32(binary_reader_t* reader)
{
	s32 value = DEREF_TO(s32, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(s32);
	return value;
}

RENDERER_API s64 binary_reader_s64(binary_reader_t* reader)
{
	s64 value = DEREF_TO(s64, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(s64);
	return value;
}

RENDERER_API f32 binary_reader_f32(binary_reader_t* reader)
{
	f32 value = DEREF_TO(f32, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(f32);
	return value;
}

RENDERER_API f64 binary_reader_f64(binary_reader_t* reader)
{
	f64 value = DEREF_TO(f64, reader->data + reader->read_cursor);
	reader->read_cursor += sizeof(f64);
	return value;
}

RENDERER_API const char* binary_reader_str(binary_reader_t* reader)
{
	const char* str = CAST_TO(const char*, reader->data + reader->read_cursor);
	reader->read_cursor += strlen(str) + 1;
	return str;
}

RENDERER_API const void* binary_reader_at(binary_reader_t* reader, u32 index)
{
	return reader->data + index;
}

RENDERER_API const void* __binary_reader_read(binary_reader_t* reader, u32 size)
{
	u32 pos = reader->read_cursor;
	reader->read_cursor += size;
	return reader->data + pos;
}

RENDERER_API const void* binary_reader_ptr(binary_reader_t* reader)
{
	return reader->data + reader->read_cursor;
}

RENDERER_API void binary_reader_reset(binary_reader_t* reader)
{
	reader->read_cursor = 0;
}

RENDERER_API void binary_reader_jump(binary_reader_t* reader, u32 pos)
{
	reader->read_cursor = pos;
}

RENDERER_API void binary_reader_push(binary_reader_t* reader)
{
	buf_push(&reader->state_stack, &reader->read_cursor);
}

RENDERER_API void binary_reader_pop(binary_reader_t* reader)
{
	buf_pop(&reader->state_stack, &reader->state_stack);
}