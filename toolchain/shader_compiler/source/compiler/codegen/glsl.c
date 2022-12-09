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
	}
	return "Invalid-stage";
}

static void serialize_shader(shader_source_t* sources, u8 shader_count, codegen_buffer_t* writer, compiler_ctx_t* ctx);

SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer, compiler_ctx_t* ctx)
{
	const char* _start = start;
	
	shader_source_t* sources = CAST_TO(shader_source_t*, malloc(sizeof(shader_source_t) * SHADER_TYPE_MAX));
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

	serialize_shader(sources, shader_count, writer, ctx);
}

static char* resolve_relative_file_path(const char* src_path, u32 src_index, const char* cwd, BUFFER* buffer)
{
	if(src_index != U32_MAX)
		src_path = buf_get_ptr_at(buffer, src_index);
	u32 src_len = strlen(src_path);
	u32 cwd_len = strlen(cwd);

	/* buf_push_pseudo_get_ptr() */
	u32 index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, cwd_len + src_len + 1);
	char* file_path = CAST_TO(char*, buf_get_ptr_at(buffer, index));

	/* update src_path as the buffer has been resized (the internall memory buffer address has been) */
	if(src_index != U32_MAX)
		src_path = buf_get_ptr_at(buffer, src_index);
	
	memcpy(CAST_TO(void*, file_path), cwd,  cwd_len);
	file_path[cwd_len] = '/';
	memcpy(CAST_TO(void*, file_path + cwd_len + 1), src_path, src_len);
	file_path[cwd_len + src_len + 1] = 0;
	return file_path;
}

static char* merge_dir_and_file(const char* dir, const char* file, BUFFER* buffer)
{
	buf_clear(buffer, NULL);
	buf_push_string(buffer, dir);
	char* ptr = buf_peek_ptr(buffer);
	if((*ptr != '/') || (*ptr != '\\'))
		buf_push_char(buffer, '/');
	buf_push_string(buffer, file);
	buf_push_null(buffer);
	return buf_get_ptr(buffer);
}

static shaderc_include_result* resolve_include(void* user_data, const char* requested_source, int type, const char* requesting_source, size_t include_depth)
{
	compiler_ctx_t* ctx = CAST_TO(compiler_ctx_t*, user_data);
	shaderc_include_result* result = CAST_TO(shaderc_include_result*, malloc(sizeof(shaderc_include_result)));

	_assert((type >= shaderc_include_type_relative) && (type <= shaderc_include_type_standard));
	
	debug_log_info("[Codegen] [Legacy] Requested include file: %s", requested_source);

	char* file_path = NULL;

	BUFFER* data = NULL;
	BUFFER buffer = buf_new(char);

	switch(type)
	{
		case shaderc_include_type_relative:
		{
			/* if the requested_source is already an absolute file path */
			if(strchr(requested_source, ':') != NULL)
				file_path = CAST_TO(char*, requested_source);
			else
				file_path = resolve_relative_file_path(requested_source, U32_MAX, ctx->cwd, &buffer);
			data = load_text_from_file(file_path);
		}
		break;
		
		case shaderc_include_type_standard:
		{
			BUFFER* dirs = &ctx->include_paths;
			u32 dir_count = buf_get_element_count(dirs);
			for(u32 i = 0; i < dir_count; i++)
			{
				const char* dir = CAST_TO(char**, buf_get_ptr_at(dirs, i))[0];
				file_path = resolve_relative_file_path(merge_dir_and_file(dir, requested_source, &buffer), 0, ctx->cwd, &buffer);
				BUFFER* _data = load_text_from_file_s(file_path);
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
		result->source_name = "";
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
	}

	
	return result;
}

static void release_include(void* user_data, shaderc_include_result* include_result)
{
	free(include_result);
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
	binary_writer_u8_mark(writer->data, MARK_ID_SHADER_MASK + ctx->current_pipeline_index);
	u8 shader_mask = 0;
	for(u8 i = 0, j = 0; i < SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		shader_mask |= (1 << source.stage);
		_ASSERT((MARK_ID_SPIRV_LENGTH + j + ctx->current_pipeline_index * 2) < MARK_ID_SPIRV_LENGTH_MAX);
		_ASSERT((MARK_ID_SPIRV_OFFSET + j + ctx->current_pipeline_index * 2) < MARK_ID_SPIRV_OFFSET_MAX);
		binary_writer_u32_mark(writer->data, MARK_ID_SPIRV_LENGTH + j + ctx->current_pipeline_index * 2);
		binary_writer_u32_mark(writer->data, MARK_ID_SPIRV_OFFSET + j + ctx->current_pipeline_index * 2);
		j++;
	}

	/* write the shader mask */
	binary_writer_u8_set(writer->data, MARK_ID_SHADER_MASK + ctx->current_pipeline_index, shader_mask);

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
		binary_writer_u32_set(writer->data, MARK_ID_SPIRV_LENGTH + j + ctx->current_pipeline_index * 2, length);
		/* write offset */
		binary_writer_u32_set(writer->data, MARK_ID_SPIRV_OFFSET + j + ctx->current_pipeline_index * 2, binary_writer_pos(writer->data));
		/* write the SPIR-V code */
		binary_writer_write(writer->data, bytes, length);

		shaderc_result_release(result);
		j++;

		DEBUG_LOG_INFO("[Codegen] [Legacy] %s shader compilation success", stage_to_string(source.stage));
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
}
