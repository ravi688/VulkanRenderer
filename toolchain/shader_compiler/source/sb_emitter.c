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
	_com_assert(emitter->depth == 0);
	emitter->vtx_attr_dsc_cnt_addr = codegen_buffer_alloc_u16(emitter->buffer, ".main");
	// binary_writer_u16_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT);
	emitter->depth += 1;
}

SC_API void sb_emitter_close_vertex_attribute_array(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 1);
	codegen_buffer_set_u16(emitter->buffer, emitter->vtx_attr_dsc_cnt_addr, CAST_TO(u16, emitter->vtx_attr_count));
	// binary_writer_u16_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT, CAST_TO(u16, emitter->vtx_attr_count));
	_ASSERT(emitter->vtx_attr_count < MARK_ID_VTX_ATTR_DSC_OFFSET_MAX);

	// binary_writer_unmark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_COUNT);

	emitter->vtx_attr_count = 0;
	emitter->depth -= 1;
}

SC_API void sb_emitter_open_vertex_attribute(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 1);
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

	AUTO addr = codegen_buffer_get_end_address(emitter->buffer, ".data");
	codegen_buffer_write_pointer(emitter->buffer, ".main", addr);

	// binary_writer_u32_mark(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + emitter->vtx_attr_counter);
	// binary_writer_u32_set(emitter->buffer->main, MARK_ID_VTX_ATTR_DSC_OFFSET + emitter->vtx_attr_counter, binary_writer_pos(emitter->buffer->data));

	codegen_buffer_write_u8(emitter->buffer, ".data", CAST_TO(u8, emitter->vtx_attr.binding));
	codegen_buffer_write_u8(emitter->buffer, ".data", CAST_TO(u8, emitter->vtx_attr.location));
	codegen_buffer_write_u32(emitter->buffer, ".data", bits);
	codegen_buffer_write_stringn(emitter->buffer, ".data", emitter->vtx_attr.name, emitter->vtx_attr.name_length);

	// binary_writer_u8(emitter->buffer->data, CAST_TO(u8, emitter->vtx_attr.binding));
	// binary_writer_u8(emitter->buffer->data, CAST_TO(u8, emitter->vtx_attr.location));
	// binary_writer_u32(emitter->buffer->data, bits);
	// binary_writer_stringn(emitter->buffer->data, emitter->vtx_attr.name, emitter->vtx_attr.name_length);

	emitter->vtx_attr_count += 1;
	emitter->vtx_attr_counter += 1;
	emitter->depth -= 1;
}


SC_API void sb_emitter_open_shader_property_array(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 0);
	emitter->shr_prop_dsc_cnt_addr = codegen_buffer_alloc_u16(emitter->buffer, ".main");
	// binary_writer_u16_mark(emitter->buffer->main, MARK_ID_SHR_PROP_DSC_COUNT);
	emitter->depth += 1;
}

/* creates a new shader_property_info_t object into the vtx_attr_infos list */
SC_API void sb_emitter_open_shader_property(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 1);
	memset(&emitter->shr_prop, 0, sizeof(shader_property_info_t));
	emitter->shr_prop.fields = buf_new_with_callbacks(emitter->callbacks, shader_property_info_t);
	emitter->depth += 1;
}

SC_API void sb_emitter_emit_shader_property_set_push_constant_offset(sb_emitter_t* emitter, u32 offset)
{
	emitter->shr_prop.offset = offset;
}

SC_API void sb_emitter_emit_shader_property_set_and_binding(sb_emitter_t* emitter, u32 set, u32 binding)
{
	emitter->shr_prop.set = set;
	emitter->shr_prop.binding = binding;
}

SC_API void sb_emitter_emit_shader_property_stage(sb_emitter_t* emitter, shader_stage_bits_t stages)
{
	emitter->shr_prop.stages |= stages;
}

SC_API void sb_emitter_emit_shader_property_storage_class(sb_emitter_t* emitter, storage_class_t storage)
{
	emitter->shr_prop.storage = storage;
}

SC_API void sb_emitter_emit_shader_property_type(sb_emitter_t* emitter, glsl_type_t type)
{
	emitter->shr_prop.type = type;
}

SC_API void sb_emitter_emit_shader_property_name(sb_emitter_t* emitter, const char* name, u32 name_length)
{
	if(name_length > SHADER_PROPERTY_NAME_MAX_SIZE)
		DEBUG_LOG_FETAL_ERROR("Shader Property name \"%.*s\" is greater than SHADER_PROPERTY_NAME_MAX_SIZE(%u)" PRIu32, name_length, name, SHADER_PROPERTY_NAME_MAX_SIZE);
	memcpy(emitter->shr_prop.name, name, name_length);
	emitter->shr_prop.name_length = name_length;
}

SC_API void sb_emitter_emit_shader_property_block_name(sb_emitter_t* emitter, const char* block_name, u32 block_name_length)
{
	if(block_name_length > SHADER_PROPERTY_NAME_MAX_SIZE)
		DEBUG_LOG_FETAL_ERROR("Shader Property Block name \"%.*s\" is greater than SHADER_PROPERTY_NAME_MAX_SIZE(%u)" PRIu32, block_name_length, block_name, SHADER_PROPERTY_NAME_MAX_SIZE);
	memcpy(emitter->shr_prop.block_name, block_name, block_name_length);
	emitter->shr_prop.block_name_length = block_name_length;
}

SC_API void sb_emitter_open_shader_property_field_array(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 2);
	emitter->depth += 1;
}
SC_API void sb_emitter_open_shader_property_field(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 3);
	emitter->depth += 1;	
	buf_push_pseudo(&emitter->shr_prop.fields, 1);
}
#define GET_SHR_PROP_FIELD(emitter) CAST_TO(shader_property_info_t*, buf_peek_ptr(&(emitter)->shr_prop.fields))
SC_API void sb_emitter_emit_shader_property_field_type(sb_emitter_t* emitter, glsl_type_t type)
{
	_com_assert(emitter->depth == 4);
	GET_SHR_PROP_FIELD(emitter)->type = type;
}
SC_API void sb_emitter_emit_shader_property_field_name(sb_emitter_t* emitter, const char* name, u32 name_length)
{
	if(name_length > SHADER_PROPERTY_NAME_MAX_SIZE)
		DEBUG_LOG_FETAL_ERROR("Shader Property Field name \"%.*s\" is greater than SHADER_PROPERTY_NAME_MAX_SIZE(%u)" PRIu32, name_length, name, SHADER_PROPERTY_NAME_MAX_SIZE);
	AUTO field = GET_SHR_PROP_FIELD(emitter);
	memcpy(field->name, name, name_length);
	field->name_length = name_length;
}
SC_API void sb_emitter_emit_shader_property_field_array_size(sb_emitter_t* emitter, u32 array_size)
{
	_com_assert(array_size != 0);
	AUTO field = GET_SHR_PROP_FIELD(emitter);
	field->array_size = array_size;
}
SC_API void sb_emitter_close_shader_property_field(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 4);
	emitter->depth -= 1;
}
SC_API void sb_emitter_close_shader_property_field_array(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 3);
	emitter->depth -= 1;
}

static u32 get_encoded_type_info(shader_property_info_t* info)
{
	u32 bits = 0;
	if(HAS_FLAG(info->stages, SHADER_STAGE_BIT_VERTEX))
		bits |= VERTEX_BIT;
	if(HAS_FLAG(info->stages, SHADER_STAGE_BIT_TESSELLATION))
		bits |= TESSELLATION_BIT;
	if(HAS_FLAG(info->stages, SHADER_STAGE_BIT_GEOMETRY))
		bits |= GEOMETRY_BIT;
	if(HAS_FLAG(info->stages, SHADER_STAGE_BIT_FRAGMENT))
		bits |= FRAGMENT_BIT;
	if(info->storage == STORAGE_CLASS_BUFFER)
	{
		_com_assert(info->type );
		bits |= STORAGE_BUFFER_BIT;
	}
	else if(info->storage == STORAGE_CLASS_UNIFORM)
	{
		if(info->type == GLSL_TYPE_PUSH_CONSTANT)
			bits |= PUSH_CONSTANT_BIT;
		else if(info->type == GLSL_TYPE_UNIFORM_BUFFER) 
			bits |= UNIFORM_BUFFER_BIT;
	}
	bits |= info->type;
	switch(info->type)
	{
		case GLSL_TYPE_SAMPLER_2D:
		case GLSL_TYPE_SAMPLER_3D:
		case GLSL_TYPE_SAMPLER_CUBE:
		case GLSL_TYPE_SUBPASS_INPUT:
		{
			bits |= OPAQUE_BIT;
			break;
		}
		default: { break; }
	}
	return bits;
}

#define IS_SHR_PROP_BLOCK(shr_prop_ptr) (((shr_prop_ptr)->type == GLSL_TYPE_STORAGE_BUFFER) || ((shr_prop_ptr)->type == GLSL_TYPE_UNIFORM_BUFFER) || ((shr_prop_ptr)->type == GLSL_TYPE_PUSH_CONSTANT))

/* does some post processing on the created shader_property_info_t object */
SC_API void sb_emitter_close_shader_property(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 2);

	AUTO addr = codegen_buffer_get_end_address(emitter->buffer, ".data");
	codegen_buffer_write_pointer(emitter->buffer, ".main", addr);

	// binary_writer_u32_mark(emitter->buffer->main, MARK_ID_SHR_PROP_DSC_OFFSET + emitter->shr_prop_counter);
	// binary_writer_u32_set(emitter->buffer->main, MARK_ID_SHR_PROP_DSC_OFFSET + emitter->shr_prop_counter, binary_writer_pos(emitter->buffer->data));

	if(emitter->shr_prop.type == GLSL_TYPE_PUSH_CONSTANT)
		codegen_buffer_write_u8(emitter->buffer, ".data", CAST_TO(u8, emitter->shr_prop.offset));
	else
	{
		codegen_buffer_write_u8(emitter->buffer, ".data", CAST_TO(u8, emitter->shr_prop.set));
		codegen_buffer_write_u8(emitter->buffer, ".data", CAST_TO(u8, emitter->shr_prop.binding));
	}
	u32 bits = get_encoded_type_info(&emitter->shr_prop);
	codegen_buffer_write_u32(emitter->buffer, ".data", bits);
	if(IS_SHR_PROP_BLOCK(&emitter->shr_prop))
		codegen_buffer_write_stringn(emitter->buffer, ".data", emitter->shr_prop.block_name, emitter->shr_prop.block_name_length);
	codegen_buffer_write_stringn(emitter->buffer, ".data", emitter->shr_prop.name, emitter->shr_prop.name_length);

	if(IS_SHR_PROP_BLOCK(&emitter->shr_prop))
	{
		u32 field_count = buf_get_element_count(&emitter->shr_prop.fields);
		codegen_buffer_write_u16(emitter->buffer, ".data", field_count);
		for(u32 i = 0; i < field_count; i++)
		{
			AUTO field = buf_get_ptr_at_typeof(&emitter->shr_prop.fields, shader_property_info_t, i);
			u32 bits = get_encoded_type_info(field);
			codegen_buffer_write_u32(emitter->buffer, ".data", bits);
			codegen_buffer_write_stringn(emitter->buffer, ".data", field->name, field->name_length);
		}
	}

	buf_free(&emitter->shr_prop.fields);

	emitter->shr_prop_count += 1;
	emitter->shr_prop_counter += 1;
	emitter->depth -= 1;
}

SC_API void sb_emitter_close_shader_property_array(sb_emitter_t* emitter)
{
	_com_assert(emitter->depth == 1);
	codegen_buffer_set_u16(emitter->buffer, emitter->shr_prop_dsc_cnt_addr, CAST_TO(u16, emitter->shr_prop_count));
	// binary_writer_u16_set(emitter->buffer->main, MARK_ID_SHR_PROP_DSC_COUNT, CAST_TO(u16, emitter->shr_prop_count));
	_ASSERT(emitter->shr_prop_count < MARK_ID_SHR_PROP_DSC_OFFSET_MAX);

	// binary_writer_unmark(emitter->buffer->main, MARK_ID_SHR_PROP_DSC_COUNT);

	emitter->shr_prop_count = 0;
	emitter->depth -= 1;
}

/* SHADER PROPERTIES <end> */
