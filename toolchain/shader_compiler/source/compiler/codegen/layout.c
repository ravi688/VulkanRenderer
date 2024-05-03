/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: layout.c is a part of VulkanRenderer

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

#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <shader_compiler/debug.h>
#include <shader_compiler/assert.h>
#include <glslcommon/glsl_types.h>
#include <stdlib.h>

static const char* parse_qualifers(const char* start, const char* end, u32 OUT bits);
static const char* parse_square_brackets(const char* start, const char* end, u32 OUT bits);
static const char* write_set_and_binding_numbers(const char* start, const char* end, u8 required, binary_writer_t* writer);
static const char* parse_storage_qualifiers(const char* start, const char* end, u32 OUT bits);
static const char* write_description(const char* start, const char* end, u32 bits, binary_writer_t* writer);

SC_API void write_layout(const char* start, const char* end, codegen_buffer_t* writer, bool is_write_count)
{
	if(is_empty(start, end))
	{
		debug_log_info("[Codegen] [Legacy] Properties or Layout is empty, skipping");
		if(is_write_count)
			binary_writer_u16(writer->main, (u16)0);
		return;
	}

	static s32 cnt = -1;
	static s32 cnt2 = -1;
	cnt2++;

	if(is_write_count)
	{
		cnt++;
		_ASSERT((MARK_ID_DESCRIPTOR_COUNT + cnt) < MARK_ID_DESCRIPTOR_COUNT_MAX);
		binary_writer_u16_mark(writer->main, MARK_ID_DESCRIPTOR_COUNT + cnt);
	}

	u16 descriptor_count = 0;
	while(start < end)
	{
		while(isspace(*start)) start++;
		if(start >= end) break; //end of the file reached

		cnt2++;
		_ASSERT((MARK_ID_DESCRIPTOR_OFFSET + cnt2) < MARK_ID_DESCRIPTOR_OFFSET_MAX);
		
		binary_writer_u32_mark(writer->main, MARK_ID_DESCRIPTOR_OFFSET + cnt2);
		binary_writer_u32_set(writer->main, MARK_ID_DESCRIPTOR_OFFSET + cnt2, binary_writer_pos(writer->data));

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

	if(is_write_count)
	{
		//Write number of descriptor count, 2 BYTES
		binary_writer_u16_set(writer->main, MARK_ID_DESCRIPTOR_COUNT + cnt, descriptor_count);
	}
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
	static long int cnt = -1;
	cnt++;
	long int _cnt = cnt;
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
	else if(strncmp(start, "subpassInput", count) == 0)
	{
		bits |= GLSL_TYPE_SUBPASS_INPUT;
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
				DEBUG_LOG_FETAL_ERROR("[Codegen] [Legacy] Unexpected symbol \"%.*s\", '{' is expected", get_word_length(start, 0), start);
			start++;
		}
		if((count == 0) || is_empty(block_name, block_name + count))
			DEBUG_LOG_ERROR("[Codegen] [Legacy] Block name cannot be empty");
		start++;

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

		_ASSERT((MARK_ID_IDENTIFIER_NAME + _cnt) < MARK_ID_IDENTIFIER_NAME_MAX);

		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME + _cnt);

		_ASSERT((MARK_ID_FIELD_COUNT + _cnt) < MARK_ID_FIELD_COUNT_MAX);

		binary_writer_u16_mark(writer, MARK_ID_FIELD_COUNT + _cnt);

		u16 num_elements = 0;
		while(*start != '}')
		{
			u16 info;
			start = write_description(start, end, 0, writer);
			while(isspace(*start)) start++;
			num_elements++;
		}

		if(num_elements == 0)
			debug_log_warning("[Codegen] [Legacy] Block \"%.*s\" has zero elements", count, block_name);

		// write number of elements to the buffer, 2 BYTES
		binary_writer_u16_set(writer, MARK_ID_FIELD_COUNT + _cnt, num_elements);

		start++;
	}

	if(found)
	{
		// write type info to the buffer, 4 BYTES
		binary_writer_u32(writer, bits);
		
		_ASSERT((MARK_ID_IDENTIFIER_NAME + _cnt) < MARK_ID_IDENTIFIER_NAME_MAX);

		binary_writer_mark(writer, MARK_ID_IDENTIFIER_NAME + _cnt);
		start += count;
	}

	while(isspace(*start)) start++;
	count = get_word_length(start, ';');
	if((count == 0) || is_empty(start, start + count))
		DEBUG_LOG_ERROR("[Codegen] [Legacy] Identifier name cannot be empty\n");

	// write identifier name to the buffer, count + 1 BYTES
	char null_char = 0;
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME + _cnt, &null_char, 1);
	binary_writer_insert(writer, MARK_ID_IDENTIFIER_NAME + _cnt, start, count);

	start += count;
	while(*start != ';')
	{
		if(!isspace(*start))
			DEBUG_LOG_ERROR("[Codegen] [Legacy] Expected ';' before \"%.*s\"\n", get_word_length(start, 0), start);
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
	else if(strncmp(start, "uniform", count) == 0)
		{ /* do nothing */ }
	else
		DEBUG_LOG_ERROR("[Codegen] [Legacy] Unrecognized storage qualifier \"%.*s\"\n", count, start);
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
				DEBUG_LOG_ERROR("[Codegen] [Legacy] Unrecognized vertex attribute qualifer \"%.*s\"\n", count, start);
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
		DEBUG_LOG_ERROR("[Codegen] [Legacy] Unrecognized symbol \"%.*s\", expected shader stage or vertex attribute qualifer!\n", count, start);
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
				DEBUG_LOG_ERROR("[Codegen] [Layout] Unrecognized symbol \"%.*s\" in square brackets\n", get_word_length(start, 0), start);
			start++;
		}
		start++;
	}
	return start;
}

static u32 parse_set(const char* str, u32 len)
{
	if(safe_strncmp(str, "CAMERA_SET", len) == 0) return 0;
	else if(safe_strncmp(str, "GLOBAL_SET", len) == 0) return 1;
	else if(safe_strncmp(str, "RENDER_SET", len) == 0) return 2;
	else if(safe_strncmp(str, "SUB_RENDER_SET", len) == 0) return 3;
	else if(safe_strncmp(str, "MATERIAL_SET", len) == 0) return 4;
	else if(safe_strncmp(str, "OBJECT_SET", len) == 0) return 5;
	return U32_MAX;
}

static u32 parse_location(const char* str, u32 len)
{
	if(safe_strncmp(str, "POSITION_LOCATION", len) == 0) return  0;
	else if(safe_strncmp(str, "NORMAL_LOCATION", len) == 0) return 1;
	else if(safe_strncmp(str, "COLOR_LOCATION", len) == 0) return 2;
	else if(safe_strncmp(str, "TEXCOORD_LOCATION", len) == 0) return 3;
	else if(safe_strncmp(str, "TANGENT_LOCATION", len) == 0) return 4;
	return U32_MAX;
}

static u32 parse_binding(const char* str, u32 len)
{
	if(safe_strncmp(str, "CAMERA_PROPERTIES_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "CAMERA_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "LIGHT_BINDING", len) == 0) return 1;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING0", len) == 0) return 0;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING1", len) == 0) return 1;
	else if(safe_strncmp(str, "INPUT_ATTACHMENT_BINDING2", len) == 0) return 2;
	else if(safe_strncmp(str, "MATERIAL_PROPERTIES_BINDING", len) == 0) return 0;
	else if(safe_strncmp(str, "TEXTURE_BINDING0", len) == 0) return 1;
	else if(safe_strncmp(str, "TEXTURE_BINDING1", len) == 0) return 2;
	else if(safe_strncmp(str, "TEXTURE_BINDING2", len) == 0) return 3;
	else if(safe_strncmp(str, "TEXTURE_BINDING3", len) == 0) return 4;
	else if(safe_strncmp(str, "TEXTURE_BINDING4", len) == 0) return 5;
	else if(safe_strncmp(str, "TEXTURE_BINDING5", len) == 0) return 6;
	else if(safe_strncmp(str, "TEXTURE_BINDING6", len) == 0) return 7;
	else if(safe_strncmp(str, "TEXTURE_BINDING7", len) == 0) return 8;
	else if(safe_strncmp(str, "TEXTURE_BINDING8", len) == 0) return 9;
	else if(safe_strncmp(str, "TRANSFORM_BINDING", len) == 0) return 0;

	else if(safe_strncmp(str, "POSITION_BINDING", len) == 0) return  0;
	else if(safe_strncmp(str, "NORMAL_BINDING", len) == 0) return 1;
	else if(safe_strncmp(str, "COLOR_BINDING", len) == 0) return  2;
	else if(safe_strncmp(str, "TEXCOORD_BINDING", len) == 0) return 3;
	else if(safe_strncmp(str, "TANGENT_BINDING", len) == 0) return 4;

	return U32_MAX;
}

static u32 try_parse_to_u32(const char* str)
{
	u32 len = strlen(str);
	u32 value;
	if((value = parse_binding(str, len)) != U32_MAX)
		return value;
	else if((value = parse_set(str, len)) != U32_MAX)
		return value;
	else if((value = parse_location(str, len)) != U32_MAX)
		return value;
	else
		return strtoul(str, NULL, 0);
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
		while(isalnum(*start) || (*start == '_')) { start++; len++; };
		char sub_str[len + 1];
		/*	
			WARNING: 	Don't use strncpy(sub_str, start - len, len) here!
		 	strncpy: 	No null-character is implicitly appended at the end of destination if source is longer than num. 
		 				Thus, in this case, destination shall not be considered a null terminated C string (reading it as such would overflow).
		*/
		memcpy(sub_str, start - len, len); sub_str[len] = 0;
		arr_len = try_parse_to_u32(sub_str);


		// first iteration: write descriptor set number/index or binding number in case of vertex attribute, 1 BYTE
		// second iteration: write descriptor binding number/index or layout number in case of vertex attribute, 1 BYTE
		binary_writer_u8(writer, (u8)arr_len);
		
		count++;
		
		if(count > required)
		{
			DEBUG_LOG_ERROR("[Codegen] [Legacy] More than required elements found in the array index operator, \"%u\"\n", arr_len);
			return start;
		}

		while(*start != ']')
		{
			if(*start == ',')	
				goto PARSE_NUMBER;
			if(!isspace(*start))
				DEBUG_LOG_ERROR("[Codegen] [Legacy] Invalid symbol \"%.*s\" in array size", __get_word_length(start, end, " \t\n"), start);
			start++;
		}
		start++;
	}
	if(!found)
	{
		u32 len = get_word_length(start, 0);
		DEBUG_LOG_ERROR("[Codegen] [Legacy] Expected open square bracket '[' before \"%.*s\"\n", count, start);
	}
	if(count < required)
		DEBUG_LOG_ERROR("[Codegen] [Legacy] Less than required elements found in the array index operator, \"%u\"\n", arr_len);
	return start;
}
