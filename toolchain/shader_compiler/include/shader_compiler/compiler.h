
#pragma once

#include <shader_compiler/defines.h>
#include <calltrace/calltrace.h>
#include <stdint.h>
#include <bufferlib/buffer.h>

enum 
{
	SHADER_COMPILER_SHADER_STAGE_VERTEX = 0,		// vertex shader
	SHADER_COMPILER_SHADER_STAGE_TESSELLATION,		// tessellation shader
	SHADER_COMPILER_SHADER_STAGE_GEOMETRY,			// geometry shader
	SHADER_COMPILER_SHADER_STAGE_FRAGMENT,			// fragment shader
	SHADER_COMPILER_SHADER_STAGE_MAX
};

enum 
{
	SHADER_COMPILER_BLOCK = 0,		// enclosed in curly braces like uniform block or push constant block
	SHADER_COMPILER_FLOAT,			// float
	SHADER_COMPILER_INT,			// int
	SHADER_COMPILER_UINT,			// uint
	SHADER_COMPILER_DOUBLE,			// double
	SHADER_COMPILER_VEC4,			// vec4
	SHADER_COMPILER_VEC3,			// vec3
	SHADER_COMPILER_VEC2,			// vec2
	SHADER_COMPILER_IVEC4,			// ivec4
	SHADER_COMPILER_IVEC3,			// ivec3
	SHADER_COMPILER_IVEC2,			// ivec2
	SHADER_COMPILER_UVEC4,			// uvec4
	SHADER_COMPILER_UVEC3,			// uvec3
	SHADER_COMPILER_UVEC2,			// uvec2
	SHADER_COMPILER_MAT4,			// mat4
	SHADER_COMPILER_MAT3,			// mat3
	SHADER_COMPILER_MAT2,			// mat2
	SHADER_COMPILER_SAMPLER_2D,		// sampler2D
	SHADER_COMPILER_SAMPLER_3D,		// sampler3D
	SHADER_COMPILER_SAMPLER_CUBE,	// samplerCube
	SHADER_COMPILER_SUBPASS_INPUT, 	// subpassInput
	SHADER_COMPILER_MAX
};

#define shader_compiler_compile(...) define_alias_function_macro(shader_compiler_compile, __VA_ARGS__)
SC_API function_signature(BUFFER*, shader_compiler_compile, const char* source, buf_ucount_t length);

#define shader_compiler_load_and_compile(...) define_alias_function_macro(shader_compiler_load_and_compile, __VA_ARGS__)
SC_API function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path);
