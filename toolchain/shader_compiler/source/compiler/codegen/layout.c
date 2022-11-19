#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <shader_compiler/debug.h>
#include <glslcommon/glsl_types.h>
#include <stdlib.h>

static const char* parse_qualifers(const char* start, const char* end, u32 OUT bits);
static const char* parse_square_brackets(const char* start, const char* end, u32 OUT bits);
static const char* write_set_and_binding_numbers(const char* start, const char* end, u8 required, binary_writer_t* writer);
static const char* parse_storage_qualifiers(const char* start, const char* end, u32 OUT bits);
static const char* write_description(const char* start, const char* end, u32 bits, binary_writer_t* writer);

SC_API void write_layout(const char* start, const char* end, codegen_buffer_t* writer)
{
	if(is_empty(start, end))
	{
		debug_log_info("Properties and Layout, both blocks are empty, skipping");
		binary_writer_u16(writer->main, (u16)0);
		return;
	}

	binary_writer_u16_mark(writer->main, MARK_ID_DESCRIPTOR_COUNT);

	u16 descriptor_count = 0;
	while(start < end)
	{
		while(isspace(*start)) start++;
		if(start >= end) break; //end of the file reached

		binary_writer_u32_mark(writer->main, MARK_ID_DESCRIPTOR_OFFSET + MARK_ID_MAX + descriptor_count);
		binary_writer_u32_set(writer->main, MARK_ID_DESCRIPTOR_OFFSET + MARK_ID_MAX + descriptor_count, binary_writer_pos(writer->data));

		u32 bits = 0;				// description info bits

		// parse the qualifiers such as per_vertex, per_instance, fragment, vertex, geometry and tessellation
		start = parse_qualifers(start, end, &bits);
		// parse the first pair of square brackets, i.e. [push_constant]
		start = parse_square_brackets(start, end, &bits);
		// parse the second pair of square backets, it shall contain list of numbers only
		// if push_constant found then look for offset (a number)
		start = write_set_and_binding_numbers(start, end, (bits & PUSH_CONSTANT_BIT) ? 1 : 2, writer->data);

		// look for storage qualifiers only if there is a shader stage qualifier
		if(!((bits & PER_VERTEX_ATTRIB_BIT) || (bits & PER_INSTANCE_ATTRIB_BIT)))
			start = parse_storage_qualifiers(start, end, &bits);

		start = write_description(start, end, bits, writer->data);

		descriptor_count++;
	}

	//Write number of descriptor count, 2 BYTES
	binary_writer_u16_set(writer->main, MARK_ID_DESCRIPTOR_COUNT, descriptor_count);
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
static const char* write_description(const char* start, const char* end, u32 bits, binary_writer_t* writer)
{
	while(isspace(*start)) start++;
	u32 count = get_word_length(start, 0);
	bool found = true;
	buf_ucount_t identifier_name_index;
	if(strncmp(start, "sampler2D", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_2D; 
		bits |= OPAQUE_BIT; 
	}
	else if(strncmp(start, "sampler3D", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_3D; 
		bits |= OPAQUE_BIT; 
	}
	else if(strncmp(start, "samplerCube", count) == 0)
	{ 
		bits |= GLSL_TYPE_SAMPLER_CUBE;
		bits |= OPAQUE_BIT;
	}
	else if(strncmp(start, "vec4", count) == 0)
		bits |= GLSL_TYPE_VEC4;
	else if(strncmp(start, "vec3", count) == 0)
		bits |= GLSL_TYPE_VEC3;
	else if(strncmp(start, "vec2", count) == 0)
		bits |= GLSL_TYPE_VEC2;
	else if(strncmp(start, "ivec4", count) == 0)
		bits |= GLSL_TYPE_IVEC4;
	else if(strncmp(start, "ivec3", count) == 0)
		bits |= GLSL_TYPE_IVEC3;
	else if(strncmp(start, "ivec2", count) == 0)
		bits |= GLSL_TYPE_IVEC2;
	else if(strncmp(start, "uvec4", count) == 0)
		bits |= GLSL_TYPE_UVEC4;
	else if(strncmp(start, "uvec3", count) == 0)
		bits |= GLSL_TYPE_UVEC3;
	else if(strncmp(start, "uvec2", count) == 0)
		bits |= GLSL_TYPE_UVEC2;
	else if(strncmp(start, "int", count) == 0)
		bits |= GLSL_TYPE_INT;
	else if(strncmp(start, "uint", count) == 0)
		bits |= GLSL_TYPE_UINT;
	else if(strncmp(start, "float", count) == 0)
		bits |= GLSL_TYPE_FLOAT;
	else if(strncmp(start, "mat4", count) == 0)
		bits |= GLSL_TYPE_MAT4;
	else if(strncmp(start, "mat3", count) == 0)
		bits |= GLSL_TYPE_MAT3;
	else if(strncmp(start, "mat2", count) == 0)
		bits |= GLSL_TYPE_MAT2;
	else
	{
		found = false;
		const char* block_name = start;
		start += count;
		while(*start != '{')
		{
			if(!isspace(*start))
			{
				count = get_word_length(start, 0);
				debug_log_fetal_error("shader layout parse error: Unexpected symbol \"%.*s\", '{' is expected\n", count, start);
			}
			start++;
		}
		if((count == 0) || is_empty(block_name, block_name + count))
			debug_log_fetal_error("shader layout parse error: Block name cannot be empty\n");
		start++;

		u32 _bits = bits;
		if(bits & STORAGE_BUFFER_BIT)
			bits |= GLSL_TYPE_STORAGE_BUFFER;
		else if(bits & PUSH_CONSTANT_BIT)
			bits |= GLSL_TYPE_PUSH_CONSTANT;
		else 
		{
			bits |= UNIFORM_BUFFER_BIT;
			bits |= GLSL_TYPE_UNIFORM_BUFFER;
		}

		// write type info to the buffer, 4 BYTES
		binary_writer_u32(writer, bits);

		// write block name to the buffer, count + 1 BYTES
		binary_writer_stringn(writer, block_name, count);
		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME);
		binary_writer_u16_mark(writer, MARK_ID_FIELD_COUNT);

		u16 num_elements = 0;
		while(*start != '}')
		{
			u16 info;
			start = write_description(start, end, _bits, writer);
			while(isspace(*start)) start++;
			num_elements++;
		}

		// write number of elements to the buffer, 2 BYTES
		binary_writer_u16_set(writer, MARK_ID_FIELD_COUNT, num_elements);

		start++;
	}

	if(found)
	{
		// write type info to the buffer, 4 BYTES
		binary_writer_u32(writer, bits);
		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME);
		start += count;
	}

	while(isspace(*start)) start++;
	count = get_word_length(start, ';');
	if((count == 0) || is_empty(start, start + count))
		debug_log_fetal_error("shader layout parse error: Identifier name cannot be empty\n");

	// write identifier name to the buffer, count + 1 BYTES
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME, start, count);
	char null_char = 0;
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME, &null_char, 1);

	start += count;
	while(*start != ';')
	{
		if(!isspace(*start))
		{
			count = get_word_length(start, 0);
			debug_log_fetal_error("shader layout parse error: Expected ';' before \"%.*s\"\n", count, start);
		}
		start++;
	}
	start++;
	return start;
}

static const char* parse_storage_qualifiers(const char* start, const char* end, u32 OUT bits)
{
	while(isspace(*start)) start++;
	u32 mask = 0;
	u32 count = get_word_length(start, 0);
	// if(strncmp(start, "uniform", count) == 0) do nothing
	if(strncmp(start, "buffer", count) == 0)
		mask |= STORAGE_BUFFER_BIT;
	else
		debug_log_fetal_error("shader data layout parse error: Unrecognized storage qualifier \"%.*s\"\n", count, start);
	OUT bits |= mask;
	return start + count;
}

static const char* parse_qualifers(const char* start, const char* end, u32 OUT bits)
{
	u32 count = 0;
	u8 stage_count = 0;
	u32 mask = 0;
	bool found_attribute = false;
	while(true)
	{
		while(isspace(*start) && (start < end)) start++;
		count = get_word_length(start, '[');
		if(count == 0) break;
		if(strncmp(start, "per", min(3, count)) == 0)
		{
			if(strncmp(start + 3, "_vertex", min(7, count - 3)) == 0)
				mask |= PER_VERTEX_ATTRIB_BIT;
			else if(strncmp(start + 3, "_instance", min(9, count - 3)) == 0)
				mask |= PER_INSTANCE_ATTRIB_BIT;
			else
				debug_log_fetal_error("shader data layout parse error: Unrecognized vertex attribute qualifer \"%.*s\"\n", count, start);
			start += count;
			found_attribute = true;
			break;
		}
		else if(strncmp(start, "vertex", count) == 0)
			mask |= VERTEX_BIT;
		else if(strncmp(start, "tessellation", count) == 0)
			mask |= TESSELLATION_BIT;
		else if(strncmp(start, "geometry", count) == 0)
			mask |= GEOMETRY_BIT;
		else if(strncmp(start, "fragment", count) == 0)
			mask |= FRAGMENT_BIT;
		else break;
		start += count;
		stage_count++;
	}
	if((stage_count == 0) && !found_attribute)
		debug_log_fetal_error("shader layout parse error: Unrecognized symbol \"%.*s\", expected shader stage or vertex attribute qualifer!\n", count, start);
	OUT bits |= mask;
	return start;
}

static const char* parse_square_brackets(const char* start, const char* end, u32 OUT bits)
{
	const char* origin = start;
	while(isspace(*start)) start++;
	if(strncmp(start, "[", 1) == 0)
	{
		start++;
		while(isspace(*start)) start++;
		u8 len = get_word_length(start, ']');
		if(strncmp(start, "push_constant", len) == 0)
			OUT bits |= PUSH_CONSTANT_BIT;
		else
			return origin;
		start += len;
		while(*start != ']')
		{
			if(!isspace(*start))
			{
				len = get_word_length(start, 0);
				debug_log_fetal_error("shader layout parse error: Unrecognized symbol \"%.*s\" in square brackets\n", len, start);
			}
			start++;
		}
		start++;
	}
	return start;
}

static const char* write_set_and_binding_numbers(const char* start, const char* end, u8 required, binary_writer_t* writer)
{
	while(isspace(*start)) start++;
	u8 count = 0;
	u32 arr_len = 0;
	bool found = false;
	if(strncmp(start, "[", 1) == 0)
	{
		found = true;
PARSE_NUMBER:
		start++;
		while(isspace(*start)) start++;
		u8 len = 0;
		while(isdigit(*start)) { start++; len++; };
		char sub_str[len + 1];
		/*	
			WARNING: 	Don't use strncpy(sub_str, start - len, len) here!
		 	strncpy: 	No null-character is implicitly appended at the end of destination if source is longer than num. 
		 				Thus, in this case, destination shall not be considered a null terminated C string (reading it as such would overflow).
		*/
		memcpy(sub_str, start - len, len); sub_str[len] = 0;
		arr_len = strtoul(sub_str, NULL, 0);


		// first iteration: write descriptor set number/index or binding number in case of vertex attribute, 1 BYTE
		// second iteration: write descriptor binding number/index or layout number in case of vertex attribute, 1 BYTE
		binary_writer_u8(writer, (u8)arr_len);
		
		// log_u32(arr_len);
		
		count++;
		if(count > required)
			debug_log_fetal_error("shader layout parse error: More than required elements found in the array index operator, \"%u\"\n", arr_len);
		while(*start != ']')
		{
			if(*start == ',')	
				goto PARSE_NUMBER;
			// if(!isspace(*start))
			// 	shader_data_layout_parse_error(SHADER_DATA_LAYOUT_PARSE_ERROR_INVALID_SYMBOLE_IN_ARRAY_SIZE, 1, start + 1);
			start++;
		}
		start++;
	}
	if(!found)
	{
		u32 len = get_word_length(start, 0);
		debug_log_fetal_error("shader layout parse error: Expected open square bracket '[' before \"%.*s\"\n", count, start);
	}
	if(count < required)
		debug_log_fetal_error("shader layout parse error: Less than required elements found in the array index operator, \"%u\"\n", arr_len);
	return start;
}
