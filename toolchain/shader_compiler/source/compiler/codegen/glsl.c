#include <shader_compiler/compiler/codegen/glsl.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <shader_compiler/shaderc/shaderc.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>
#include <phymac_parser/string.h>
#include <stdlib.h>
#include <string.h>

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

static void serialize_shader(shader_source_t* sources, u8 shader_count, codegen_buffer_t* writer);

SC_API void write_glsl(const char* start, const char* end, codegen_buffer_t* writer)
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
			if(previous_source->count > 1)
				debug_log_warning("[Codegen] [Legacy] More than once occurrences of the stage \"%s\"", stage_to_string(i));
			break;
		}
	}

	if(previous_source != NULL)
		previous_source->length = end - previous_source->source;

	/* calculate the number of unique shader stages present in the source */
	u32 shader_count = 0;
	for(u32 i = 0; i < SHADER_STAGE_MAX; i++)
		shader_count += min(1, sources[i].count);

	serialize_shader(sources, shader_count, writer);
}

static void serialize_shader(shader_source_t* sources, u8 shader_count, codegen_buffer_t* writer)
{
	/* calculate the shader mask */
	u8 shader_mask = 0;
	for(u8 i = 0; i < SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		shader_mask |= (1 << source.stage);
	}

	/* write the shader mask */
	binary_writer_u8(writer->main, shader_mask);

	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
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
			default: debug_log_error("[Codegen] [Legacy] stage \"%s\" is undefined or unsupported shader stage", stage_to_string(source.stage));
		}
		debug_log_info("GLSL source:\n-------------\n%.*s\n--------------", source.length, source.source);
		shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source.source, source.length, shader_type, stage_to_string(source.stage), "main", options);
		_assert(result != NULL);
		if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
			debug_log_error("[Codegen] [Legacy] GLSL to SPIR-V compilation failed:\n%s", shaderc_result_get_error_message(result));

		void* bytes = (void*)shaderc_result_get_bytes(result);
		_assert(bytes != NULL);
		u64 length = shaderc_result_get_length(result);
		_assert(length > 0);

		_assert((MARK_ID_SPIRV_OFFSET + i) < MARK_ID_SPIRV_OFFSET_MAX);

		/* write offset */
		binary_writer_u32_mark(writer->main, MARK_ID_SPIRV_OFFSET + i);
		binary_writer_u32_set(writer->main, MARK_ID_SPIRV_OFFSET + i, binary_writer_pos(writer->data));
		/* write length */
		binary_writer_u32(writer->main, length);
		/* write the SPIR-V code */
		binary_writer_write(writer->data, bytes, length);

		shaderc_result_release(result);
		j++;
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
}
