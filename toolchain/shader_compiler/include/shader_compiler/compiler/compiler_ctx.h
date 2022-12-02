#pragma once

#include <shader_compiler/defines.h>
#include <bufferlib/buffer.h>

/* pair of a pointer and u32; usually used for storing pointer to a list of elements and the number of elements in that list */
typedef struct ptr_u32_pair_t
{
	void* ptr;
	u32 size;
} ptr_u32_pair_t;

typedef ptr_u32_pair_t* look_ahead_table_t;
typedef ptr_u32_pair_t* symbol_qualifiers_table_t;
typedef ptr_u32_pair_t* symbol_attributes_table_t;

typedef struct compiler_ctx_t
{
	/* private/internal properties */

	/* table for looking the expected symbols in the current depth */
	look_ahead_table_t lat;
	/* number of elements in the look ahead table */
	u32 lat_size;
	/* table of symbol qualifiers */
	symbol_qualifiers_table_t sqt;
	/* number of elements in the symbol qualifiers table */
	u32 sqt_size;
	/* table of symbol attributes */
	symbol_attributes_table_t sat;
	/* number of elements in the symbol attribute table */
	u32 sat_size;
	/* list of keywords in the language grammar */
	char** keywords;
	/* number of elements in the keywords list */
	u32 keywords_size;
	/* current depth of a block while parsing the source */
	s32 depth;

	s32 current_pipeline_index;

	/* internal string buffer */
	BUFFER string_buffer;

	/* public properties */	

	/* ptr to the source string */
	const char* src;
	/* length (in bytes) of the above source string */
	u32 src_len;
	/* path to the source file*/
	const char* src_path;
	/* path to the vsc executable */
	const char* exe_path;
	/* list of paths to look for including the files in the standard brackets <> */
	BUFFER include_paths;
} compiler_ctx_t;

#define PTR_U32_NULL (ptr_u32_pair_t) { }

SC_API compiler_ctx_t* compiler_ctx_create();
SC_API void compiler_ctx_destroy(compiler_ctx_t* ctx);