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
	emitter->vtx_attr_infos = buf_new_with_callbacks(callbacks, vertex_attribute_info_t);
	return emitter;
}

SC_API void sb_emitter_destroy(sb_emitter_t* emitter)
{
	buf_free(&emitter->vtx_attr_infos);
	com_deallocate(emitter->callbacks, emitter);
}

#define VTX_ATTR_AT(emitter, index) CAST_TO(vertex_attribute_info_t*, buf_get_ptr_at(&(emitter)->vtx_attr_infos, index))

SC_API void sb_emitter_initialize(sb_emitter_t* emitter)
{
	binary_writer_u16_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT);
	emitter->is_initialized = true;
}

SC_API void sb_emitter_flush(sb_emitter_t* emitter)
{
	if(!emitter->is_initialized)
	{
		debug_log_info("sb_emitter_t hasn't been initialized, so skipping call to sb_emitter_flush");
		return;
	}

	u32 vtx_attr_count = buf_get_element_count(&emitter->vtx_attr_infos);
	binary_writer_u16_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT, CAST_TO(u16, vtx_attr_count));
	_ASSERT(vtx_attr_count < MARK_ID_VTX_ATTR_DSC_OFFSET_MAX);
}

SC_API void sb_emitter_open_vertex_attribute(sb_emitter_t* emitter)
{
	buf_push_pseudo(&emitter->vtx_attr_infos, 1);
}

#define VTX_ATTR(emitter) CAST_TO(vertex_attribute_info_t*, buf_peek_ptr(&(emitter)->vtx_attr_infos))

SC_API void sb_emitter_emit_vertex_bind_and_location(sb_emitter_t* emitter, u32 binding, u32 location)
{
	AUTO attr = VTX_ATTR(emitter);
	attr->binding = binding;
	attr->location = location;
}

SC_API void sb_emitter_emit_vertex_input_rate(sb_emitter_t* emitter, vertex_input_rate_t rate)
{
	_assert((rate == VERTEX_INPUT_RATE_VERTEX) || (rate == VERTEX_INPUT_RATE_INSTANCE));
	AUTO attr = VTX_ATTR(emitter);
	attr->rate = rate;
}

SC_API void sb_emitter_emit_vertex_attribute_type(sb_emitter_t* emitter, glsl_type_t type)
{
	AUTO attr = VTX_ATTR(emitter);
	attr->type = type;
}

SC_API void sb_emitter_emit_vertex_attribute_name(sb_emitter_t* emitter, const char* name, u32 name_length)
{
	AUTO attr = VTX_ATTR(emitter);
	if(name_length > VERTEX_ATTRIBUTE_NAME_MAX_SIZE)
		DEBUG_LOG_FETAL_ERROR("Vertex Attribute name \"%.*s\" is greater than VERTEX_ATTRIBUTE_NAME_MAX_SIZE(%u)" PRIu32, name_length, name, VERTEX_ATTRIBUTE_NAME_MAX_SIZE);
	memcpy(attr->name, name, name_length);
	attr->name_length = name_length;
}

SC_API void sb_emitter_close_vertex_attribute(sb_emitter_t* emitter)
{
	com_assert(buf_get_element_count(&emitter->vtx_attr_infos) > 0, "You haven't called sb_emitter_open_vertex_attribute() to create a vertex attribute info");
	AUTO attr = VTX_ATTR(emitter);
	u32 bits = 0;
	bits |= (attr->rate == VERTEX_INPUT_RATE_VERTEX) ? PER_VERTEX_ATTRIB_BIT : PER_INSTANCE_ATTRIB_BIT;
	bits |= attr->type;

	u32 index = buf_get_element_count(&emitter->vtx_attr_infos) - 1;
	binary_writer_u32_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + index);
	binary_writer_u32_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + index, binary_writer_pos(emitter->buffer->data));

	binary_writer_u8(emitter->buffer->data, CAST_TO(u8, attr->binding));
	binary_writer_u8(emitter->buffer->data, CAST_TO(u8, attr->location));
	binary_writer_u32(emitter->buffer->data, bits);
	binary_writer_stringn(emitter->buffer->data, attr->name, attr->name_length);
}
