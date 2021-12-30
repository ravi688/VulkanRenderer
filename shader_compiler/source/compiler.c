
#include <shader_compiler/compiler.h>
#include <shader_compiler/shaderc/shaderc.h>
#include <shader_compiler/assert.h> 	//assert
#include <disk_manager/file_reader.h> 	//load_text_from_file
#include <stdio.h> 						//fopen, fwrite
#include <string.h> 					//strncmp, strchr, strlen
#include <ctype.h>						//isspace

#define log_u32(value) printf("%s: %u\n", #value, value)

#define SHADER_BINARY_HEADER "SHADER BINARY"

#define PREPROCESSOR '#'
#define STAGE_DIRECTIVE "stage"
#define SECTION_DIRECTIVE "section"

#define SECTION_SETTINGS "SETTINGS"
#define SECTION_LAYOUT "LAYOUT"
#define SECTION_SHADER "SHADER"

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

#define serialize_shader(...) define_alias_function_macro(serialize_shader, __VA_ARGS__)
function_signature(static void, serialize_shader, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count, BUFFER* buffer);
#define parse_shader(...) define_alias_function_macro(parse_shader, __VA_ARGS__)
function_signature(static const char*, parse_shader, const char* _source, buf_ucount_t length, BUFFER* buffer);
#define parse_layout(...) define_alias_function_macro(parse_layout, __VA_ARGS__)
function_signature(static const char*, parse_layout, const char* _source, buf_ucount_t length, BUFFER* buffer);
#define parse_settings(...) define_alias_function_macro(parse_settings, __VA_ARGS__)
function_signature(static const char*, parse_settings, const char* _source, buf_ucount_t length, BUFFER* buffer);

#define SHADER_PARSE_ERROR_MORE_THAN_ONE_SETTINGS_SECTION_FOUND "More than one settings section found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_LAYOUT_SECTION_FOUND "More than one layout section found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_SHADER_SECTION_FOUND "More than one shader section found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_VERTEX_SHADER_FOUND "More than one vertex shader found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_TESSELLATION_SHADER_FOUND "More than one tessellation shader found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_GEOMETRY_SHADER_FOUND "More than one geometry shader found"
#define SHADER_PARSE_ERROR_MORE_THAN_ONE_FRAGMENT_SHADER_FOUND "More than one fragment shader found"

inline static void shader_parse_error(const char* string)
{
	LOG_FETAL_ERR("shader parse error: %s\n", string);
}

enum
{
	SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_SHADER_STAGE,
	SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_DIRECTIVE,
	SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_DECLARATION,
	SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_SIZE_INVALID,
	SHADER_DATA_LAYOUT_PARSE_ERROR_ARRAY_SIZE_ZERO,
	SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE,
	SHADER_DATA_LAYOUT_PARSE_ERROR_UNRECOGNIZED_DATA_TYPE,
	SHADER_DATA_LAYOUT_PARSE_ERROR_SHADER_STAGE_NOT_FOUND
};
static void shader_data_layout_parse_error(u32 err, u32 len, const char* string);
static u32 get_word_length(const char* string, const char delimiter);
static const char* parse_shader_stage(const char* string, u16* out_mask);
static const char* parse_array_operator(const char* string, u32* out_data, u8 required);
static const char* parse_type(const char* string, u16 mask, BUFFER* buffer);
static const char* parse_storage_qualifiers(const char* string, u16* out_info);
static void buffer_write_string(BUFFER* buffer, const char* string, u32 len, bool null_terminate);
static void buffer_write_u8(BUFFER* buffer, u8 value);
static void buffer_write_u16(BUFFER* buffer, u16 value);
static void buffer_write_u32(BUFFER* buffer, u32 value);

function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path)
{
	CALLTRACE_BEGIN();
	BUFFER* buffer = load_text_from_file(file_path);
	BUFFER* shader_binary = shader_compiler_compile(buffer->bytes, buffer->element_count - 1);
	buf_free(buffer);
	CALLTRACE_RETURN(shader_binary);
}

/*
 		*------------------------INPUT STRING FORMAT------------------------------*

#section SETTINGS

#section LAYOUT

vertex[0, 0] uniform
{
    float time;
};

vertex[0, 1] uniform
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
};

fragment[0, 0] uniform sampler2D albedo_texture;
fragment[0, 1] uniform
{
    float time;
    vec3 light_dir;
    float light_intensity;
};

#section SHADER

#stage vertex

// vertex shader code goes here

#stage fragment

//fragment shader code goes here

#stage geometry

//geometry shader code goes here

#stage tessellation

//tessellation shader code goes here


 		*-------------------------------OUTPUT BINARY FORMAT---------------------------*

 Byte Index | Interpretation 			| Description
 ___________|___________________________|________________________________________
 0 			| string of length 13 bytes | "SHADER BINARY"
 			| section mask byte 		| BIT(0) = settings, BIT(1) = layout, BIT(2) = shader
 			| u32, settings offset 		| settings section offset if BIT(0) is set
 			| u32, layout offset 		| layout section offset if BIT(1) is set
 			| u32, shader offset 		| shader section offset if BIT(2) is set
 13 		| shader mask byte 			| BIT(0) = vertex shader, BIT(1) = tessellation shader, BIT(2) = geometry shader, BIT(3) = fragment shader
 14			| u32, contains a number 	| vertex shader offset if BIT(0) is set
 18			| u32, contains a number 	| vertex shader length if BIT(0) is set
 22			| u32, contains a number 	| tessellation shader offset if BIT(1) is set
 26			| u32, contains a number 	| tessellation shader length if BIT(1) is set
 30			| u32, contains a number 	| geometry shader offset if BIT(2) is set
 34			| u32, contains a number 	| geometry shader length if BIT(2) is set
 40			| u32, contains a number 	| fragment shader offset if BIT(3) is set
 44			| u32, contains a number 	| fragment shader length if BIT(3) is set
 */
function_signature(BUFFER*, shader_compiler_compile, const char* _source, buf_ucount_t length)
{
	CALLTRACE_BEGIN();

	BUFFER* buffer = BUFcreate(NULL, sizeof(uint8_t), 0, 0);

	//Write the shader binary header "SHADER BINARY" to the buffer, 13 BYTES
	buffer_write_string(buffer, (char*)SHADER_BINARY_HEADER, strlen(SHADER_BINARY_HEADER), false);

	//Reserve 1 BYTE for section mask
	buf_ucount_t section_mask_index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 1);

	//Store index for section offsets to insert at later
	buf_resize(buffer, buf_get_element_count(buffer) + 1);
	buf_ucount_t section_offsets_index = buf_get_element_count(buffer);

	const char* source = _source;
	const char* temp1 = source;
	const char* temp2 = source;
	func_ptr_sig(const char* , parse, const char* source, buf_ucount_t length, BUFFER* buffer) = NULL;
	u8 shader_section_count = 0;
	u8 layout_section_count = 0;
	u8 settings_section_count = 0;
	u8 section_mask = 0x00;
	while((source = strchr(source, PREPROCESSOR)) != NULL)
	{
		temp2 = source - 1;
		source += 1;
		while((*source == ' ') || (*source == '\t')) source++;
		if(strncmp(source, SECTION_DIRECTIVE, strlen(SECTION_DIRECTIVE)) != 0)
			continue;
		source += strlen(SECTION_DIRECTIVE);
		while((*source == ' ') || (*source == '\t')) source++;
		uint32_t count = 0;
		while(!isspace(*source)) { source++; count++; }
		source -= count;

		if(strncmp(source, SECTION_SETTINGS, count) == 0)
		{
			if(settings_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_SETTINGS_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n') + 1;
			if(source == NULL) break; //if the file ended
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buf_insert_pseudo(buffer, section_offsets_index, 4);
				*(u32*)buf_get_ptr_at(buffer, section_offsets_index) = buf_get_element_count(buffer);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer);
			}
			func_ptr(parse) = func_ptr(parse_settings);
			temp1 = __source;
			settings_section_count++;
			section_mask |= 1 << 0;
			continue;
		}
		else if(strncmp(source, SECTION_LAYOUT, count) == 0)
		{
			if(layout_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_LAYOUT_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n') + 1;
			if(source == NULL) break; //if the file ended
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buf_insert_pseudo(buffer, section_offsets_index, 4);
				*(u32*)buf_get_ptr_at(buffer, section_offsets_index) = buf_get_element_count(buffer);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer);
			}
			func_ptr(parse) = func_ptr(parse_layout);
			temp1 = __source;
			layout_section_count++;
			section_mask |= 1 << 1;
			continue;
		}
		else if(strncmp(source, SECTION_SHADER, count) == 0)
		{
			if(shader_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_SHADER_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n') + 1;
			if(source == NULL) break; //if the file ended
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buf_insert_pseudo(buffer, section_offsets_index, 4);
				*(u32*)buf_get_ptr_at(buffer, section_offsets_index) = buf_get_element_count(buffer);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer);
			}
			func_ptr(parse) = func_ptr(parse_shader);
			temp1 = __source;
			shader_section_count++;
			section_mask |= 1 << 2;
			continue;
		}
		LOG_FETAL_ERR("shader parse error: Unsupported section: %.*s\n", count, source);
	}

	if(func_ptr(parse) != NULL)
	{
		buf_insert_pseudo(buffer, section_offsets_index, 4);
		*(u32*)buf_get_ptr_at(buffer, section_offsets_index) = buf_get_element_count(buffer);
		source = func_ptr(parse) params(temp1, _source + length - temp1, buffer);
	}

	//Write section mask to the buffer, 1 BYTE
	*(u8*)buf_get_ptr_at(buffer, section_mask_index) = section_mask;

	//Reverse the offsets (array of u32)
	u8 section_count = layout_section_count + shader_section_count + settings_section_count;
	u32* offsets = (u32*)buf_get_ptr_at(buffer, section_offsets_index);
	for(u8 i = 0; i < (section_count >> 1); i++)
	{
		u32 temp = offsets[i];
		offsets[i] = offsets[section_count - i - 1];
		offsets[section_count - i - 1] = temp;
	}

	buf_fit(buffer);
	CALLTRACE_RETURN(buffer);
}

function_signature(static const char*, parse_settings, const char* _source, buf_ucount_t length, BUFFER* buffer)
{
	CALLTRACE_BEGIN();
	// serialize_settings();
	CALLTRACE_RETURN(_source + length);
}

/*
  		*---------------------------INPUT STRING FORMAT---------------------------*
vertex[0, 0] uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

fragment[0, 0] uniform sampler2D albedo_texture;
fragment vertex [0, 1] uniform SceneData
{
    float time;
    vec3 light_dir;
    float light_intensity;
} scene_data;

	*------------------------------OUTPUT BINARY FORMAT----------------------------*
 Byte Index 	| Interpretation			| Description
 _______________|___________________________|___________________________________________
 0 				| u16 						| descriptor count
 ...			| array of u32 				| offsets for each serialized descriptor
 0 				| u8 						| descriptor set number/index
 1 				| u8 						| descriptor binding number/index
 2 				| u16						| BIT(15) set if the type is an opaque type
 				| 							| BIT(14) set if the type is an uniform
  				| 							| BIT(13) = vertex shader, BIT(12) = fragment shader, BIT(11) = geometry shader, BIT(10) = tessellation shader
  				| 							| Lower 8 Bits contains the type information
 3 				| null terminated string 	| block name
 len0() + 4		| null terminated string 	| identifier name
 len01() + 5	| u16						| contains number of members in the block if the type is a block
 len01() + 7 	| u32 						| contains length of the array if the type is an array
 */
function_signature(static const char*, parse_layout, const char* _source, buf_ucount_t length, BUFFER* buffer)
{
	CALLTRACE_BEGIN();
	const char* string = _source;
	const char* end = string + length;

	//Reserve 2 BYTES for descriptor count in the buffer
	buf_ucount_t descriptor_count_index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 2);

	//Store the descriptor offsets index to insert at later
	buf_resize(buffer, buf_get_element_count(buffer) + 1);
	buf_ucount_t descriptor_offsets_index = buf_get_element_count(buffer);

	u16 descriptor_count = 0;
	while(string < end)
	{
		while(isspace(*string)) string++;
		if(string >= end) break; //end of the file reached
		u16 shader_mask = 0;
		u16 storage_mask = 0;
		u32 set_info[2] = { 0xFF, 0xFF };

		string = parse_shader_stage(string, &shader_mask);
		string = parse_array_operator(string, &set_info[0], 2);
		assert(set_info[0] < 0xFF);
		assert(set_info[1] < 0xFF);
		string = parse_storage_qualifiers(string, &storage_mask);

		//Write offset (byte count), 4 BYTES
		buf_insert_pseudo(buffer, descriptor_offsets_index, 4);
		*(u32*)buf_get_ptr_at(buffer, descriptor_offsets_index) = buf_get_element_count(buffer);
		log_u32(buf_get_element_count(buffer));

		//Write descriptor set number/index, 1 BYTE
		buffer_write_u8(buffer, (u8)set_info[0]);

		//Write descriptor binding number/index, 1 BYTE
		buffer_write_u8(buffer, (u8)set_info[1]);

		string = parse_type(string, storage_mask | shader_mask, buffer);
		descriptor_count++;
	}

	//Write number of descriptor count, 2 BYTES
	*(u16*)buf_get_ptr_at(buffer, descriptor_count_index) = descriptor_count;
	log_u32(descriptor_count);

	//Reverse the descriptor_offsets (array of u32)
	u32* descriptor_offsets = buf_get_ptr_at(buffer, descriptor_offsets_index);
	for(u16 i = 0; i < (descriptor_count >> 1); i++)
	{
		u32 temp = descriptor_offsets[i];
		descriptor_offsets[i] = descriptor_offsets[descriptor_count - i - 1];
		descriptor_offsets[descriptor_count - i - 1] = temp;
	}

	log_u32(descriptor_offsets_index);
	log_u32(*(u32*)buf_get_ptr_at(buffer, descriptor_offsets_index));
	log_u32(*(u32*)buf_get_ptr_at(buffer, descriptor_offsets_index + 4));
	log_u32(*(u32*)buf_get_ptr_at(buffer, descriptor_offsets_index + 8));

	CALLTRACE_RETURN(_source + length);
}

/*
	fragment[0, 0] uniform sampler2D albedo_texture;
	fragment vertex [0, 1] uniform SceneData
	{
	    float time;
	    vec3 light_dir;
	    float light_intensity;
	} scene_data;
 */
static const char* parse_type(const char* string, u16 mask, BUFFER* buffer)
{
	while(isspace(*string)) string++;
	u32 count = get_word_length(string, 0);
	bool found = true;
	u16 type;
	if(strncmp(string, "sampler2D", count) == 0) { type = (1 << 15) | (u16)SHADER_COMPILER_SAMPLER2D;  }
	else if(strncmp(string, "sampler3D", count) == 0) { type = (1 << 15) | (u16)SHADER_COMPILER_SAMPLER3D; }
	else if(strncmp(string, "vec4", count) == 0) { type = (u16)SHADER_COMPILER_VEC4; }
	else if(strncmp(string, "vec3", count) == 0) { type = (u16)SHADER_COMPILER_VEC3; }
	else if(strncmp(string, "vec2", count) == 0) { type = (u16)SHADER_COMPILER_VEC2; }
	else if(strncmp(string, "ivec4", count) == 0) { type = (u16)SHADER_COMPILER_IVEC4; }
	else if(strncmp(string, "ivec3", count) == 0) { type = (u16)SHADER_COMPILER_IVEC3; }
	else if(strncmp(string, "ivec2", count) == 0) { type = (u16)SHADER_COMPILER_IVEC2; }
	else if(strncmp(string, "uvec4", count) == 0) { type = (u16)SHADER_COMPILER_UVEC4; }
	else if(strncmp(string, "uvec3", count) == 0) { type = (u16)SHADER_COMPILER_UVEC3; }
	else if(strncmp(string, "uvec2", count) == 0) { type = (u16)SHADER_COMPILER_UVEC2; }
	else if(strncmp(string, "int", count) == 0) { type = (u16)SHADER_COMPILER_INT; }
	else if(strncmp(string, "uint", count) == 0) { type = (u16)SHADER_COMPILER_UINT; }
	else if(strncmp(string, "float", count) == 0) { type = (u16)SHADER_COMPILER_FLOAT; }
	else if(strncmp(string, "mat4", count) == 0) { type = (u16)SHADER_COMPILER_MAT4; }
	else if(strncmp(string, "mat3", count) == 0) { type = (u16)SHADER_COMPILER_MAT3; }
	else if(strncmp(string, "mat2", count) == 0) { type = (u16)SHADER_COMPILER_MAT2; }
	else
	{
		found = false;
		const char* block_name = string;
		string += count;
		while(*string != '{')
		{
			if(!isspace(*string))
			{
				count = get_word_length(string, 0);
				LOG_FETAL_ERR("shader layout parse error: Unexpected symbol \"%.*s\", '{' is expected\n", count, string);
			}
			string++;
		}
		string++;
		type = (u16)SHADER_COMPILER_BLOCK;

		//Write type info to the buffer, 2 BYTES
		buffer_write_u16(buffer, type | mask);

		//Write block name to the buffer, count + 1 BYTES
		buffer_write_string(buffer, block_name, count, true);

		//Reserve 2 BYTES for element count in the buffer
		buf_ucount_t index = buf_get_element_count(buffer);
		buf_push_pseudo(buffer, 2);

		u16 num_elements = 0;
		while(*string != '}')
		{
			u16 info;
			string = parse_type(string, mask, buffer);
			while(isspace(*string)) string++;
			num_elements++;
		}

		//Write number of elements to the buffer, 2 BYTES
		*(u16*)buf_get_ptr_at(buffer, index) = num_elements;

		string++;
	}

	if(found)
	{
		//Write type info to the buffer, 2 BYTES
		buffer_write_u16(buffer, type | mask);
		string += count;
	}

	while(isspace(*string)) string++;
	count = get_word_length(string, ';');

	//Write identifier name to the buffer, count + 1 BYTES
	buffer_write_string(buffer, string, count, true);
	printf("Identifier: %.*s\n", count, string);

	string += count;
	while(*string != ';')
	{
		if(!isspace(*string))
		{
			count = get_word_length(string, 0);
			LOG_FETAL_ERR("shader layout parse error: Expected ';' before \"%.*s\"\n", count, string);
		}
		string++;
	}
	string = strchr(string, ';') + 1;
	return string;
}

static const char* parse_storage_qualifiers(const char* string, u16* out_info)
{
	while(isspace(*string)) string++;
	u16 mask = 0;
	u32 count = get_word_length(string, 0);
	if(strncmp(string, "uniform", count) == 0)
		mask |= (1 << 14);
	else
		LOG_FETAL_ERR("shader data layout parse error: Unrecognized storage qualifier \"%.*s\"\n", count, string);
	return string + count;
}

static const char* parse_shader_stage(const char* string, u16* out_mask)
{
	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
	u32 count = 0;
	u16 mask = 0;
	u8 stage_count = 0;
	while(true)
	{
		while(isspace(*string)) string++;
		count = get_word_length(string, '[');
		if(count == 0) break;
		if(strncmp(string, SHADER_STAGE_VERTEX_STR, count) == 0)
			mask |= 1 << (13 - SHADER_COMPILER_SHADER_STAGE_VERTEX);
		else if(strncmp(string, SHADER_STAGE_TESSELLATION_STR, count) == 0)
			mask |= 1 << (13 - SHADER_COMPILER_SHADER_STAGE_TESSELLATION);
		else if(strncmp(string, SHADER_STAGE_GEOMETRY_STR, count) == 0)
			mask |= 1 << (13 - SHADER_COMPILER_SHADER_STAGE_GEOMETRY);
		else if(strncmp(string, SHADER_STAGE_FRAGMENT_STR, count) == 0)
			mask |= 1 << (13 - SHADER_COMPILER_SHADER_STAGE_FRAGMENT);
		else break;
		string += count;
		stage_count++;
	}
	if(stage_count == 0)
		LOG_FETAL_ERR("shader layout parse error: Unrecognized symbol \"%.*s\", expected shader stage!\n", count, string);
	*out_mask = mask;
	return string;
}

static const char* parse_array_operator(const char* string, u32* out_data, u8 required)
{
	while(isspace(*string)) string++;
	u8 count = 0;
	u32 arr_len = 0;
	if(strncmp(string, "[", 1) == 0)
	{
PARSE_NUMBER:
		string++;
		while(isspace(*string)) string++;
		u8 len = 0;
		while(isdigit(*string)) { string++; len++; };
		char sub_str[len + 1];
		strncpy(sub_str, string - len, len);
		arr_len = strtoul(sub_str, NULL, 0);
		out_data[count] = arr_len;
		log_u32(arr_len);
		count++;
		if(count > required)
			LOG_FETAL_ERR("shader layout parse error: More than required elements found in the array index operator, \"%u\"\n", arr_len);
		while(*string != ']')
		{
			if(*string == ',')
				goto PARSE_NUMBER;
			if(!isspace(*string))
				shader_data_layout_parse_error(SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE, 1, string + 1);
			string++;
		}
		string++;
	}
	if(count < required)
		LOG_FETAL_ERR("shader layout parse error: Less than required elements found in the array index operator, \"%u\"\n", arr_len);
	return string;
}

static u32 get_word_length(const char* string, const char delimiter)
{
	u32 count = 0;
	while((!isspace(*string)) && (delimiter != (*string))) { string++; count++; }
	return count;
}

static void shader_data_layout_parse_error(u32 err, u32 len, const char* string)
{
	switch(err)
	{
		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_SHADER_STAGE : LOG_FETAL_ERR("shader_data_layout parse error: Unsupported shader stage: \"%.*s\"\n", len, string - len); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNSUPPORTED_DIRECTIVE : LOG_FETAL_ERR("shader_data_layout parse error: Unsupported preprocessor directive \"%s\"\n", len, string - len); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_DECLARATION : LOG_FETAL_ERR("shader_data_layout parse error: Invalid array declaration\n"); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_ARRAY_SIZE_INVALID : LOG_FETAL_ERR("shader_data_layout parse error: Array size is invalid\n"); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_ARRAY_SIZE_ZERO : LOG_FETAL_ERR("shader_data_layout parse error: Array size can't be zero\n"); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE : LOG_FETAL_ERR("shader_data_layout parse error: Invalid symbol \"%.*s\" in array index operator\n", len, string - len); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_UNRECOGNIZED_DATA_TYPE : LOG_FETAL_ERR("shader_data_layout parse error: Unrecognised data type: \"%.*s\"\n", len, string - len); break;
		case SHADER_DATA_LAYOUT_PARSE_ERROR_SHADER_STAGE_NOT_FOUND: LOG_FETAL_ERR("shader_data_layout parse error: Shader stage not found\n");
		default: LOG_FETAL_ERR("shader_data_layout parse error: Unkown\n"); break;
	}
}

static void buffer_write_string(BUFFER* buffer, const char* string, u32 len, bool null_terminate)
{
	buf_pushv(buffer, (char*)string, len);
	if(!null_terminate)
		return;
	char null_char = 0;
	buf_push(buffer, &null_char);
}

static void buffer_write_u8(BUFFER* buffer, u8 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_push(buffer, &value);
}

static void buffer_write_u16(BUFFER* buffer, u16 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_ucount_t index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 2);
	*(u16*)buf_get_ptr_at(buffer, index) = value;
}

static void buffer_write_u32(BUFFER* buffer, u32 value)
{
	assert(buf_get_element_size(buffer) == 1);
	buf_ucount_t index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 4);
	*(u32*)buf_get_ptr_at(buffer, index) = value;
}

function_signature(static const char*, parse_shader, const char* _source, buf_ucount_t length, BUFFER* buffer)
{
	CALLTRACE_BEGIN();
	//Parse the _source string to separate vertex, tessellation, geometry, and fragment shaders
	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
	shader_source_t sources[SHADER_COMPILER_SHADER_STAGE_MAX]; memset(sources, 0, sizeof(shader_source_t) * SHADER_COMPILER_SHADER_STAGE_MAX);
	const char* source = _source;
	const char* temp = NULL;
	shader_source_t* previous_source = NULL;
	uint8_t shader_count = 0;
	u8 vertex_shader_count = 0;
	u8 tessellation_shader_count = 0;
	u8 geometry_shader_count = 0;
	u8 fragment_shader_count = 0;
	while((source = strchr(source, PREPROCESSOR)) != NULL)
	{
		temp = source - 1;
		source += 1;
		while((*source == ' ') || (*source == '\t')) source++;
		if(strncmp(source, STAGE_DIRECTIVE, strlen(STAGE_DIRECTIVE)) != 0)
			continue;
		source += strlen(STAGE_DIRECTIVE);
		while((*source == ' ') || (*source == '\t')) source++;
		uint32_t count = 0;
		while(!isspace(*source)) { source++; count++; }
		if(count > strlen(SHADER_STAGE_TESSELLATION_STR))
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
		if(previous_source != NULL)
			previous_source->length = temp - previous_source->source;
		source -= count;
		if(strncmp(source, SHADER_STAGE_VERTEX_STR, count) == 0)
		{
			if(vertex_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_VERTEX_SHADER_FOUND);
			//set vertex shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_VERTEX] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_VERTEX,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_VERTEX];
			shader_count++;
			vertex_shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_TESSELLATION_STR, count) == 0)
		{
			if(tessellation_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_TESSELLATION_SHADER_FOUND);
			//set tessellation shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION];
			shader_count++;
			tessellation_shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_GEOMETRY_STR, count) == 0)
		{
			if(geometry_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_GEOMETRY_SHADER_FOUND);
			//set geometry shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY];
			shader_count++;
			geometry_shader_count++;
			continue;
		}
		if(strncmp(source, SHADER_STAGE_FRAGMENT_STR, count) == 0)
		{
			if(fragment_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_FRAGMENT_SHADER_FOUND);
			//set fragment shader
			source = strchr(source, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT] = (shader_source_t)
			{
				.source = source,
				.stage = SHADER_COMPILER_SHADER_STAGE_FRAGMENT,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT];
			shader_count++;
			fragment_shader_count++;
			continue;
		}

		LOG_FETAL_ERR("Shader parse error: Shader stage not found\n");
	}
	if(previous_source != NULL)
		previous_source->length = _source + length - previous_source->source;
	serialize_shader(stage_strings, sources, shader_count, buffer);
	CALLTRACE_RETURN(_source + length);
}

function_signature(static void, serialize_shader, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count, BUFFER* buffer)
{
	CALLTRACE_BEGIN();

	uint8_t shader_mask = 0x00;
	for(uint8_t i = 0; i < SHADER_COMPILER_SHADER_STAGE_MAX; i++)
	{
		shader_source_t source = sources[i];
		if(source.length == 0) continue;
		shader_mask |= (0x01 << source.stage);
	}

	//Write the shader mask 0 0 0 0  1 0 0 1 -> fragment & vertex shaders
	buf_push(buffer, &shader_mask);

	//Allocate space for offsets and lengths for each shader SPIRV binary
	uint64_t indices[shader_count];
	for(uint8_t i = 0; i < shader_count; i++)
	{
		indices[i] = buf_get_element_count(buffer);
		buf_push_pseudo(buffer, 8);
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
			default: LOG_FETAL_ERR("Shader compile error: stage \"%u\" is undefined or unsupported shader stage\n", source.stage); break;
		}
		shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source.source, source.length, shader_type, stage_strings[source.stage], "main", options);
		assert(result != NULL);
		if(shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
			LOG_FETAL_ERR("Shader compile error: %s | %s\n", stage_strings[source.stage], shaderc_result_get_error_message(result));

		void* bytes = (void*)shaderc_result_get_bytes(result);
		assert(bytes != NULL);
		uint64_t length = shaderc_result_get_length(result);
		assert(length > 0);
		uint32_t offset = buf_get_element_count(buffer);

		//Write offset
		memcpy(buf_get_ptr_at(buffer, indices[j]), &offset, 4);

		//Write length
		memcpy(buf_get_ptr_at(buffer, indices[j]) + 4, &length, 4);

		//Write SPIRV binary
		while(length > 0) { buf_push(buffer, bytes); bytes++; --length; }

		shaderc_result_release(result);
		j++;
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
	CALLTRACE_END();
}
