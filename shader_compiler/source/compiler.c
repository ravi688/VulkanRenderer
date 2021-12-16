
#include <shader_compiler/compiler.h>
#include <shader_compiler/shaderc/shaderc.h>
#include <shader_compiler/assert.h> 	//assert
#include <disk_manager/file_reader.h> 	//load_text_from_file
#include <stdio.h> 						//fopen, fwrite
#include <string.h> 					//strncmp, strchr, strlen
#include <ctype.h>						//isspace

#define SHADER_BINARY_HEADER "SHADER BINARY"

#define PREPROCESSOR '#'
#define STAGE_DIRECTIVE "stage"

#define SHADER_STAGE_VERTEX_STR stage_strings[SHADER_COMPILER_SHADER_STAGE_VERTEX]
#define SHADER_STAGE_GEOMETRY_STR stage_strings[SHADER_COMPILER_SHADER_STAGE_GEOMETRY]
#define SHADER_STAGE_TESSELLATION_STR stage_strings[SHADER_COMPILER_SHADER_STAGE_TESSELLATION]
#define SHADER_STAGE_FRAGMENT_STR stage_strings[SHADER_COMPILER_SHADER_STAGE_FRAGMENT]

typedef struct shader_source_t
{
	const char* source;
	uint32_t length;
	uint8_t stage;
} shader_source_t;

#define generate_shader_binary(...) define_alias_function_macro(generate_shader_binary, __VA_ARGS__)
function_signature(static BUFFER*, generate_shader_binary, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count);

function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path)
{
	CALLTRACE_BEGIN();
	BUFFER* buffer = load_text_from_file(file_path);
	BUFFER* shader_binary = shader_compiler_compile(buffer->bytes, buffer->element_count - 1);
	buf_free(buffer);
	CALLTRACE_RETURN(shader_binary);
}

function_signature(BUFFER*, shader_compiler_compile, const char* _source, buf_ucount_t length)
{
	CALLTRACE_BEGIN();
	//Parse the _source string to separate vertex, tessellation, geometry, and fragment shaders
	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
	shader_source_t sources[SHADER_COMPILER_SHADER_STAGE_MAX]; memset(sources, 0, sizeof(shader_source_t) * SHADER_COMPILER_SHADER_STAGE_MAX);
	const char* source = _source;
	const char* temp = NULL;
	shader_source_t* previous_source = NULL;
	uint8_t shader_count = 0;
	while((source = strchr(source, PREPROCESSOR)) != NULL)
	{
		temp = source - 1;
		source += 1;
		while((*source == ' ') || (*source == '\t')) source++;
		if(strncmp(source, STAGE_DIRECTIVE, strlen(STAGE_DIRECTIVE)) != 0)
			continue;
		source += strlen(STAGE_DIRECTIVE);
		while((*source == ' ') || (*source == '\t')) source++;
		uint8_t count = 0;
		while(!isspace(*source)) { source++; count++; }
		if(count > strlen(SHADER_STAGE_TESSELLATION_STR))
		{
			LOG_FETAL_ERR(	"Shader parse error: Shader stage name must one of the followings:\n"
							"1. %s\n"
							"2. %s\n"
							"3. %s\n"
							"4. %s\n",
							SHADER_STAGE_VERTEX_STR,
							SHADER_STAGE_TESSELLATION_STR,
							SHADER_STAGE_GEOMETRY_STR,
							SHADER_STAGE_FRAGMENT_STR
						 );
		}
		if(previous_source != NULL)
			previous_source->length = temp - previous_source->source;
		source -= count;
		if(strncmp(source, SHADER_STAGE_VERTEX_STR, count) == 0)
		{
			//set vertex shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_VERTEX] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_VERTEX,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_VERTEX];
			shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_TESSELLATION_STR, count) == 0)
		{
			//set tessellation shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION];
			shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_GEOMETRY_STR, count) == 0)
		{
			//set geometry shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY];
			shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_FRAGMENT_STR, count) == 0)
		{
			//set fragment shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_FRAGMENT,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT];
			shader_count++;
			continue;
		}

		LOG_FETAL_ERR("Shader parse error: Shader stage not found\n");
	}
	if(previous_source != NULL)
		previous_source->length = _source + length - previous_source->source;
	CALLTRACE_RETURN(generate_shader_binary(stage_strings, sources, shader_count));
}


function_signature(static BUFFER*, generate_shader_binary, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count)
{
	CALLTRACE_BEGIN();
	BUFFER* shader_binary = BUFcreate(NULL, sizeof(uint8_t), 0, 0);

	//TODO: This should be like buf_pushv(shader_binary, SHADER_BINARY_HEADER, strlen(SHADER_BINARY_HEADER))
	//Write the shader binary header "SHADER BINARY"
	uint8_t header_len = strlen(SHADER_BINARY_HEADER);
	buf_resize(shader_binary, header_len);
	memcpy(shader_binary->bytes, SHADER_BINARY_HEADER, header_len);
	buf_set_element_count(shader_binary, header_len);

	uint8_t shader_mask = 0x00;
	for(uint8_t i = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		shader_mask |= (0x01 << source.stage);
	}
	//Write the shader mask 0 0 0 0  1 0 0 1 -> fragment & vertex shaders
	buf_push(shader_binary, &shader_mask);

	//Allocate space for offsets and lengths for each shader SPIRV binary
	uint64_t indices[shader_count];
	for(uint8_t i = 0; i < shader_count; i++)
	{
		indices[i] = buf_get_element_count(shader_binary);
		buf_push_pseudo(shader_binary, 8);
	}

	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	for(uint8_t i = 0, j = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		uint32_t shader_type = 0;
		switch(source.stage)
		{
			case SHADER_COMPILER_SHADER_STAGE_VERTEX: shader_type = shaderc_vertex_shader; break;
			case SHADER_COMPILER_SHADER_STAGE_TESSELLATION: shader_type = shaderc_tess_control_shader; break;
			case SHADER_COMPILER_SHADER_STAGE_GEOMETRY: shader_type = shaderc_geometry_shader; break;
			case SHADER_COMPILER_SHADER_STAGE_FRAGMENT: shader_type = shaderc_fragment_shader; break;
			default: LOG_FETAL_ERR("Shader parse error: stage \"%u\" is undefined or unsupported shader stage\n", source.stage); break;
		}
		shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source.source, source.length, shader_type, stage_strings[source.stage], "main", options);
		assert(result != NULL);
		if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
		{
			LOG_FETAL_ERR("Shader compile error: %s | %s\n", stage_strings[source.stage], shaderc_result_get_error_message(result));
		}

		void* bytes = (void*)shaderc_result_get_bytes(result);
		assert(bytes != NULL);
		uint64_t length = shaderc_result_get_length(result);
		assert(length > 0);
		uint32_t offset = buf_get_element_count(shader_binary);
		//Write offset
		memcpy(buf_get_ptr_at(shader_binary, indices[j]), &offset, 4);
		//Write length
		memcpy(buf_get_ptr_at(shader_binary, indices[j]) + 4, &length, 4);
		//Write SPIRV binary
		while(length > 0) { buf_push(shader_binary, bytes); bytes++; --length; }
		shaderc_result_release(result);
		j++;
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
	CALLTRACE_RETURN(shader_binary);
}
