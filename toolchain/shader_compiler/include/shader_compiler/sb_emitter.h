#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h> /* codegen_buffer_t */
#include <shader_compiler/compiler/codegen/header.h> /* sb_version_t */
#include <glslcommon/glsl_types.h> /* glsl_type_t */

typedef enum vertex_input_rate_t
{
	VERTEX_INPUT_RATE_VERTEX = 1,
	VERTEX_INPUT_RATE_INSTANCE = 2
} vertex_input_rate_t;

#define VERTEX_ATTRIBUTE_NAME_MAX_SIZE 64

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

typedef struct sb_emitter_t
{
	com_allocation_callbacks_t* callbacks;
	sb_version_t version;
	codegen_buffer_t* buffer;
	/* number of vertex attribute in the current Layout { } block, it will be reset to zero upon calling sb_emitter_open_vertex_attribute_array() function */
	u32 vtx_attr_count;
	/* just a counter (increments upon call to sb_emitter_close_vertex_attribute), to generate unique mark ids for vertex attribute count*/
	u32 vtx_attr_counter;
	s32 depth;
	union
	{
		vertex_attribute_info_t vtx_attr;
	};
} sb_emitter_t;

SC_API sb_emitter_t* sb_emitter_create(com_allocation_callbacks_t* callbacks, codegen_buffer_t* buffer, sb_version_t version);
SC_API void sb_emitter_destroy(sb_emitter_t* emitter);

/* it must be called after writing the header and material descriptors to the codegen buffer, otherwise we might get a corrupted shader binary at the end */
SC_API void sb_emitter_initialize(sb_emitter_t* emitter);

/* writes to the codegen_buffer_t respecting the shader binary version specified at the time of creating sb_emitter_t object */
SC_API void sb_emitter_flush(sb_emitter_t* emitter);

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
