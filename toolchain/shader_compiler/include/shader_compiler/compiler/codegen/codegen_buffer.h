#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/utilities/binary_writer.h>
#include <bufferlib/buffer.h>

// marks in the binary_writer_t
enum
{
	MARK_ID_DESCRIPTOR_COUNT,
	MARK_ID_DESCRIPTOR_OFFSET,
	MARK_ID_IDENTIFIER_NAME,
	MARK_ID_FIELD_COUNT,
	MARK_ID_SPIRV_OFFSET,
	MARK_ID_MAX
};

enum
{
	MAGIC_NUM0 = 0,
	MAGIC_NUM1 = 3000,
	MAGIC_NUM2 = 6000
};


typedef struct codegen_buffer_t
{
	binary_writer_t* main;
	binary_writer_t* data;
	BUFFER* flatten;
} codegen_buffer_t;

/* constructors and destructors */
SC_API codegen_buffer_t* codegen_buffer_new();
SC_API codegen_buffer_t* codegen_buffer_create();
SC_API void codegen_destroy(codegen_buffer_t* buffer);
SC_API void codegen_release_resources(codegen_buffer_t* buffer);

/* flattens the codegen buffer and returns pointer to the BUFFER instance */
SC_API BUFFER* codegen_buffer_flatten(codegen_buffer_t* buffer);
