
#pragma once

#include <renderer/debug.h>

typedef enum glsl_type_t
{
	GLSL_TYPE_U8 	= 1ULL,
	GLSL_TYPE_U16 	= 2ULL,
	GLSL_TYPE_U32	= 3ULL,
	GLSL_TYPE_U64 	= 4ULL,
	GLSL_TYPE_S8 	= 5ULL,
	GLSL_TYPE_S16 	= 6ULL,
	GLSL_TYPE_S32 	= 7ULL,
	GLSL_TYPE_S64 	= 8ULL,
	GLSL_TYPE_F32 	= 9ULL,
	GLSL_TYPE_F64 	= 10ULL,
	GLSL_TYPE_VEC2 	= 11ULL,
	GLSL_TYPE_VEC3 	= 12ULL,
	GLSL_TYPE_VEC4 	= 13ULL,
	GLSL_TYPE_MAT2 	= 14ULL,
	GLSL_TYPE_MAT3 	= 15ULL,
	GLSL_TYPE_MAT4	= 16ULL,

	GLSL_TYPE_IVEC2 = 17ULL,
	GLSL_TYPE_IVEC3 = 18ULL,
	GLSL_TYPE_IVEC4 = 19ULL,
	GLSL_TYPE_UVEC2 = 20ULL,
	GLSL_TYPE_UVEC3 = 21ULL,
	GLSL_TYPE_UVEC4 = 22ULL,

	GLSL_TYPE_BLOCK = 23ULL,
	GLSL_TYPE_SAMPLER_2D = 24ULL,
	GLSL_TYPE_SAMPLER_3D = 25ULL,
	GLSL_TYPE_SAMPLER_CUBE = 26ULL,
	GLSL_TYPE_SUBPASS_INPUT = 27ULL,

	GLSL_TYPE_FLOAT = GLSL_TYPE_F32,
	GLSL_TYPE_INT = GLSL_TYPE_S32,
	GLSL_TYPE_UINT = GLSL_TYPE_U32,
	GLSL_TYPE_DOUBLE = GLSL_TYPE_F64
} glsl_type_t;

/*
	From the spec:

	Standard Uniform Buffer Layout

	The 'base alignment' of the type of an OpTypeStruct member of is defined recursively as follows:

	A scalar of size N has a base alignment of N.
	A two-component vector, with components of size N, has a base alignment of 2 N.
	A three- or four-component vector, with components of size N, has a base alignment of 4 N.
	An array has a base alignment equal to the base alignment of its element type, rounded up to a multiple of 16.
	A structure has a base alignment equal to the largest base alignment of any of its members, rounded up to a multiple of 16.
	A row-major matrix of C columns has a base alignment equal to the base alignment of a vector of C matrix components.
	A column-major matrix has a base alignment equal to the base alignment of the matrix column type.
	The std140 layout in GLSL satisfies these rules.

 */
static u16 alignof_glsl_type(glsl_type_t type)
{
	switch(type)
	{
		case GLSL_TYPE_BLOCK 			: log_err("getting alignment of non glsl type \"GLSL_TYPE_BLOCK\" is a invalid operation\n"); return 0;
		case GLSL_TYPE_FLOAT 			:
		case GLSL_TYPE_INT 				:
		case GLSL_TYPE_UINT 			: return 4;
		case GLSL_TYPE_DOUBLE 			: log_err("\"double\" isn't supported yet\n"); return 0;
		case GLSL_TYPE_VEC4 			:
		case GLSL_TYPE_IVEC4 			:
		case GLSL_TYPE_UVEC4 			:
		case GLSL_TYPE_IVEC3 			:
		case GLSL_TYPE_UVEC3 			:
		case GLSL_TYPE_VEC3 			:
		case GLSL_TYPE_MAT4 			:
		case GLSL_TYPE_MAT3 			: return 16;
		case GLSL_TYPE_IVEC2 			:
		case GLSL_TYPE_UVEC2 			:
		case GLSL_TYPE_VEC2 			:
		case GLSL_TYPE_MAT2 			: return 8;
		case GLSL_TYPE_SAMPLER_2D 		:
		case GLSL_TYPE_SAMPLER_3D		: 
		case GLSL_TYPE_SAMPLER_CUBE		: log_err("getting alignment of opaque types are invalid operation\n"); return 0;
		default							: log_err("Unrecognized glsl type \"%u\"\n", type); return 0;
	};
}

static u16 sizeof_glsl_type(glsl_type_t type)
{
	switch(type)
	{
		case GLSL_TYPE_BLOCK 			: log_err("getting size of non glsl type \"GLSL_TYPE_BLOCK\" is a invalid operation\n"); return 0;
		case GLSL_TYPE_FLOAT 			:
		case GLSL_TYPE_INT 				:
		case GLSL_TYPE_UINT 			: return 4;
		case GLSL_TYPE_DOUBLE 			: log_err("\"double\" isn't supported yet\n"); return 0;
		case GLSL_TYPE_VEC4 			:
		case GLSL_TYPE_IVEC4 			:
		case GLSL_TYPE_UVEC4 			:
		case GLSL_TYPE_MAT2 			: return 16;
		case GLSL_TYPE_IVEC3 			:
		case GLSL_TYPE_UVEC3 			:
		case GLSL_TYPE_VEC3 			: return 12;
		case GLSL_TYPE_IVEC2 			:
		case GLSL_TYPE_UVEC2 			:
		case GLSL_TYPE_VEC2 			: return 8;
		case GLSL_TYPE_MAT4 			: return 64;
		case GLSL_TYPE_MAT3 			: return 36;
		case GLSL_TYPE_SAMPLER_2D 		:
		case GLSL_TYPE_SAMPLER_3D		: 
		case GLSL_TYPE_SAMPLER_CUBE		: log_err("getting size of opaque types are invalid operation\n"); return 0;
		default							: log_err("Unrecognized glsl type \"%u\"\n", type); return 0;
	};
}

#define GLSL_TYPE_U8_SIZE 		sizeof_glsl_type(GLSL_TYPE_U8)
#define GLSL_TYPE_U16_SIZE 		sizeof_glsl_type(GLSL_TYPE_U16)
#define GLSL_TYPE_U32_SIZE 		sizeof_glsl_type(GLSL_TYPE_U32)
#define GLSL_TYPE_U64_SIZE 		sizeof_glsl_type(GLSL_TYPE_U64)
#define GLSL_TYPE_S8_SIZE 		sizeof_glsl_type(GLSL_TYPE_S8)
#define GLSL_TYPE_S16_SIZE 		sizeof_glsl_type(GLSL_TYPE_S16)
#define GLSL_TYPE_S32_SIZE 		sizeof_glsl_type(GLSL_TYPE_S32)
#define GLSL_TYPE_S64_SIZE 		sizeof_glsl_type(GLSL_TYPE_S64)
#define GLSL_TYPE_F32_SIZE 		sizeof_glsl_type(GLSL_TYPE_F32)
#define GLSL_TYPE_F64_SIZE		sizeof_glsl_type(GLSL_TYPE_F64)
#define GLSL_TYPE_VEC2_SIZE		sizeof_glsl_type(GLSL_TYPE_VEC2)
#define GLSL_TYPE_VEC3_SIZE		sizeof_glsl_type(GLSL_TYPE_VEC3)
#define GLSL_TYPE_VEC4_SIZE		sizeof_glsl_type(GLSL_TYPE_VEC4)
#define GLSL_TYPE_MAT2_SIZE		sizeof_glsl_type(GLSL_TYPE_MAT2)
#define GLSL_TYPE_MAT3_SIZE		sizeof_glsl_type(GLSL_TYPE_MAT3)
#define GLSL_TYPE_MAT4_SIZE		sizeof_glsl_type(GLSL_TYPE_MAT4)
#define GLSL_TYPE_IVEC2_SIZE	sizeof_glsl_type(GLSL_TYPE_IVEC2)
#define GLSL_TYPE_IVEC3_SIZE	sizeof_glsl_type(GLSL_TYPE_IVEC3)
#define GLSL_TYPE_IVEC4_SIZE	sizeof_glsl_type(GLSL_TYPE_IVEC4)
#define GLSL_TYPE_UVEC2_SIZE	sizeof_glsl_type(GLSL_TYPE_UVEC2)
#define GLSL_TYPE_UVEC3_SIZE	sizeof_glsl_type(GLSL_TYPE_UVEC3)
#define GLSL_TYPE_UVEC4_SIZE	sizeof_glsl_type(GLSL_TYPE_UVEC4)
#define GLSL_TYPE_BLOCK_SIZE	sizeof_glsl_type(GLSL_TYPE_BLOCK)

#define GLSL_TYPE_FLOAT_SIZE GLSL_TYPE_F32_SIZE
#define GLSL_TYPE_INT_SIZE GLSL_TYPE_S32_SIZE
#define GLSL_TYPE_UINT_SIZE GLSL_TYPE_U32_SIZE
#define GLSL_TYPE_DOUBLE_SIZE GLSL_TYPE_F6_SIZE
