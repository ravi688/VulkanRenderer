
#pragma once

#include <shader_compiler/defines.h>
#include <calltrace/calltrace.h>
#include <stdint.h>
#include <bufferlib/buffer.h>

enum 
{
	SHADER_COMPILER_SHADER_STAGE_VERTEX = 0,
	SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
	SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
	SHADER_COMPILER_SHADER_STAGE_FRAGMENT,
	SHADER_COMPILER_SHADER_STAGE_MAX
};

enum 
{
	SHADER_COMPILER_BLOCK = 0,
	SHADER_COMPILER_FLOAT,
	SHADER_COMPILER_INT,
	SHADER_COMPILER_UINT,
	SHADER_COMPILER_DOUBLE,
	SHADER_COMPILER_VEC4,
	SHADER_COMPILER_VEC3,
	SHADER_COMPILER_VEC2,
	SHADER_COMPILER_IVEC4,
	SHADER_COMPILER_IVEC3,
	SHADER_COMPILER_IVEC2,
	SHADER_COMPILER_UVEC4,
	SHADER_COMPILER_UVEC3,
	SHADER_COMPILER_UVEC2,
	SHADER_COMPILER_MAT4,
	SHADER_COMPILER_MAT3,
	SHADER_COMPILER_MAT2,
	SHADER_COMPILER_SAMPLER_2D,
	SHADER_COMPILER_SAMPLER_3D,
	SHADER_COMPILER_SAMPLER_CUBE,
	SHADER_COMPILER_MAX
};

#define shader_compiler_compile(...) define_alias_function_macro(shader_compiler_compile, __VA_ARGS__)
SC_API function_signature(BUFFER*, shader_compiler_compile, const char* source, buf_ucount_t length);

#define shader_compiler_load_and_compile(...) define_alias_function_macro(shader_compiler_load_and_compile, __VA_ARGS__)
SC_API function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path);
