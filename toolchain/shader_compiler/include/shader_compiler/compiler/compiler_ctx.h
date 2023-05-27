/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: compiler_ctx.h is a part of VulkanRenderer

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
	/* current working directory */
	const char* cwd;
	/* list of paths to look for including the files in the standard brackets <> */
	BUFFER include_paths;
} compiler_ctx_t;

#define PTR_U32_NULL (ptr_u32_pair_t) { }

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SC_API compiler_ctx_t* compiler_ctx_create();
SC_API void compiler_ctx_destroy(compiler_ctx_t* ctx);

END_CPP_COMPATIBLE
