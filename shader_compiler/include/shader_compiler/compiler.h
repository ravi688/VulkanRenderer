
#pragma once

#include <calltrace/calltrace.h>
#include <stdint.h>
#include <buffer.h>

enum 
{
	SHADER_COMPILER_SHADER_STAGE_VERTEX = 0,
	SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
	SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
	SHADER_COMPILER_SHADER_STAGE_FRAGMENT,
	SHADER_COMPILER_SHADER_STAGE_MAX
};

#define shader_compiler_compile(...) define_alias_function_macro(shader_compiler_compile, __VA_ARGS__)
function_signature(BUFFER*, shader_compiler_compile, const char* source, buf_ucount_t length);

#define shader_compiler_load_and_compile(...) define_alias_function_macro(shader_compiler_load_and_compile, __VA_ARGS__)
function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path);
