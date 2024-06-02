/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: compiler_ctx.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/compiler/grammar.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>
#include <phymac_parser/v3d_generic.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/syntax.h>
#include <shader_compiler/compiler/codegen/codegen.h>
#include <shader_compiler/sb_emitter.h>

#include <stdlib.h>
#include <stdarg.h>

typedef ptr_u32_pair_t list_t;

static list_t new_u32_list(com_allocation_callbacks_t* callbacks, u32 count, ...)
{
	u32* ptr = CAST_TO(u32*, com_call_allocate(callbacks, count * sizeof(u32)));
	va_list args;
	va_start(args, count);
	for(u32 i = 0; i < count; i++)
		ptr[i] = va_arg(args, u32);
	return (list_t) { ptr, count };
}

SC_API compiler_ctx_t* compiler_ctx_create(com_allocation_callbacks_t* callbacks)
{
	/* if the callbacks supplied by the user is NULL, then use standard library malloc, realloc and free functions */
	AUTO _callbacks = (callbacks == NULL) ? com_allocation_callbacks_get_std() : *callbacks;

	compiler_ctx_t* ctx = CAST_TO(compiler_ctx_t*, com_call_allocate(&_callbacks, sizeof(compiler_ctx_t)));
	memset(ctx, 0, sizeof(compiler_ctx_t));

	/* cache pointers to the allocator functions */
	ctx->callbacks.allocate = _callbacks.allocate;
	ctx->callbacks.reallocate = _callbacks.reallocate;
	ctx->callbacks.deallocate = _callbacks.deallocate;

	/* prepare the look ahead table */
	ctx->lat_size = KEYWORD_MAX;
	ctx->lat = CAST_TO(ptr_u32_pair_t*, com_call_allocate(&ctx->callbacks, sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->lat[KEYWORD_UNDEFINED] = new_u32_list(&ctx->callbacks, 1, KEYWORD_SHADER);
	ctx->lat[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->lat[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->lat[KEYWORD_SHADER] = new_u32_list(&ctx->callbacks, 3, KEYWORD_PROPERTIES, KEYWORD_LAYOUT, KEYWORD_RENDERPASS);
	ctx->lat[KEYWORD_PROPERTIES] = PTR_U32_NULL;
	ctx->lat[KEYWORD_LAYOUT] = PTR_U32_NULL;
	ctx->lat[KEYWORD_RENDERPASS] = new_u32_list(&ctx->callbacks, 1, KEYWORD_SUBPASS);
	ctx->lat[KEYWORD_SUBPASS] = new_u32_list(&ctx->callbacks, 2, KEYWORD_GFXPIPELINE, KEYWORD_GLSL);
	ctx->lat[KEYWORD_GLSL] = PTR_U32_NULL;
	ctx->lat[KEYWORD_GFXPIPELINE] = PTR_U32_NULL;
	ctx->lat[KEYWORD_VEC4] = PTR_U32_NULL;
	ctx->lat[KEYWORD_VEC3] = PTR_U32_NULL;
	ctx->lat[KEYWORD_VEC2] = PTR_U32_NULL;

	/* prepare the symbol qualifiers table */
	ctx->sqt_size = KEYWORD_MAX;
	ctx->sqt = CAST_TO(ptr_u32_pair_t*, com_call_allocate(&ctx->callbacks, sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->sqt[KEYWORD_UNDEFINED] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->sqt[KEYWORD_SHADER] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_SHADER);
	ctx->sqt[KEYWORD_PROPERTIES] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_PROPERTIES);
	ctx->sqt[KEYWORD_LAYOUT] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_LAYOUT);
	ctx->sqt[KEYWORD_RENDERPASS] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_RENDERPASS);
	ctx->sqt[KEYWORD_SUBPASS] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_SUBPASS);
	ctx->sqt[KEYWORD_GLSL] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_GLSL);
	ctx->sqt[KEYWORD_GFXPIPELINE] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_GFXPIPELINE);
	ctx->sqt[KEYWORD_VEC4] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_VEC4);
	ctx->sqt[KEYWORD_VEC3] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_VEC3);
	ctx->sqt[KEYWORD_VEC2] = new_u32_list(&ctx->callbacks, 1, QUALIFIER_VEC2);

	/* prepare the symbol atributes table */
	ctx->sat_size = KEYWORD_MAX;
	ctx->sat = CAST_TO(ptr_u32_pair_t*, com_call_allocate(&ctx->callbacks, sizeof(ptr_u32_pair_t) * KEYWORD_MAX));
	ctx->sat[KEYWORD_UNDEFINED] = PTR_U32_NULL;
	ctx->sat[KEYWORD_TRUE] = PTR_U32_NULL;
	ctx->sat[KEYWORD_FALSE] = PTR_U32_NULL;
	ctx->sat[KEYWORD_SHADER] = new_u32_list(&ctx->callbacks, 1, ATTRIBUTE_NAME);
	ctx->sat[KEYWORD_PROPERTIES] = new_u32_list(&ctx->callbacks, 1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_LAYOUT] = new_u32_list(&ctx->callbacks, 1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_RENDERPASS] = new_u32_list(&ctx->callbacks, 2, ATTRIBUTE_READ, ATTRIBUTE_WRITE);
	ctx->sat[KEYWORD_SUBPASS] = new_u32_list(&ctx->callbacks, 2, ATTRIBUTE_READ, ATTRIBUTE_WRITE);
	ctx->sat[KEYWORD_GLSL] = new_u32_list(&ctx->callbacks, 1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_GFXPIPELINE] = new_u32_list(&ctx->callbacks, 1, ATTRIBUTE_NOPARSE);
	ctx->sat[KEYWORD_VEC4] = new_u32_list(&ctx->callbacks, 3, ATTRIBUTE_MESH_LAYOUT, ATTRIBUTE_RATE, ATTRIBUTE_ATTRIBUTE);
	ctx->sat[KEYWORD_VEC3] = new_u32_list(&ctx->callbacks, 3, ATTRIBUTE_MESH_LAYOUT, ATTRIBUTE_RATE, ATTRIBUTE_ATTRIBUTE);
	ctx->sat[KEYWORD_VEC2] = new_u32_list(&ctx->callbacks, 3, ATTRIBUTE_MESH_LAYOUT, ATTRIBUTE_RATE, ATTRIBUTE_ATTRIBUTE);

	ctx->keywords_size = sizeof(keywords) / sizeof(const char*);
	ctx->keywords = CAST_TO(char**, keywords);

	ctx->current_symbol = KEYWORD_UNDEFINED;
	ctx->current_pipeline_index = -1;

	ctx->codegen_buffer = codegen_buffer_create(&ctx->callbacks);
	ctx->emitter = sb_emitter_create(&ctx->callbacks, ctx->codegen_buffer, ctx->sb_version);

	return ctx;
}

SC_API void compiler_ctx_destroy(compiler_ctx_t* ctx)
{
	for(u32 i = 0; i < KEYWORD_MAX; i++)
		if(!PTR_U32_IS_NULL(&ctx->sqt[i]))
			com_call_deallocate(&ctx->callbacks, ctx->sqt[i].ptr);
	for(u32 i = 0; i < KEYWORD_MAX; i++)
		if(!PTR_U32_IS_NULL(&ctx->sat[i]))
			com_call_deallocate(&ctx->callbacks, ctx->sat[i].ptr);
	for(u32 i = 0; i < KEYWORD_MAX; i++)
		if(!PTR_U32_IS_NULL(&ctx->lat[i]))
			com_call_deallocate(&ctx->callbacks, ctx->lat[i].ptr);
	sb_emitter_destroy(ctx->emitter);
	codegen_buffer_destroy(ctx->codegen_buffer);
	codegen_buffer_release_resources(ctx->codegen_buffer);
	com_call_deallocate(&ctx->callbacks, ctx->lat);
	com_call_deallocate(&ctx->callbacks, ctx->sat);
	com_call_deallocate(&ctx->callbacks, ctx->sqt);
	com_call_deallocate(&ctx->callbacks, ctx);
}

SC_API sc_compiler_output_t sc_compile(const sc_compiler_input_t* input, com_allocation_callbacks_t* callbacks)
{
	compiler_ctx_t* ctx = compiler_ctx_create(callbacks);
	ctx->input = input;
	const char* start = input->src;
	// end should point to null character
	const char* end = start + input->src_len;
	char* result_log = remove_comments(callbacks, (char*)start, end);
	if(result_log != NULL)
	{
		sc_compiler_output_t output = 
		{
			.is_success = false,
			.log = result_log
		};
		return output;
	}

	// write the file header
	ctx->src = start = write_header(ctx, start, end);

	// parse the source code (perhaps partially)
	ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(callbacks, start, CAST_TO(u32, end - start));
	switch(result.result)
	{
		case PPSR_SUCCESS:
		{
			break;
		}
		case PPSR_WARNING:
		{
			debug_log_warning(result.log_buffer);
			break;
		}
		case PPSR_ERROR:
		{
			DEBUG_LOG_FETAL_ERROR(result.log_buffer);
			break;
		}
		case PPSR_ERROR_UNKOWN:
		{
			DEBUG_LOG_FETAL_ERROR("Unknown error has been occured while parsing the shader");
			break;
		}
		default:
		{
			DEBUG_LOG_FETAL_ERROR("Invalid or Unrecognized result code recieved from the parser");
			break;
		}
	}

	_ASSERT(result.root->child_count == 1);

	/* perform syntax checking */
	syntax(result.root->childs[0], ctx);
	/* generate code */
	codegen(result.root->childs[0], ctx);

	ppsr_v3d_generic_parse_result_destroy(callbacks, result);

	BUFFER* f_buffer = codegen_buffer_flatten(ctx->codegen_buffer);
	sc_compiler_output_t output = 
	{
		.sb_bytes = buf_get_ptr(f_buffer),
		.sb_byte_count = buf_get_element_count(f_buffer),
		.is_success = true
	};
	buf_free_except_data(f_buffer);

	compiler_ctx_destroy(ctx);
	return output;
}
