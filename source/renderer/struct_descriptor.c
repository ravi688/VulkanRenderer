
#include <renderer/struct_descriptor.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <renderer/alloc.h>
#include <string.h>
#include <bufferlib/buffer.h>

/*
 		COMPUTING PADDING
		The following formulas provide the number of padding bytes required to align the start of a data structure 
		(where mod is the modulo operator):
		padding = (align - (offset mod align)) mod align
		aligned = offset + padding
        		= offset + ((align - (offset mod align)) mod align)
        Since the alignment is by definition a power of two,[a] the modulo operation can be reduced to a bitwise boolean AND operation.
        padding = (align - (offset & (align - 1))) & (align - 1)
        		= (-offset & (align - 1))
		aligned = (offset + (align - 1)) & ~(align - 1)
        		= (offset + (align - 1)) & -align
 */

RENDERER_API void struct_descriptor_recalculate(struct_descriptor_t* descriptor)
{
	assert(descriptor != NULL);
	if((descriptor->fields == 0) || (descriptor->field_count == 0) || (descriptor->field_count == 0xFFFF))
		return;
	struct_field_t* fields = descriptor->fields;
	u32 offset = 0;
	descriptor->size = 0;
	for(u16 i = 0; i < descriptor->field_count; i++)
	{
		u16 align = fields[i].alignment;
		fields[i].offset = ((align - (offset % align)) % align) + offset;
		offset = fields[i].offset + fields[i].size;
	}
	descriptor->size = offset;
}

RENDERER_API void struct_descriptor_map(struct_descriptor_t* descriptor, void* ptr)
{
	assert(descriptor != NULL);
	descriptor->ptr = ptr;
}

RENDERER_API void struct_descriptor_unmap(struct_descriptor_t* descriptor)
{
	assert(descriptor != NULL);
	descriptor->ptr = NULL;
}

RENDERER_API u32 struct_descriptor_sizeof(struct_descriptor_t* descriptor)
{
	assert(descriptor != NULL);
	return descriptor->size;
}

RENDERER_API struct_field_handle_t struct_descriptor_get_field_handle(struct_descriptor_t* descriptor, const char* field_name)
{
	assert(descriptor != NULL);
	assert(descriptor->field_count < 0xFFFF);
	for(u16 i = 0; i < descriptor->field_count; i++)
		if(strcmp(descriptor->fields[i].name, field_name) == 0)
			return i;
	LOG_WRN("Returning STRUCT_FIELD_INVALID_HANDLE, field_name: %s\n", field_name);
	return STRUCT_FIELD_INVALID_HANDLE;
}

#ifndef GLOBAL_DEBUG
#	define check_precondition(descriptor, handle)
#else
	static void check_precondition(struct_descriptor_t* descriptor, struct_field_handle_t handle);
#endif

#define cpy_data_from(descriptor, handle, data) __cpy_data_from(descriptor, handle, data, descriptor->fields[handle].size)
#define cpy_data_to(descriptor, handle, data) __cpy_data_to(descriptor, handle, data, descriptor->fields[handle].size)

static inline void __cpy_data_from(struct_descriptor_t* descriptor, struct_field_handle_t handle, const void* const data, u16 size)
{
	memcpy(descriptor->ptr + descriptor->fields[handle].offset, data, size);
}

static inline void __cpy_data_to(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const data, u16 size)
{
	memcpy(data, descriptor->ptr + descriptor->fields[handle].offset, size);
}

RENDERER_API void struct_descriptor_set_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, const void* const in)
{
	check_precondition(descriptor, handle);
	cpy_data_from(descriptor, handle, in);
}

RENDERER_API void struct_descriptor_set_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_FLOAT_SIZE);
}

RENDERER_API void struct_descriptor_set_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_INT_SIZE);
}

RENDERER_API void struct_descriptor_set_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_UINT_SIZE);
}

RENDERER_API void struct_descriptor_set_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_VEC4_SIZE);
}

RENDERER_API void struct_descriptor_set_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_VEC3_SIZE);
}

RENDERER_API void struct_descriptor_set_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_VEC2_SIZE);
}

RENDERER_API void struct_descriptor_set_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_IVEC4_SIZE);
}

RENDERER_API void struct_descriptor_set_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_IVEC3_SIZE);
}

RENDERER_API void struct_descriptor_set_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_IVEC2_SIZE);
}

RENDERER_API void struct_descriptor_set_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_UVEC4_SIZE);
}

RENDERER_API void struct_descriptor_set_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_UVEC3_SIZE);
}

RENDERER_API void struct_descriptor_set_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_UVEC2_SIZE);
}

RENDERER_API void struct_descriptor_set_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_MAT4_SIZE);
}

RENDERER_API void struct_descriptor_set_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_MAT3_SIZE);
}

RENDERER_API void struct_descriptor_set_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	check_precondition(descriptor, handle);
	__cpy_data_from(descriptor, handle, in, GLSL_TYPE_MAT2_SIZE);
}

RENDERER_API void struct_descriptor_get_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const out)
{
	check_precondition(descriptor, handle);
	cpy_data_to(descriptor, handle, out);
}

RENDERER_API void struct_descriptor_get_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_FLOAT_SIZE);
}

RENDERER_API void struct_descriptor_get_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_INT_SIZE);
}

RENDERER_API void struct_descriptor_get_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_UINT_SIZE);
}

RENDERER_API void struct_descriptor_get_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_VEC4_SIZE);
}

RENDERER_API void struct_descriptor_get_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_VEC3_SIZE);
}

RENDERER_API void struct_descriptor_get_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_VEC2_SIZE);
}

RENDERER_API void struct_descriptor_get_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_IVEC4_SIZE);
}

RENDERER_API void struct_descriptor_get_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_IVEC3_SIZE);
}

RENDERER_API void struct_descriptor_get_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_IVEC2_SIZE);
}

RENDERER_API void struct_descriptor_get_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_UVEC4_SIZE);
}

RENDERER_API void struct_descriptor_get_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_UVEC3_SIZE);
}

RENDERER_API void struct_descriptor_get_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_UVEC2_SIZE);
}

RENDERER_API void struct_descriptor_get_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_MAT4_SIZE);
}

RENDERER_API void struct_descriptor_get_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_MAT3_SIZE);
}

RENDERER_API void struct_descriptor_get_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	check_precondition(descriptor, handle);
	__cpy_data_to(descriptor, handle, out, GLSL_TYPE_MAT2_SIZE);
}

static struct_field_t* create_field(struct_descriptor_t* descriptor)
{
	BUFFER* fields = CAST_TO(BUFFER*, descriptor->fields);
	buf_push_pseudo(fields, 1);
	return buf_peek_ptr(fields);
}

RENDERER_API void struct_descriptor_add_field(struct_descriptor_t* descriptor, const char* name, u8 type)
{
	struct_field_t* field = create_field(descriptor);
	strcpy(field->name, name);
	field->type = type;
	field->size = sizeof_glsl_type(type);
	field->alignment = alignof_glsl_type(type);
}

RENDERER_API void struct_descriptor_begin(struct_descriptor_t* descriptor, const char* name, u8 type)
{
	memzero(descriptor, struct_descriptor_t);
	
	strcpy(descriptor->name, name);
	descriptor->type = type;

	BUFFER* buffer = heap_new(BUFFER);
	BUFFER _buffer = buf_create(sizeof(struct_field_t), 1, 0);
	memcpy(buffer, &_buffer, sizeof(BUFFER));
	descriptor->fields = CAST_TO(struct_field_t*, buffer);
}

RENDERER_API void struct_descriptor_end(struct_descriptor_t* descriptor)
{
	BUFFER* buffer = CAST_TO(BUFFER*, descriptor->fields);
	descriptor->fields = buf_get_ptr(buffer);
	descriptor->field_count = buf_get_element_count(buffer);
	heap_free(buffer);
	struct_descriptor_recalculate(descriptor);
}


#ifdef GLOBAL_DEBUG
static void check_precondition(struct_descriptor_t* descriptor, struct_field_handle_t handle)
{
	assert(descriptor != NULL);
	assert(descriptor->ptr != NULL);
	assert(descriptor->fields != NULL);
	assert(handle < descriptor->field_count);
}
#endif
