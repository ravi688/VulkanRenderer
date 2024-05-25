/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: diagnostic.h is a part of VulkanRenderer

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
#include <calltrace/calltrace.h>
#include <bufferlib/buffer.h>


typedef enum diagnostic_type_t
{
	DIAGNOSTIC_TYPE_PARSE_ERROR,			// for parse time errors, for example: failed to open file or unexpected end of file.
	DIAGNOSTIC_TYPE_PARSE_WARNING,			// for parse time warnings.
	DIAGNOSTIC_TYPE_PARSE_INFO,				// for parse time infos, for example: writing the intermediate outputs to a file for debugging purpose.
	
	DIAGNOSTIC_TYPE_SYNTAX_ERROR,			// for AST generation time errors, for example: plus sign used as an unary operator.
	DIAGNOSTIC_TYPE_SYNTAX_WARNING,			// for AST generation time warnings.
	DIAGNOSTIC_TYPE_SYNTAX_INFO,			// for AST generation time infos, for example: writing the intermediate outputs to a file for debugging purpose.
	
	DIAGNOSTIC_TYPE_SEMANTIC_ERROR,			// for semantic analysis time errors, for example: type mismatch or incorrect converstion.
	DIAGNOSTIC_TYPE_SEMANTIC_WARNING,		// for semantic analysis time warnings.
	DIAGNOSTIC_TYPE_SEMANTIC_INFO,			// for semantic analysis time infos, for example: writing the intermediate outputs to ta  file for debugging purpose.

	DIAGNOSTIC_TYPE_MAX
} diagnostic_type_t;

typedef enum diagnostic_message_t
{
	DIAGNOSTIC_MESSAGE_FAILED_TO_OPEN_FILE,			// the given file is not able to open
	DIAGNOSTIC_MESSAGE_UNEXPECTED_END_OF_FILE,		// the file has unexpected end of file
	DIAGNOSTIC_MESSAGE_UNEXPECTED_SYMBOL,
	DIAGNOSTIC_MESSAGE_MAX
} diagnostic_message_t;

typedef struct diagnostic_t
{
	BUFFER args;			// argument buffer for holding the pointer
} diagnostic_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SC_API diagnostic_t* diagnostic_new();
SC_API diagnostic_t* diagnostic_create();
SC_API void diagnostic_release_resources(diagnostic_t* diagnostic);
SC_API void diagnostic_destroy(diagnostic_t* diagnostic);

/* pushes a string on top of the argument stack */
SC_API void diagnostic_push_string(diagnostic_t* diagnostic, const char* value);
/* pushes a contiguous array of bytes pointed by 'ptr' of size 'size' on top of the argument stack */
SC_API void diagnostic_push(diagnostic_t* diagnostic, const void* const ptr, u32 size);

/* spits out the log message with appropriate diagnostic cateogry and diagnostic message */
#define diagnostic_log(...) __diagnostic_log(__VA_ARGS__, __LINE__, __FUNCTION__, __FILE__)
SC_API void __diagnostic_log(diagnostic_t* diagnostic, diagnostic_type_t type, diagnostic_message_t message, u32 __line__, const char* __function__, const char* __file__);

END_CPP_COMPATIBLE
