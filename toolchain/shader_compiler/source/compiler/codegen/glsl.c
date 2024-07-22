/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: glsl.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/codegen/glsl.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <shader_compiler/shaderc/shaderc.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>
#include <phymac_parser/string.h>
#include <disk_manager/file_reader.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h> 	// debug purpose, remove it

typedef enum shader_type_t
{
	SHADER_TYPE_VERTEX = 0,
	SHADER_TYPE_FRAGMENT = 1,
	SHADER_TYPE_GEOMETRY = 2,
	SHADER_TYPE_TESSELLATION = 3,
	SHADER_TYPE_MAX,

	SHADER_STAGE_VERTEX = SHADER_TYPE_VERTEX,
	SHADER_STAGE_FRAGMENT = SHADER_TYPE_FRAGMENT,
	SHADER_STAGE_GEOMETRY = SHADER_TYPE_GEOMETRY,
	SHADER_STAGE_TESSELLATION = SHADER_TYPE_TESSELLATION,
	SHADER_STAGE_MAX = SHADER_TYPE_MAX

} shader_type_t;

typedef shader_type_t shader_stage_t;

typedef struct shader_source_t
{
	/* pointer to the source string  */
	const char* source;
	/* length of the string without null termination character */
	u32 length;
	/* number of occurences of this stage (used for diagnostic message) */
	u32 count;
	/* stage of this shader */
	shader_stage_t stage;
} shader_source_t;

static const char* stage_to_string(shader_stage_t stage)
{
	switch(stage)
	{
		case SHADER_STAGE_VERTEX:
			return "vertex";
		case SHADER_STAGE_FRAGMENT:
			return "fragment";
		case SHADER_STAGE_GEOMETRY:
			return "geometry";
		case SHADER_STAGE_TESSELLATION:
			return "tessellation";
		default:
			return "Invalid-stage";
	}
}

static void serialize_shader(shader_source_t* sources, u8 shader_count, codegen_buffer_t* writer, compiler_ctx_t* ctx);

SC_API void write_glsl(const char* start, const char* end, compiler_ctx_t* ctx)
{
	CAN_BE_UNUSED_VARIABLE const char* _start = start;
	
	shader_source_t* sources = CAST_TO(shader_source_t*, com_call_allocate(&ctx->callbacks, sizeof(shader_source_t) * SHADER_TYPE_MAX));
	memset(sources, 0, sizeof(shader_source_t) * SHADER_TYPE_MAX);
	
	const char* temp = NULL;
	shader_source_t* previous_source = NULL;
	while(((start = strchr(start, '#')) != NULL) && (start < end))
	{
		temp = start - 1;
		start += 1;
		start = skip(start, " \t", end);
		if(safe_strncmp(start, "stage", 5) != 0)
			continue;
		if(previous_source != NULL)
			previous_source->length = temp - previous_source->source;
		start += 5;
		start = skip(start, " \t", end);
		u32 len = get_token_length(start, end);

		for(u32 i = 0; i < SHADER_STAGE_MAX; i++)
		{
			if(safe_strncmp(start, stage_to_string(i), len) != 0) continue;

			start = strchr(start, '\n') + 1;
			previous_source = &sources[i];
			previous_source->source = start;
			previous_source->stage = i;
			previous_source->count++;

			DEBUG_BLOCK
			(
				if(previous_source->count > 1)
					debug_log_warning("[Codegen] [Legacy] More than once occurrences of the stage \"%s\"", stage_to_string(i));
			)
			break;
		}
	}

	if(previous_source != NULL)
		previous_source->length = end - previous_source->source;

	/* calculate the number of unique shader stages present in the source */
	u32 shader_count = 0;
	for(u32 i = 0; i < SHADER_STAGE_MAX; i++)
		shader_count += min(1, sources[i].count);

	serialize_shader(sources, shader_count, ctx->codegen_buffer, ctx);

	com_call_deallocate(&ctx->callbacks, sources);
}

static char* resolve_relative_file_path(const char* src_path, const char* cwd, com_allocation_callbacks_t* callbacks)
{
	u32 src_len = strlen(src_path);
	u32 cwd_len = strlen(cwd);

	/* buf_push_pseudo_get_ptr() */
	buffer_t buffer = buf_create_with_callbacks(callbacks, sizeof(char), 128, 0);
	buf_push_pseudo(&buffer, cwd_len + src_len + 1);
	char* file_path = CAST_TO(char*, buf_get_ptr(&buffer));

	memcpy(CAST_TO(void*, file_path), cwd,  cwd_len);
	file_path[cwd_len] = '/';
	memcpy(CAST_TO(void*, file_path + cwd_len + 1), src_path, src_len);
	file_path[cwd_len + src_len + 1] = 0;
	return file_path;
}

static char* merge_dir_and_file(const char* dir, const char* file, com_allocation_callbacks_t* callbacks)
{
	buffer_t buffer = buf_create_with_callbacks(callbacks, sizeof(char), 128, 0);
	buf_push_string(&buffer, dir);
	char* ptr = buf_peek_ptr(&buffer);
	if((*ptr != '/') || (*ptr != '\\'))
		buf_push_char(&buffer, '/');
	buf_push_string(&buffer, file);
	buf_push_null(&buffer);
	buf_fit(&buffer);
	return buf_get_ptr(&buffer);
}

static buffer_t* load_file(compiler_ctx_t* ctx, const char* file_path)
{
	if(ctx->file_load_callback != NULL)
		return ctx->file_load_callback(file_path);
	else return load_text_from_file_s(file_path);
}

static shaderc_include_result* resolve_include(void* user_data, const char* requested_source, int type, const char* requesting_source, size_t include_depth)
{
	compiler_ctx_t* ctx = CAST_TO(compiler_ctx_t*, user_data);
	shaderc_include_result* result = com_allocate_obj_init(&ctx->callbacks, shaderc_include_result);
	memset(result, 0, sizeof(shaderc_include_result));

	_assert((type >= shaderc_include_type_relative) && (type <= shaderc_include_type_standard));
	
	debug_log_info("[Codegen] [Legacy] Requested include file: %s", requested_source);

	char* file_path = NULL;

	BUFFER* data = NULL;

	switch(type)
	{
		case shaderc_include_type_relative:
		{
			/* if the requested_source is already an absolute file path */
			if(strchr(requested_source, ':') != NULL)
				file_path = CAST_TO(char*, requested_source);
			else
			{
				file_path = resolve_relative_file_path(requested_source, ctx->input->cwd, &ctx->callbacks);
				ctx->is_include_path_allocated = true;
			}
			data = load_file(ctx, file_path);
			if(data == NULL)
				debug_log_error("[Codegen] [Legacy] Failed to load %s", file_path);
		}
		break;
		
		case shaderc_include_type_standard:
		{
			for(u32 i = 0; i < ctx->input->include_path_count; i++)
			{
				const char* dir = ctx->input->include_paths[i];
				char* merged_path = merge_dir_and_file(dir, requested_source, &ctx->callbacks);
				file_path = resolve_relative_file_path(merged_path, ctx->input->cwd, &ctx->callbacks);
				com_call_deallocate(&ctx->callbacks, merged_path);
				ctx->is_include_path_allocated = true;
				BUFFER* _data = load_file(ctx, file_path);
				debug_log_info("[Codegen] [Legacy] Resolved include file: %s", file_path);
				if(_data == NULL)
					continue;
				else 
				{
					data = _data;
					break;
				}
			}
		}
		break;
	}

	if(data == NULL)
	{
		result->source_name = NULL;
		result->source_name_length = 0;
		result->content = "Include Error";
		result->content_length = strlen(result->content);
	}
	else
	{
		result->source_name = file_path;
		result->source_name_length = strlen(file_path);
		result->content = buf_get_ptr(data);
		result->content_length = buf_get_element_count(data) - 1;
		buf_free_except_data(data);
	}

	
	return result;
}

static void release_include(void* user_data, shaderc_include_result* include_result)
{
	AUTO ctx = CAST_TO(compiler_ctx_t*, user_data);
	if(ctx->is_include_path_allocated)
	{
		com_call_deallocate(&ctx->callbacks, CAST_TO(void*, include_result->source_name));
		ctx->is_include_path_allocated = false;
	}
	if((include_result->content != NULL) && (strcmp(include_result->content, "Include Error") != 0))
		free(CAST_TO(void*, include_result->content));
	com_call_deallocate(&ctx->callbacks, include_result);
}

static shaderc_compile_options_t get_compile_options(compiler_ctx_t* ctx)
{
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	shaderc_compile_options_set_include_callbacks(options, resolve_include, release_include, ctx);
	return options;
}

static void serialize_shader(shader_source_t* sources, u8 shader_count, codegen_buffer_t* writer, compiler_ctx_t* ctx)
{
	/* calculate the shader mask */
	_ASSERT((MARK_ID_SHADER_MASK + ctx->current_pipeline_index) < MARK_ID_SHADER_MASK_MAX);
	AUTO shader_mask_addr = codegen_buffer_alloc_u8(writer, ".data");
	u8 shader_mask = 0;
	codegen_buffer_address_t length_offset_addrs[SHADER_STAGE_MAX * 2];
	for(u8 i = 0, j = 0; i < SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		shader_mask |= (1 << source.stage);
		length_offset_addrs[j * 2 + 0] = codegen_buffer_alloc_u32(writer, ".data");
		length_offset_addrs[j * 2 + 1] = codegen_buffer_alloc_pointer(writer, ".data");
		j++;
	}

	/* write the shader mask */
	codegen_buffer_set_u8(writer, shader_mask_addr, shader_mask);

	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = get_compile_options(ctx);
	for(u8 i = 0, j = 0; i < SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		u32 shader_type = 0;
		switch(source.stage)
		{
			case SHADER_STAGE_VERTEX: 		{ shader_type = shaderc_vertex_shader; 			break; }
			case SHADER_STAGE_TESSELLATION: { shader_type = shaderc_tess_control_shader; 	break; }
			case SHADER_STAGE_GEOMETRY: 	{ shader_type = shaderc_geometry_shader; 		break; }
			case SHADER_STAGE_FRAGMENT: 	{ shader_type = shaderc_fragment_shader; 		break; }
			default: DEBUG_LOG_ERROR("[Codegen] [Legacy] stage \"%s\" is undefined or unsupported shader stage", stage_to_string(source.stage));
		}
		debug_log_info("GLSL source:\n-------------\n%.*s\n--------------", source.length, source.source);
		shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source.source, source.length, shader_type, stage_to_string(source.stage), "main", options);
		_ASSERT(result != NULL);
		if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
		{
			DEBUG_LOG_ERROR("[Codegen] [Legacy] GLSL to SPIR-V compilation failed:\n%s", shaderc_result_get_error_message(result));
			DEBUG_LOG_ERROR("[Codegen] [Legacy] Compilation Aborted");
			return;
		}

		void* bytes = (void*)shaderc_result_get_bytes(result);
		_ASSERT(bytes != NULL);
		u64 length = shaderc_result_get_length(result);
		_ASSERT(length > 0);

		/* write length */
		codegen_buffer_set_u32(writer, length_offset_addrs[j * 2 + 0], length);
		/* write offset */
		AUTO addr = codegen_buffer_get_end_address(writer, ".data");
		codegen_buffer_set_pointer(writer, length_offset_addrs[j * 2 + 1], addr);
		/* write the SPIR-V code */
		codegen_buffer_write(writer, ".data", bytes, length);

		shaderc_result_release(result);
		j++;

		DEBUG_LOG_INFO("[Codegen] [Legacy] %s shader compilation success", stage_to_string(source.stage));
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
}
