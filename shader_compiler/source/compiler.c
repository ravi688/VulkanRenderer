
#include <shader_compiler/compiler.h>
#include <shader_compiler/settings_parser.h> 		// parse_settings
#include <shader_compiler/shaderc/shaderc.h>
#include <shader_compiler/assert.h> 	//assert
#include <disk_manager/file_reader.h> 	//load_text_from_file
#include <stdio.h> 						//fopen, fwrite
#include <shader_compiler/string_utilities.h>

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
static function_signature(void, serialize_shader, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count, BUFFER* buffer, BUFFER* offsets_buffer);
#define parse_shader(...) define_alias_function_macro(parse_shader, __VA_ARGS__)
static function_signature(const char*, parse_shader, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer);
#define parse_layout(...) define_alias_function_macro(parse_layout, __VA_ARGS__)
static function_signature(const char*, parse_layout, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer);

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
static const char* parse_shader_stage(const char* string, u32* out_mask);
static const char* parse_array_operator(const char* string, u32* out_data, u8 required);
static const char* parse_square_brackets(const char* string, bool* is_found, u32* layout_info_mask);
static const char* parse_type(const char* string, u32 mask, BUFFER* buffer, BUFFER* offsets_buffer);
static const char* parse_storage_qualifiers(const char* string, u32* out_info);
static void buffer_write_string(BUFFER* buffer, const char* string, u32 len, bool null_terminate);
static void buffer_write_u8(BUFFER* buffer, u8 value);
static void buffer_write_u16(BUFFER* buffer, u16 value);
static void buffer_write_u32(BUFFER* buffer, u32 value);
static void buffer_insert_offset(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index);
static void buffer_insert_bytes(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index, const u8* bytes, u32 count);

SC_API function_signature(BUFFER*, shader_compiler_load_and_compile, const char* file_path)
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

vertex [push_constant] uniform Push
{
	mat4 mvp_matrix; 		// 64 bytes
	mat4 model_matrix; 		// 64 bytes
} push;

vertex[0, 0] uniform Data
{
    float time;
} data;

vertex[0, 1] uniform Matrices
{
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;
} matrices;

fragment[0, 0] uniform Sampler2D albedo_texture;
fragment[0, 1] uniform Light
{
    float time;
    vec3 light_dir;
    float light_intensity;
} light;

#section SHADER

#stage vertex

// vertex shader code goes here

#stage fragment

//fragment shader code goes here

#stage geometry

//geometry shader code goes here

#stage tessellation

//tessellation shader code goes here

 0b 0000 0000 0101 0100
 0x  54 

 		*-------------------------------OUTPUT BINARY FORMAT---------------------------*

 Byte Index | Interpretation 			| Description
 ___________|___________________________|________________________________________
 0 			| string of length 13 bytes | "SHADER BINARY"
 			| u32, section masks  		| 32 bits are divided into 3 bits group with BIT(0) = settings, BIT(1) = layout, BIT(2) = shader
 			| u32, settings offset 		| settings section offset if BIT(0) is set
 			| u32, layout offset 		| layout section offset if BIT(1) is set
 			| u32, shader offset 		| shader section offset if BIT(2) is set
____________|___________________________|________________________________________
________________________________SETTINGS_________________________________________
			| VkPrimitiveTopology 		| topology
			| VkBool32 					| primitiveRestartEnable
------------|---------------------------|----------------------------------------
			| uint32_t 					| patchControlPoints
------------|---------------------------|----------------------------------------
			| uint32_t 					| viewportCount
			| array of VkViewport 		| viewports
			| uint32_t 					| scissorCount
			| array of VkRect2D 		| scissors
------------|---------------------------|----------------------------------------
			| VkBool32 					| depthClampEnable
			| VkBool32 					| rasterizerDiscardEnable
			| VkPolygonMode 			| polygonMode
			| VkCullModeFlags 			| cullMode
			| VkFrontFace 				| frontFace
			| VkBool32 					| depthBiasEnable
			| float 					| depthBiasConstantFactor
			| float 					| depthBiasClamp
			| float 					| depthBiasSlopeFactor
			| float 					| lineWidth
------------|---------------------------|----------------------------------------
			| VkSampleCountFlagBits 	| rasterizationSamples
			| VkBool32 					| sampleShadingEnable
			| float 					| minSampleShading
			| VkBool32 					| alphaToCoverageEnable
			| VkBool32 					| alphaToOneEnable
------------|---------------------------|----------------------------------------
			| VkBool32 					| depthTestEnable
			| VkBool32 					| depthWriteEnable
			| VkCompareOp 				| depthCompareOp
			| VkBool32 					| depthBoundsTestEnable
			| VkBool32 					| stencilTestEnable
			| VkStencilOpState 			| front
			| VkStencilOpState 			| back
			| float 					| minDepthBounds
			| float 					| maxDepthBounds
------------|--------------------------------------------------------------------
			| VkBool32 										| logicOpEnable
			| VkLogicOp 									| logicOp
			| uint32_t 										| attachmentCount
			| array of VkPipelineColorBlendAttachmentState 	| attachments
			| float[4] 										| blendConstants[4]
------------|-----------------------------------------------|--------------------
			| uint32_t 										| dynamicStateCount
			| array of VkDynamicState 						| dynamicStates
____________|_______________________________________________|____________________
________________________________LAYOUT___________________________________________
 0 			| u16 						| descriptor count + attribute count
 ...		| array of u32 				| offsets for each serialized descriptor
 0 			| u8 						| descriptor set number/index or vertex attribute binding number, if the type is push_constant then it would be the offset
 1 			| u8 						| descriptor binding number/index or vertex attribute layout number, if the type is push_constant then it would be 0xFF
 2 			| u32 						| BIT(18) set if the type is a per-vertex attribute
 			|							| BIT(17) set if the type is a per-instance attribute
  			| 							| BIT(16) set if the type is a push_constant
  			| 							| BIT(15) set if the type is an opaque type
 			| 							| BIT(14) set if the type is an uniform
  			| 							| BIT(13) = vertex shader, BIT(12) = tessellation shader, BIT(11) = geometry shader, BIT(10) = fragment shader
  			| 							| Lower 8 Bits contains the type information
			| null terminated string 	| if the type is block then it would be block name
 			| null terminated string 	| identifier name
 			| u16						| contains number of members in the block if the type is a block
 			| u32 						| contains length of the array if the type is an array
____________|___________________________|_________________________________________
________________________________SHADER____________________________________________
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
SC_API function_signature(BUFFER*, shader_compiler_compile, const char* _source, buf_ucount_t length)
{
	CALLTRACE_BEGIN();

	remove_comments((char*)_source, length);

	BUFFER* buffer = BUFcreate(NULL, sizeof(uint8_t), 0, 0);
	BUFFER* offsets_buffer = BUFcreate(NULL, sizeof(buf_ucount_t), 0, 0);

	//Write the shader binary header "SHADER BINARY" to the buffer, 13 BYTES
	buffer_write_string(buffer, (char*)SHADER_BINARY_HEADER, strlen(SHADER_BINARY_HEADER), false);

	//Reserve 4 BYTES for section mask
	buf_ucount_t section_mask_index = buf_get_element_count(buffer);
	buf_push_pseudo(buffer, 4);

	//Store index for section offsets to insert at later
	buf_resize(buffer, buf_get_element_count(buffer) + 1);
	buf_ucount_t section_offsets_index = buf_get_element_count(buffer);

	const char* source = _source;
	const char* temp1 = source;
	const char* temp2 = source;
	func_ptr_sig(const char* , parse, const char* source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer) = NULL;
	u8 shader_section_count = 0;
	u8 layout_section_count = 0;
	u8 settings_section_count = 0;
	u32 section_mask = 0x00;
	u8 section_count = 0;
	while((source = strchr(source, PREPROCESSOR)) != NULL)
	{
		temp2 = source;
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
			LOG_MSG("SETTINGS section found, parsing & compiling ...\n");
			if(settings_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_SETTINGS_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n');
			if(source == NULL) break; //if the file ended
			source++;
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buffer_insert_offset(buffer, offsets_buffer, section_offsets_index);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer, offsets_buffer);
			}
			func_ptr(parse) = func_ptr(parse_settings);
			temp1 = __source;
			settings_section_count++;
			section_mask |= (1UL << 0) << (section_count * 3);
			section_count++;
			continue;
		}
		else if(strncmp(source, SECTION_LAYOUT, count) == 0)
		{
			LOG_MSG("LAYOUT section found, parsing & compiling ...\n");
			if(layout_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_LAYOUT_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n');
			if(source == NULL) break; //if the file ended
			source++;
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buffer_insert_offset(buffer, offsets_buffer, section_offsets_index);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer, offsets_buffer);
			}
			func_ptr(parse) = func_ptr(parse_layout);
			temp1 = __source;
			layout_section_count++;
			section_mask |= (1UL << 1) << (section_count * 3);
			section_count++;
			continue;
		}
		else if(strncmp(source, SECTION_SHADER, count) == 0)
		{
			LOG_MSG("SHADER section found, parsing & compiling ...\n");
			if(shader_section_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_SHADER_SECTION_FOUND);
			source += count;
			source = strchr(source, '\n');
			if(source == NULL) break; //if the file ended
			source++;
			const char* __source = source;
			if(func_ptr(parse) != NULL)
			{
				buffer_insert_offset(buffer, offsets_buffer, section_offsets_index);
				func_ptr(parse) params(temp1, temp2 - temp1, buffer, offsets_buffer);
			}
			func_ptr(parse) = func_ptr(parse_shader);
			temp1 = __source;
			shader_section_count++;
			section_mask |= (1UL << 2) << (section_count * 3);
			section_count++;
			continue;
		}
		LOG_FETAL_ERR("shader parse error: Unsupported section: %.*s\n", count, source);
	}

	if(shader_section_count == 0)
		LOG_FETAL_ERR("shader parse error: Shader section not found\n");

	if(func_ptr(parse) != NULL)
	{
		buffer_insert_offset(buffer, offsets_buffer, section_offsets_index);
		source = func_ptr(parse) params(temp1, _source + length - temp1, buffer, offsets_buffer);
	}

	//Write section mask to the buffer, 4 BYTES
	*(u32*)buf_get_ptr_at(buffer, section_mask_index) = section_mask;

	//Reverse the offsets (array of u32)
	u32* offsets = (u32*)buf_get_ptr_at(buffer, section_offsets_index);
	for(u8 i = 0; i < (section_count >> 1); i++)
	{
		u32 temp = offsets[i];
		offsets[i] = offsets[section_count - i - 1];
		offsets[section_count - i - 1] = temp;
	}

	buf_fit(buffer);
	LOG_MSG("Compiled shader binary info: { size = %llu bytes }\n", buf_get_element_count(buffer));
	buf_free(offsets_buffer);
	CALLTRACE_RETURN(buffer);
}

static void buffer_insert_bytes(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index, const u8* bytes, u32 size)
{
	buf_ucount_t count = buf_get_element_count(offsets_buffer);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		buf_ucount_t _index;
		buf_get_at(offsets_buffer, i, &_index);
		if(_index < index)
			continue;
		*(u32*)buf_get_ptr_at(buffer, _index) += size;
		_index += size;
		buf_set_at(offsets_buffer, i, &_index);
	}
	buf_insert_pseudo(buffer, index, size);
	char* const ptr = buf_get_ptr_at(buffer, index);
	memcpy(ptr, bytes, size);
}

static void buffer_insert_offset(BUFFER* buffer, BUFFER* offsets_buffer, buf_ucount_t index)
{
	// update all the previous offsets
	buf_ucount_t count = buf_get_element_count(offsets_buffer);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		buf_ucount_t _index;
		buf_get_at(offsets_buffer, i, &_index);
		if(_index < index)
			continue;
		*(u32*)buf_get_ptr_at(buffer, _index) += 4;
		_index += 4;
		buf_set_at(offsets_buffer, i, &_index);
	}

	// register for next update
	buf_push(offsets_buffer, &index);
	
	// insert 4 bytes into "buffer"
	buf_insert_pseudo(buffer, index, 4);
	*(u32*)buf_get_ptr_at(buffer, index) = buf_get_element_count(buffer);
}

/*
  		*---------------------------INPUT STRING FORMAT---------------------------*

per-vertex [0, 0] vec3 position;
per-vertex [0, 1] vec3 normal;
per-vertex [0, 2] vec2 texcoord;
per-vertex [0, 3] vec3 tangent;

per-instance [0, 0] vec3 offset;

vertex [push_constant] uniform Push
{
	mat4 mvp_matrix; 		// 64 bytes
	mat4 model_matrix; 		// 64 bytes
} push;

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
 0 				| u16 						| descriptor count + attribute count
 ...			| array of u32 				| offsets for each serialized descriptor
 0 				| u8 						| descriptor set number/index or vertex attribute binding number, if the type is push_constant then it would be the offset
 1 				| u8 						| descriptor binding number/index or vertex attribute layout number, if the type is push_constant then it would be 0xFF
 2 				| u32 						| BIT(18) set if the type is a per-vertex attribute
 				|							| BIT(17) set if the type is a per-instance attribute
  				| 							| BIT(16) set if the type is a push_constant
  				| 							| BIT(15) set if the type is an opaque type
 				| 							| BIT(14) set if the type is an uniform
  				| 							| BIT(13) = vertex shader, BIT(12) = tessellation shader, BIT(11) = geometry shader, BIT(10) = fragment shader
  				| 							| Lower 8 Bits contains the type information
 3 				| null terminated string 	| if the type is block then it would be block name
 len0() + 4		| null terminated string 	| identifier name
 len01() + 5	| u16						| contains number of members in the block if the type is a block
 len01() + 7 	| u32 						| contains length of the array if the type is an array
 */
static function_signature(const char*, parse_layout, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer)
{
	CALLTRACE_BEGIN();
	const char* string = _source;
	const char* const end = string + length;
	if(is_empty(string, end))
	{
		LOG_MSG("LAYOUT section is empty, skipping\n");
		buffer_write_u16(buffer, 0);
		CALLTRACE_RETURN(_source + length);
	}

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
		u32 shader_mask = 0;
		u32 storage_mask = 0;
		u32 layout_info_mask = 0;
		u32 values[2] = { 0xFF, 0xFF };

		string = parse_shader_stage(string, &shader_mask);
		bool is_attribute = (shader_mask & (1UL << 18)) || (shader_mask & (1UL << 17));
		bool is_found = false;			// true if [push_constant] is found
		string = parse_square_brackets(string, &is_found, &layout_info_mask);
		//  if push_constant found then look for offset (a number) in another square brackets
		string = parse_array_operator(string, &values[0], is_found ? 1 : 2);
		// look for storage qualifiers only if there is a shader stage (meaning a descriptor)
		if(!is_attribute)
			string = parse_storage_qualifiers(string, &storage_mask);

		//Write offset (byte count), 4 BYTES
		buffer_insert_offset(buffer, offsets_buffer, descriptor_offsets_index);
		// log_u32(buf_get_element_count(buffer));

		//Write descriptor set number/index or binding number in case of vertex attribute, 1 BYTE
		buffer_write_u8(buffer, (u8)values[0]);

		//Write descriptor binding number/index or layout number in case of vertex attribute, 1 BYTE
		buffer_write_u8(buffer, (u8)values[1]);

		string = parse_type(string, layout_info_mask | storage_mask | shader_mask, buffer, offsets_buffer);
		
		descriptor_count++;
	}

	//Write number of descriptor count, 2 BYTES
	*(u16*)buf_get_ptr_at(buffer, descriptor_count_index) = descriptor_count;
	// log_u32(descriptor_count);

	//Reverse the descriptor_offsets (array of u32)
	u32* descriptor_offsets = buf_get_ptr_at(buffer, descriptor_offsets_index);
	for(u16 i = 0; i < (descriptor_count >> 1); i++)
	{
		u32 temp = descriptor_offsets[i];
		descriptor_offsets[i] = descriptor_offsets[descriptor_count - i - 1];
		descriptor_offsets[descriptor_count - i - 1] = temp;
	}
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
static const char* parse_type(const char* string, u32 mask, BUFFER* buffer, BUFFER* offsets_buffer)
{
	while(isspace(*string)) string++;
	u32 count = get_word_length(string, 0);
	bool found = true;
	u32 type;
	buf_ucount_t identifier_name_index;
	if(strncmp(string, "sampler2D", count) == 0) { type = (1UL << 15) | (u16)SHADER_COMPILER_SAMPLER_2D;  }
	else if(strncmp(string, "sampler3D", count) == 0) { type = (1UL << 15) | (u16)SHADER_COMPILER_SAMPLER_3D; }
	else if(strncmp(string, "samplerCube", count) == 0) { type = (1UL << 15) | (u16)SHADER_COMPILER_SAMPLER_CUBE; }
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
		if((count == 0) || is_empty(block_name, block_name + count))
			LOG_FETAL_ERR("shader layout parse error: Block name cannot be empty\n");
		string++;
		type = (u16)SHADER_COMPILER_BLOCK;

		//Write type info to the buffer, 4 BYTES
		buffer_write_u32(buffer, type | mask);
		// log_u32(type | mask);

		//Write block name to the buffer, count + 1 BYTES
		buffer_write_string(buffer, block_name, count, true);

		identifier_name_index = buf_get_element_count(buffer);

		//Reserve 2 BYTES for element count in the buffer
		buf_ucount_t index = buf_get_element_count(buffer);
		buf_push_pseudo(buffer, 2);

		u16 num_elements = 0;
		while(*string != '}')
		{
			u16 info;
			string = parse_type(string, mask, buffer, offsets_buffer);
			while(isspace(*string)) string++;
			num_elements++;
		}

		//Write number of elements to the buffer, 2 BYTES
		*(u16*)buf_get_ptr_at(buffer, index) = num_elements;

		string++;
	}

	if(found)
	{
		//Write type info to the buffer, 4 BYTES
		buffer_write_u32(buffer, type | mask);
		identifier_name_index = buf_get_element_count(buffer);
		string += count;
	}

	while(isspace(*string)) string++;
	count = get_word_length(string, ';');
	if((count == 0) || is_empty(string, string + count))
		LOG_FETAL_ERR("shader layout parse error: Identifier name cannot be empty\n");

	//Write identifier name to the buffer, count + 1 BYTES
	char identifier_name[count + 1]; memcpy(identifier_name, string, count); identifier_name[count] = 0;
	buffer_insert_bytes(buffer, offsets_buffer, identifier_name_index, identifier_name, count + 1);
	// LOG_MSG("Identifier: %s\n", identifier_name);

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
	string++;
	return string;
}

static const char* parse_storage_qualifiers(const char* string, u32* out_info)
{
	while(isspace(*string)) string++;
	u32 mask = 0;
	u32 count = get_word_length(string, 0);
	if(strncmp(string, "uniform", count) == 0)
		mask |= (1UL << 14);
	else
		LOG_FETAL_ERR("shader data layout parse error: Unrecognized storage qualifier \"%.*s\"\n", count, string);
	*out_info = mask;
	return string + count;
}

static inline u32 min(u32 v1, u32 v2) { return (v1 > v2) ? v2 : v1; }

static const char* parse_shader_stage(const char* string, u32* out_mask)
{
	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
	u32 count = 0;
	u8 stage_count = 0;
	u32 mask = 0;
	bool found_attribute = false;
	while(true)
	{
		while(isspace(*string)) string++;
		count = get_word_length(string, '[');
		if(count == 0) break;
		if(strncmp(string, "per", min(3, count)) == 0)
		{
			if(strncmp(string + 3, "-vertex", min(7, count - 3)) == 0)
				mask |= 1UL << 18;
			else if(strncmp(string + 3, "-instance", min(9, count - 3)) == 0)
				mask |= 1UL << 17;
			else
				LOG_FETAL_ERR("shader data layout parse error: Unrecognized vertex attribute qualifer \"%.*s\"\n", count, string);
			string += count;
			found_attribute = true;
			break;
		}
		else if(strncmp(string, SHADER_STAGE_VERTEX_STR, count) == 0)
			mask |= 1UL << (13 - SHADER_COMPILER_SHADER_STAGE_VERTEX);
		else if(strncmp(string, SHADER_STAGE_TESSELLATION_STR, count) == 0)
			mask |= 1UL << (13 - SHADER_COMPILER_SHADER_STAGE_TESSELLATION);
		else if(strncmp(string, SHADER_STAGE_GEOMETRY_STR, count) == 0)
			mask |= 1UL << (13 - SHADER_COMPILER_SHADER_STAGE_GEOMETRY);
		else if(strncmp(string, SHADER_STAGE_FRAGMENT_STR, count) == 0)
			mask |= 1UL << (13 - SHADER_COMPILER_SHADER_STAGE_FRAGMENT);
		else break;
		string += count;
		stage_count++;
	}
	if((stage_count == 0) && !found_attribute)
		LOG_FETAL_ERR("shader layout parse error: Unrecognized symbol \"%.*s\", expected shader stage or vertex attribute qualifer!\n", count, string);
	*out_mask = mask;
	return string;
}

static const char* parse_square_brackets(const char* string, bool* is_found, u32* layout_info_mask)
{
	const char* origin = string;
	while(isspace(*string)) string++;
	if(strncmp(string, "[", 1) == 0)
	{
		string++;
		while(isspace(*string)) string++;
		u8 len = get_word_length(string, ']');
		if(strncmp(string, "push_constant", len) == 0)
		{
			*is_found = true;
			*layout_info_mask = (1UL << 16);
		}
		else
			return origin;
		string += len;
		while(*string != ']')
		{
			if(!isspace(*string))
			{
				len = get_word_length(string, 0);
				LOG_FETAL_ERR("shader layout parse error: Unrecognized symbol \"%.*s\" in square brackets\n", len, string);
			}
			string++;
		}
		string++;
	}
	return string;
}

static const char* parse_array_operator(const char* string, u32* out_data, u8 required)
{
	while(isspace(*string)) string++;
	u8 count = 0;
	u32 arr_len = 0;
	bool found = false;
	if(strncmp(string, "[", 1) == 0)
	{
		found = true;
PARSE_NUMBER:
		string++;
		while(isspace(*string)) string++;
		u8 len = 0;
		while(isdigit(*string)) { string++; len++; };
		char sub_str[len + 1];
		/*	
			WARNING: 	Don't use strncpy(sub_str, string - len, len) here!
		 	strncpy: 	No null-character is implicitly appended at the end of destination if source is longer than num. 
		 				Thus, in this case, destination shall not be considered a null terminated C string (reading it as such would overflow).
		*/
		memcpy(sub_str, string - len, len); sub_str[len] = 0;
		arr_len = strtoul(sub_str, NULL, 0);
		out_data[count] = arr_len;
		// log_u32(arr_len);
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
	if(!found)
	{
		u32 len = get_word_length(string, 0);
		LOG_FETAL_ERR("shader layout parse error: Expected open square bracket '[' before \"%.*s\"\n", count, string);
	}
	if(count < required)
		LOG_FETAL_ERR("shader layout parse error: Less than required elements found in the array index operator, \"%u\"\n", arr_len);
	return string;
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

static function_signature(const char*, parse_shader, const char* _source, buf_ucount_t length, BUFFER* buffer, BUFFER* offsets_buffer)
{
	CALLTRACE_BEGIN();
	const char* string = _source;
	if(is_empty(string, string + length))
	{
		LOG_MSG("SHADER section is empty, skipping\n");
		CALLTRACE_RETURN(_source + length);
	}

	//Parse the _source string to separate vertex, tessellation, geometry, and fragment shaders
	const char* stage_strings[SHADER_COMPILER_SHADER_STAGE_MAX] = { "vertex", "tessellation", "geometry", "fragment" };
	shader_source_t sources[SHADER_COMPILER_SHADER_STAGE_MAX]; memset(sources, 0, sizeof(shader_source_t) * SHADER_COMPILER_SHADER_STAGE_MAX);
	const char* temp = NULL;
	shader_source_t* previous_source = NULL;
	uint8_t shader_count = 0;
	u8 vertex_shader_count = 0;
	u8 tessellation_shader_count = 0;
	u8 geometry_shader_count = 0;
	u8 fragment_shader_count = 0;
	while((string = strchr(string, PREPROCESSOR)) != NULL)
	{
		temp = string - 1;
		string += 1;
		while((*string == ' ') || (*string == '\t')) string++;
		if(strncmp(string, STAGE_DIRECTIVE, strlen(STAGE_DIRECTIVE)) != 0)
			continue;
		string += strlen(STAGE_DIRECTIVE);
		while((*string == ' ') || (*string == '\t')) string++;
		uint32_t count = 0;
		while(!isspace(*string)) { string++; count++; }
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
		string -= count;
		if(strncmp(string, SHADER_STAGE_VERTEX_STR, count) == 0)
		{
			if(vertex_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_VERTEX_SHADER_FOUND);
			//set vertex shader
			string = strchr(string, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_VERTEX] = (shader_source_t)
			{
				.source = string,
				.stage = SHADER_COMPILER_SHADER_STAGE_VERTEX,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_VERTEX];
			shader_count++;
			vertex_shader_count++;
			continue;
		}
		if(strncmp(string, SHADER_STAGE_TESSELLATION_STR, count) == 0)
		{
			if(tessellation_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_TESSELLATION_SHADER_FOUND);
			//set tessellation shader
			string = strchr(string, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION] = (shader_source_t)
			{
				.source = string,
				.stage = SHADER_COMPILER_SHADER_STAGE_TESSELLATION,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_TESSELLATION];
			shader_count++;
			tessellation_shader_count++;
			continue;
		}
		if(strncmp(string, SHADER_STAGE_GEOMETRY_STR, count) == 0)
		{
			if(geometry_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_GEOMETRY_SHADER_FOUND);
			//set geometry shader
			string = strchr(string, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY] = (shader_source_t)
			{
				.source = string,
				.stage = SHADER_COMPILER_SHADER_STAGE_GEOMETRY,
			};
			previous_source = &sources[SHADER_COMPILER_SHADER_STAGE_GEOMETRY];
			shader_count++;
			geometry_shader_count++;
			continue;
		}
		if(strncmp(string, SHADER_STAGE_FRAGMENT_STR, count) == 0)
		{
			if(fragment_shader_count > 0) shader_parse_error(SHADER_PARSE_ERROR_MORE_THAN_ONE_FRAGMENT_SHADER_FOUND);
			//set fragment shader
			string = strchr(string, '\n') + 1;
			sources[SHADER_COMPILER_SHADER_STAGE_FRAGMENT] = (shader_source_t)
			{
				.source = string,
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
	serialize_shader(stage_strings, sources, shader_count, buffer, offsets_buffer);
	CALLTRACE_RETURN(_source + length);
}

static function_signature(void, serialize_shader, const char* const* stage_strings, shader_source_t* sources, uint8_t shader_count, BUFFER* buffer, BUFFER* offsets_buffer)
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

	// log_u32(shader_mask);

	//Allocate space for offsets and lengths for each shader SPIRV binary
	buf_ucount_t indices[shader_count];
	for(uint8_t i = 0; i < shader_count; i++)
	{
		indices[i] = buf_get_element_count(buffer);
		buf_push_pseudo(buffer, 8);
		// register for next update
		buf_push(offsets_buffer, &indices[i]);
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
		// log_u32(offset);
		// log_u32(length);

		//Write offset
		memcpy(buf_get_ptr_at(buffer, indices[j]), &offset, 4);

		//Write length
		memcpy(buf_get_ptr_at(buffer, indices[j]) + 4, &length, 4);

		//Write SPIRV binary
		buf_pushv(buffer, bytes, length);

		shaderc_result_release(result);
		j++;
	}
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
	CALLTRACE_END();
}
