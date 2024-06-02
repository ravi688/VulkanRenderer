#include <shader_compiler/sb_emitter.h>
#include <shader_compiler/compiler/codegen/layout.h>
#include <shader_compiler/assert.h>
#include <shader_compiler/debug.h>

SC_API sb_emitter_t* sb_emitter_create(com_allocation_callbacks_t* callbacks, codegen_buffer_t* buffer, sb_version_t version)
{
	sb_emitter_t* emitter = com_allocate_obj_init(callbacks, sb_emitter_t);
	emitter->callbacks = callbacks;
	emitter->buffer = buffer;
	emitter->version = version;
	return emitter;
}

SC_API void sb_emitter_destroy(sb_emitter_t* emitter)
{
	com_deallocate(emitter->callbacks, emitter);
}

SC_API void sb_emitter_open_vertex_attribute_array(sb_emitter_t* emitter)
{
	binary_writer_u16_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT);
	emitter->depth += 1;
}

SC_API void sb_emitter_close_vertex_attribute_array(sb_emitter_t* emitter)
{
	binary_writer_u16_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT, CAST_TO(u16, emitter->vtx_attr_count));
	_ASSERT(emitter->vtx_attr_count < MARK_ID_VTX_ATTR_DSC_OFFSET_MAX);

	binary_writer_unmark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT);

	emitter->vtx_attr_count = 0;
	emitter->depth -= 1;
}

SC_API void sb_emitter_open_vertex_attribute(sb_emitter_t* emitter)
{
	memset(&emitter->vtx_attr, 0, sizeof(vertex_attribute_info_t));
	emitter->depth += 1;
}

#define VTX_ATTR(emitter) CAST_TO(vertex_attribute_info_t*, buf_peek_ptr(&(emitter)->vtx_attr_infos))

SC_API void sb_emitter_emit_vertex_bind_and_location(sb_emitter_t* emitter, u32 binding, u32 location)
{
	emitter->vtx_attr.binding = binding;
	emitter->vtx_attr.location = location;
}

SC_API void sb_emitter_emit_vertex_input_rate(sb_emitter_t* emitter, vertex_input_rate_t rate)
{
	_assert((rate == VERTEX_INPUT_RATE_VERTEX) || (rate == VERTEX_INPUT_RATE_INSTANCE));
	emitter->vtx_attr.rate = rate;
}

SC_API void sb_emitter_emit_vertex_attribute_type(sb_emitter_t* emitter, glsl_type_t type)
{
	emitter->vtx_attr.type = type;
}

SC_API void sb_emitter_emit_vertex_attribute_name(sb_emitter_t* emitter, const char* name, u32 name_length)
{
	if(name_length > VERTEX_ATTRIBUTE_NAME_MAX_SIZE)
		DEBUG_LOG_FETAL_ERROR("Vertex Attribute name \"%.*s\" is greater than VERTEX_ATTRIBUTE_NAME_MAX_SIZE(%u)" PRIu32, name_length, name, VERTEX_ATTRIBUTE_NAME_MAX_SIZE);
	memcpy(emitter->vtx_attr.name, name, name_length);
	emitter->vtx_attr.name_length = name_length;
}

SC_API void sb_emitter_close_vertex_attribute(sb_emitter_t* emitter)
{
	com_assert(emitter->depth == 2, "You haven't called sb_emitter_open_vertex_attribute() and its predecessors to create a vertex attribute info");
	u32 bits = 0;
	bits |= (emitter->vtx_attr.rate == VERTEX_INPUT_RATE_VERTEX) ? PER_VERTEX_ATTRIB_BIT : PER_INSTANCE_ATTRIB_BIT;
	bits |= emitter->vtx_attr.type;

	binary_writer_u32_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + emitter->vtx_attr_counter);
	binary_writer_u32_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + emitter->vtx_attr_counter, binary_writer_pos(emitter->buffer->data));

	binary_writer_u8(emitter->buffer->data, CAST_TO(u8, emitter->vtx_attr.binding));
	binary_writer_u8(emitter->buffer->data, CAST_TO(u8, emitter->vtx_attr.location));
	binary_writer_u32(emitter->buffer->data, bits);
	binary_writer_stringn(emitter->buffer->data, emitter->vtx_attr.name, emitter->vtx_attr.name_length);

	emitter->vtx_attr_count += 1;
	emitter->vtx_attr_counter += 1;
	emitter->depth -= 1;
}
