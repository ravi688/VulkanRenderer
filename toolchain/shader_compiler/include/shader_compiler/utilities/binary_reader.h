/* This class is responsible for reader binary data in a convenient way.
 *
 * Examples:
 * 
 * BUFFER* data = load_binary_from_file("myFile.bin");				// include <disk_manager/file_reader.h>
 * binary_reader_t* reader = binary_reader_create()
 */


#pragma once

#include <shader_compiler/defines.h>
#include <bufferlib/buffer.h>

/* structure which stores the current state of reading binary information */
typedef struct binary_reader_t
{
	/* cursor position (in bytse) */
	u32 read_cursor;
	/* pointer to contiguous bytes */
	const u8* data;
	/* size of the data (in bytes) */
	u32 size;
	/* stack for holding states */
	BUFFER state_stack;
} binary_reader_t;


BEGIN_CPP_COMPATIBLE

#define binary_reader_create_from_buffer(buffer) binary_reader_create(buf_get_ptr(buffer), buf_get_element_count(buffer))

/* constructors and destructors */
SC_API binary_reader_t* binary_reader_new();
SC_API binary_reader_t* binary_reader_create(const void* data, u32 size);
SC_API void binary_reader_destroy(binary_reader_t* reader);
SC_API void binary_reader_release_resources(binary_reader_t* reader);

/* readers */

/* reads and returns an unsigned integer of 8 bits in size */
SC_API u8 binary_reader_u8(binary_reader_t* reader);
/* reads and returns an unsigned itneger of 16 bits in size */
SC_API u16 binary_reader_u16(binary_reader_t* reader);
/* reads and returns an unsigned integer of 32 bits in size */
SC_API u32 binary_reader_u32(binary_reader_t* reader);
/* reads and returns an unsigned integer of 64 its in size */
SC_API u64 binary_reader_u64(binary_reader_t* reader);
/* reads and returns an signed integer of 8 bits in size */
SC_API s8 binary_reader_s8(binary_reader_t* reader);
/* reads and returns an signed integer of 16 bits in size */
SC_API s16 binary_reader_s16(binary_reader_t* reader);
/* reads and returns an signed integer of 32 bits in size */
SC_API s32 binary_reader_s32(binary_reader_t* reader);
/* reads and returns an signed integer of 64 bits in size */
SC_API s64 binary_reader_s64(binary_reader_t* reader);
/* reads and returns a single precision floating point number of 32 bits in size */
SC_API f32 binary_reader_f32(binary_reader_t* reader);
/* reads and returns a double precision floating point number of 64 bits in size */
SC_API f64 binary_reader_f64(binary_reader_t* reader);
/* reads and returns ptr to a null terminated string */
SC_API const char* binary_reader_str(binary_reader_t* reader);
/* returns pointer to the current read position */
SC_API const void* binary_reader_ptr(binary_reader_t* reader);
/* returns pointer index-th (zero based) byte */
SC_API const void* binary_reader_at(binary_reader_t* reader, u32 index);
/* returns pointer to the current read position and moves the read position by the read size */
#define binary_reader_read(reader, type) (type*)__binary_reader_read(reader, sizeof(type))
SC_API const void* __binary_reader_read(binary_reader_t* reader, u32 size);

/* resets the internal read cursor to its initial position, which is 0*/
SC_API void binary_reader_reset(binary_reader_t* reader);
/* sets the internal read cursor to 'pos' */
SC_API void binary_reader_jump(binary_reader_t* reader, u32 pos);
/* pushes the state of the reader on top of the state stack */
SC_API void binary_reader_push(binary_reader_t* reader);
/* pops out the state on top of the stack and applies it to the reader */
SC_API void binary_reader_pop(binary_reader_t* reader);

END_CPP_COMPATIBLE
