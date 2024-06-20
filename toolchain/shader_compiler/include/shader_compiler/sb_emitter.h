/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: sb_emitter.h is a part of VulkanRenderer

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

#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h> /* codegen_buffer_t */
#include <shader_compiler/compiler/codegen/header.h> /* sb_version_t */
#include <glslcommon/glsl_types.h> /* glsl_type_t, glsl_memory_layout_t */
#include <bufferlib/buffer.h> /* buffer_t */

typedef enum vertex_input_rate_t
{
	VERTEX_INPUT_RATE_VERTEX = 1,
	VERTEX_INPUT_RATE_INSTANCE = 2
} vertex_input_rate_t;

typedef enum storage_class_t
{
	STORAGE_CLASS_UNIFORM,
	STORAGE_CLASS_BUFFER
} storage_class_t;

/* user defined aggregate layout */
typedef enum udat_layout_t
{
	SCALAR_LAYOUT,
	STD430_LAYOUT,
	STD140_LAYOUT
} udat_layout_t;

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE udat_layout_t get_udat_layout_from_glsl_memory_layout(const glsl_memory_layout_t layout)
{
	switch(layout)
	{
		case GLSL_SCALAR: return SCALAR_LAYOUT;
		case GLSL_STD430: return STD430_LAYOUT;
		case GLSL_STD140: return STD140_LAYOUT;
		default: _assert(false); return SCALAR_LAYOUT;
	}
}

#define VERTEX_ATTRIBUTE_NAME_MAX_SIZE 64
#define SHADER_PROPERTY_NAME_MAX_SIZE 64
#define AGGREGATE_NAME_MAX_SIZE 64

/* vertex attribute information */
typedef struct vertex_attribute_info_t
{
	/* it is not null terminated */
	char name[VERTEX_ATTRIBUTE_NAME_MAX_SIZE];
	u32 name_length;
	vertex_input_rate_t rate;
	u32 binding;
	u32 location;
	glsl_type_t type;
} vertex_attribute_info_t;

typedef enum shader_stage_bits_t
{
	SHADER_STAGE_BIT_VERTEX = BIT32(0),
	SHADER_STAGE_BIT_TESSELLATION = BIT32(1),
	SHADER_STAGE_BIT_GEOMETRY = BIT32(2),
	SHADER_STAGE_BIT_FRAGMENT = BIT32(3)
} shader_stage_bits_t;

typedef struct shader_property_info_t
{
	/* it is not null terminated */
	char name[SHADER_PROPERTY_NAME_MAX_SIZE];
	u32 name_length;
	shader_stage_bits_t stages;
	union
	{
		u32 set;
		/* push constant offset */
		u32 offset;
	};
	u32 binding;
	glsl_type_t type;
	/* if this shader property is a user defined type, then it is never equal to CODEGEN_BUFFER_ADDRESS_NULL */
	codegen_buffer_address_t udat_address;
	storage_class_t storage;
	/* number of instances of this shader property */
	u32 array_size;
} shader_property_info_t;

typedef struct sb_emitter_t
{
	com_allocation_callbacks_t* callbacks;
	sb_version_t version;
	codegen_buffer_t* buffer;
	union
	{
		/* number of vertex attributes in the current Layout { } block, it will be reset to zero upon calling sb_emitter_open_vertex_attribute_array() function */
		u32 vtx_attr_count;
		/* number of shader properties in the current Properties { } block, it will be reset to zero upon calling sb_emitter_close_shader_property_array() function */
		u32 shr_prop_count;
	};
	union
	{
		codegen_buffer_address_t vtx_attr_dsc_cnt_addr;
		codegen_buffer_address_t shr_prop_dsc_cnt_addr;
	};
	s32 depth;
	union
	{
		vertex_attribute_info_t vtx_attr;
		shader_property_info_t shr_prop;
	};
} sb_emitter_t;

SC_API sb_emitter_t* sb_emitter_create(com_allocation_callbacks_t* callbacks, codegen_buffer_t* buffer, sb_version_t version);
SC_API void sb_emitter_destroy(sb_emitter_t* emitter);

/* it must be called after writing the header and material descriptors to the codegen buffer, otherwise we might get a corrupted shader binary at the end */
SC_API void sb_emitter_initialize(sb_emitter_t* emitter);

/* writes to the codegen_buffer_t respecting the shader binary version specified at the time of creating sb_emitter_t object */
SC_API void sb_emitter_flush(sb_emitter_t* emitter);

/* VERTEX ATTRIBUTES <begin> */

SC_API void sb_emitter_open_vertex_attribute_array(sb_emitter_t* emitter);

/* creates a new vertex_attribute_info_t object into the vtx_attr_infos list */
SC_API void sb_emitter_open_vertex_attribute(sb_emitter_t* emitter);

SC_API void sb_emitter_emit_vertex_bind_and_location(sb_emitter_t* emitter, u32 binding, u32 location);

SC_API void sb_emitter_emit_vertex_input_rate(sb_emitter_t* emitter, vertex_input_rate_t rate);

SC_API void sb_emitter_emit_vertex_attribute_type(sb_emitter_t* emitter, glsl_type_t type);

SC_API void sb_emitter_emit_vertex_attribute_name(sb_emitter_t* emitter, const char* name, u32 name_length);

/* does some post processing on the created vertex_attribute_info_t object */
SC_API void sb_emitter_close_vertex_attribute(sb_emitter_t* emitter);

SC_API void sb_emitter_close_vertex_attribute_array(sb_emitter_t* emitter);

/* VERTEX ATTRIBUTES <end> */

/* SHADER PROPERTIES <begin> */

SC_API void sb_emitter_open_shader_property_array(sb_emitter_t* emitter);

/* creates a new shader_property_info_t object into the vtx_attr_infos list */
SC_API void sb_emitter_open_shader_property(sb_emitter_t* emitter);

SC_API void sb_emitter_emit_shader_property_set_push_constant_offset(sb_emitter_t* emitter, u32 offset);

SC_API void sb_emitter_emit_shader_property_set_and_binding(sb_emitter_t* emitter, u32 set, u32 binding);

SC_API void sb_emitter_emit_shader_property_stage(sb_emitter_t* emitter, shader_stage_bits_t stages);

SC_API void sb_emitter_emit_shader_property_storage_class(sb_emitter_t* emitter, storage_class_t storage);

SC_API void sb_emitter_emit_shader_property_type(sb_emitter_t* emitter, glsl_type_t type);
SC_API void sb_emitter_emit_shader_property_udat_address(sb_emitter_t* emitter, codegen_buffer_address_t udat_address);
SC_API void sb_emitter_emit_shader_property_name(sb_emitter_t* emitter, const char* name, u32 name_length);

/* does some post processing on the created shader_property_info_t object */
SC_API void sb_emitter_close_shader_property(sb_emitter_t* emitter);

SC_API void sb_emitter_close_shader_property_array(sb_emitter_t* emitter);

/* SHADER PROPERTIES <end> */