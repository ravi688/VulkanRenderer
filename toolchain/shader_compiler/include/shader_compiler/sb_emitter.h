#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h> /* codegen_buffer_t */
#include <shader_compiler/compiler/codegen/header.h> /* sb_version_t */
#include <glslcommon/glsl_types.h> /* glsl_type_t */
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

#define VERTEX_ATTRIBUTE_NAME_MAX_SIZE 64
#define SHADER_PROPERTY_NAME_MAX_SIZE 64

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

typedef struct shader_property_info_t shader_property_info_t;

typedef buffer_t /* shader_property_info_t */shader_property_info_list_t;

typedef struct shader_property_info_t
{
	/* it is not null terminated */
	char name[SHADER_PROPERTY_NAME_MAX_SIZE];
	char block_name[SHADER_PROPERTY_NAME_MAX_SIZE];
	u32 name_length;
	u32 block_name_length;
	shader_stage_bits_t stages;
	union
	{
		u32 set;
		/* push constant offset */
		u32 offset;
	};
	u32 binding;
	glsl_type_t type;
	storage_class_t storage;
	/* is this shader propery push constant? */
	shader_property_info_list_t fields;
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
	/* just a counter (increments upon call to sb_emitter_close_vertex_attribute), to generate unique mark ids for vertex attribute offsets */
	u32 vtx_attr_counter;
	/* just a counter (increments upon call to sb_emitter_close_shader_property), to generate unique mark ids for shader property offsets */
	u32 shr_prop_counter;
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

SC_API void sb_emitter_emit_shader_property_name(sb_emitter_t* emitter, const char* name, u32 name_length);
SC_API void sb_emitter_emit_shader_property_block_name(sb_emitter_t* emitter, const char* block_name, u32 block_name_length);

SC_API void sb_emitter_open_shader_property_field_array(sb_emitter_t* emitter);
SC_API void sb_emitter_open_shader_property_field(sb_emitter_t* emitter);
SC_API void sb_emitter_emit_shader_property_field_type(sb_emitter_t* emitter, glsl_type_t type);
SC_API void sb_emitter_emit_shader_property_field_name(sb_emitter_t* emitter, const char* name, u32 name_length);
SC_API void sb_emitter_emit_shader_property_field_array_size(sb_emitter_t* emitter, u32 array_size);
SC_API void sb_emitter_close_shader_property_field(sb_emitter_t* emitter);
SC_API void sb_emitter_close_shader_property_field_array(sb_emitter_t* emitter);

/* does some post processing on the created shader_property_info_t object */
SC_API void sb_emitter_close_shader_property(sb_emitter_t* emitter);

SC_API void sb_emitter_close_shader_property_array(sb_emitter_t* emitter);

/* SHADER PROPERTIES <end> */