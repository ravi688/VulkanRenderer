/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: compiler.h is a part of VulkanRenderer

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

#pragma once

#include <shader_compiler/defines.h>
#include <common/allocation_callbacks.h>
#include <bufferlib/buffer.h>
#include <shader_compiler/sb_emitter.h>

/* enclosure structure to hold src string and include paths */
typedef struct sc_compiler_input_t
{
	/* pointer to ASCII characters in the source string data */
	const char* src;
	/* number of ASCII characters in the above source string data */
	u32 src_len;
	/* path to the source file*/
	const char* src_path;
	/* path to the vsc executable */
	const char* exe_path;
	/* current working directory */
	const char* cwd;
	/* list of paths to look for including the files in the standard brackets <> */
	const char** include_paths;
	/* number of 'include_path' strings */
	u32 include_path_count;
} sc_compiler_input_t;

/* enclosure structure to hold compiled shader binary (bytes) and */
typedef struct sc_compiler_output_t
{
	/* pointer to contiguous bytes representing the compiled shader binary 
	 * , it holds NULL, if 'is_success' holds 'false' value */
	void* sb_bytes;
	/* number of bytes in the above buffer */
	u32 sb_byte_count;
	/* null terminated log strings, NULL if no errors or warnings are occured while compiling the source file */
	const char* log;
	/* holds true if the compilation has succeeded with or without warnings; false if the compilation has been failed
	 * , in case of failure, the 'log' field holds pointer to the error description string */
	bool is_success;
} sc_compiler_output_t;

CAN_BE_UNUSED_FUNCTION static void sc_compiler_output_destroy(sc_compiler_output_t* output, com_allocation_callbacks_t* callbacks)
{
	AUTO _callbacks = (callbacks == NULL) ? com_allocation_callbacks_get_std() : *callbacks;
	if(output->sb_bytes != NULL)
		com_call_deallocate(&_callbacks, output->sb_bytes);
	if(output->log != NULL)
		com_call_deallocate(&_callbacks, CAST_TO(void*, output->log));
}

/* pair of a pointer and u32; usually used for storing pointer to a list of elements and the number of elements in that list */
typedef struct ptr_u32_pair_t
{
	void* ptr;
	u32 size;
} ptr_u32_pair_t;

typedef ptr_u32_pair_t* look_ahead_table_t;
typedef ptr_u32_pair_t* symbol_qualifiers_table_t;
typedef ptr_u32_pair_t* symbol_attributes_table_t;

typedef struct codegen_buffer_t codegen_buffer_t;
typedef struct sb_emitter_t sb_emitter_t;

typedef struct compiler_ctx_t
{
	/* private/internal properties */

	com_allocation_callbacks_t callbacks;

	/* binary buffer to write code generation data (final output) */
	codegen_buffer_t* codegen_buffer;

	/* shader binary emitter, which is used in shader binary generation for 2023 version */
	sb_emitter_t* emitter;

	/* table for looking the expected symbols after the current symbol */
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
	/* current symbol while syntax checking */
	s32 current_symbol;

	s32 current_pipeline_index;

	/* this is used in glsl.c file in the resolve_include function, it is used to determine if the allocation need to be freed */
	bool is_include_path_allocated;

	/* shader language version, assigned during header parsing, and later checked in parsing, etc. stages to adapt the compilation with 'sl_version' */
	u32 sl_version;
	/* shader binary version, assigned during header parsing, and later checked in code generation stage to make the binary compatible with 'sb_version' */
	u32 sb_version;

	/* ptr to the source string, it will be changed internally while compiling */
	const char* src;

	/* readonly pointer to the original sc_compiler_input_t object supplied by the user while calling sc_compile */
	const sc_compiler_input_t* input;
} compiler_ctx_t;

#define PTR_U32_NULL (ptr_u32_pair_t) { }
#define PTR_U32_IS_NULL(pair) (((pair)->ptr == NULL) && ((pair)->size == 0))

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SC_API compiler_ctx_t* compiler_ctx_create(com_allocation_callbacks_t* callbacks);
SC_API void compiler_ctx_destroy(compiler_ctx_t* ctx);

/*
	compiles a V3D shader source configured inside the compiler_ctx_t object
	ctx: compilation context (it also includes the source to be compiled)
	returns a pointer to a BUFFER object

	see: Documents/V3D/V3DShaderBinarySpec.docx for format of the compiled binary output.
 */
SC_API sc_compiler_output_t sc_compile(const sc_compiler_input_t* input, com_allocation_callbacks_t* callbacks);

END_CPP_COMPATIBLE
