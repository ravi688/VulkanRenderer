/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: struct_descriptor.c is a part of VulkanRenderer

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


#include <renderer/struct_descriptor.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>
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
	_debug_assert__(descriptor != NULL);
	if((descriptor->fields == 0) || (descriptor->field_count == 0) || (descriptor->field_count == 0xFFFF))
		return;
	struct_field_t* fields = descriptor->fields;
	u32 offset = 0;
	u32 align = 0;
	for(u16 i = 0; i < descriptor->field_count; i++)
	{
		u16 field_align = fields[i].alignment;
		if(align < field_align)
			align = field_align;
		fields[i].offset = ((field_align - (offset % field_align)) % field_align) + offset;
		offset = fields[i].offset + fields[i].size;
	}
	descriptor->size = offset;
	descriptor->align = align;
}

RENDERER_API void struct_descriptor_map(struct_descriptor_t* descriptor, void* ptr)
{
	_debug_assert__(descriptor != NULL);
	descriptor->ptr = ptr;
}

RENDERER_API void* struct_descriptor_get_mapped(struct_descriptor_t* descriptor)
{
	return descriptor->ptr;
}

RENDERER_API void struct_descriptor_unmap(struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	descriptor->ptr = NULL;
}

RENDERER_API u32 struct_descriptor_sizeof(struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	return descriptor->size;
}

RENDERER_API u32 struct_descriptor_alignof(struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	return descriptor->align;
}

RENDERER_API bool struct_descriptor_is_variable_sized(struct_descriptor_t* descriptor)
{
	for(u32 i = 0; i < descriptor->field_count; i++)
	{
		AUTO field = descriptor->fields[i];
		if((field.is_array && (field.array_size == U32_MAX)) || (field.record != NULL && struct_descriptor_is_variable_sized(field.record)))
			return true;
	}
	return false;
}

static struct_field_t* get_field(struct_descriptor_t* descriptor, const char* name, u32 name_len)
{
	/* get the index of the field */
	u16 index = 0; 
	bool is_found = false;
	while(index < descriptor->field_count)
	{
		if(strncmp(descriptor->fields[index].name, name, name_len) == 0)
		{
			is_found = true;
			break;
		}
		++index;
	}

	/* if not such field is found then return invalid handle */
	if(!is_found)
		return NULL;

	return &descriptor->fields[index];
}

RENDERER_API struct_field_handle_t struct_descriptor_get_field_handle(struct_descriptor_t* descriptor, const char* name)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(descriptor->field_count < 0xFFFF);
	_debug_assert__(name != NULL);

	const char* ptr = strchr(name, '.');
	AUTO field = get_field(descriptor, name, (ptr == NULL) ? strlen(name) : (ptr - name));

	if(field == NULL)
	{
		debug_log_warning("Returning STRUCT_FIELD_INVALID_HANDLE, name: %s", name);
		return STRUCT_FIELD_INVALID_HANDLE;
	}

	/* if this field is not a record then just return it's offset and size */
	if(field->record == NULL)
	{
		release_assert__(!((field->is_array) && (field->array_size == U32_MAX)), "You can't get a handle to a variable sized field, first make it fixed sized");
		
		if(field->is_array)
			return BIT64_PACK32(field->offset, field->size * field->array_size);

		return BIT64_PACK32(field->offset, field->size);
	}

	release_assert__((field->record == NULL) || (!struct_descriptor_is_variable_sized(field->record)), "You can't get a handle to a variable sized field, first make it fixed sized");
	
	/* otherwise get the offset and size recursively */
	u64 handle = struct_descriptor_get_field_handle(field->record, ptr + 1);
	if(handle == STRUCT_FIELD_INVALID_HANDLE)
		return handle;
	return BIT64_PACK32(field->offset + BIT64_UNPACK32(handle, 1), BIT64_UNPACK32(handle, 0));
}

RENDERER_API void struct_descriptor_set_array_size(struct_descriptor_t* descriptor, const char* name, u32 size)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(descriptor->field_count < 0xFFFF);
	_debug_assert__(name != NULL);

	const char* ptr = strchr(name, '.');
	AUTO field = get_field(descriptor, name, (ptr == NULL) ? strlen(name) : (ptr - name));

	release_assert__(field != NULL, "The field \"%s\" is not found", name);

	/* if this is the last name in the fully qualified name */
	if(ptr == NULL)
	{
		release_assert__(field->is_array, "The field \"%s\" is not an array so it couldn't be variable sized", name);
		field->array_size = size;
		return;
	}

	release_assert__(!struct_descriptor_is_variable_sized(field->record), "The field \"%s\" is variable sized record so it can't be declared as an array at all", name);
	
	/* set array size recursively */
	struct_descriptor_set_array_size(field->record, ptr + 1, size);
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
	_release_assert__(BIT64_UNPACK32(handle, 0) == size);
	memcopyv(descriptor->ptr + BIT64_UNPACK32(handle, 1), data, u8, BIT64_UNPACK32(handle, 0));
}

static inline void __cpy_data_to(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const data, u16 size)
{
	_release_assert__(BIT64_UNPACK32(handle, 0) == size);
	memcopyv(data, descriptor->ptr + BIT64_UNPACK32(handle, 1), u8, BIT64_UNPACK32(handle, 0));
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
	/* lazily allocate buffer for storing the fields, 
	 * that's because, we use struct_descriptor_t even for representing non-block single opaque variables in GLSL 
	 * so, it is good to not to allocate any memory unless needed */
	if(descriptor->fields == NULL)
	{
		BUFFER* buffer = memory_allocator_alloc_obj(descriptor->allocator, MEMORY_ALLOCATION_TYPE_OBJ_BUFFER, BUFFER);
		BUFFER _buffer = memory_allocator_buf_create(descriptor->allocator, sizeof(struct_field_t), 1, 0);
		memcopy(buffer, &_buffer, BUFFER);
		descriptor->fields = CAST_TO(struct_field_t*, buffer);
	}

	BUFFER* fields = CAST_TO(BUFFER*, descriptor->fields);
	buf_push_pseudo(fields, 1);
	return buf_peek_ptr(fields);
}

static void prvt_strncpy(char* const dst, const char* src)
{
	_debug_assert__(strlen(src) < STRUCT_MAX_NAME_SIZE);
	strncpy(dst, src, STRUCT_MAX_NAME_SIZE);
}

static struct_field_t* prvt_add_field(struct_descriptor_t* descriptor, const char* name, u8 type)
{
	if(struct_descriptor_is_variable_sized(descriptor))
		debug_log_fetal_error("The interface layout descriptor is variable sized, so no more fields can be added");
	struct_field_t* field = create_field(descriptor);
	prvt_strncpy(field->name, name);
	field->type = type;
	field->size = sizeof_glsl_type(type);
	field->alignment = alignof_glsl_type(type);
	field->record = NULL;
	return field;
}

RENDERER_API void struct_descriptor_add_field(struct_descriptor_t* descriptor, const char* name, u8 type)
{
	prvt_add_field(descriptor, name, type);
}

static struct_field_t* prvt_add_field2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record)
{
	if(struct_descriptor_is_variable_sized(descriptor))
		debug_log_fetal_error("The interface layout descriptor is variable sized, so no more fields can be added");
	struct_field_t* field = create_field(descriptor);
	prvt_strncpy(field->name, name);
	field->type = record->type;
	struct_descriptor_recalculate(record);
	field->size = struct_descriptor_sizeof(record);
	field->alignment = struct_descriptor_alignof(record);
	field->record = record;
	return field;
}

RENDERER_API void struct_descriptor_add_field2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record)
{
	prvt_add_field2(descriptor, name, record);
}

RENDERER_API void struct_descriptor_add_field_array(struct_descriptor_t* descriptor, const char* name, u8 type, u32 array_size)
{
	struct_field_t* field = prvt_add_field(descriptor, name, type);
	field->is_array = true;
	field->array_size = array_size;
}

RENDERER_API void struct_descriptor_add_field_array2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record, u32 array_size)
{
	struct_field_t* field = prvt_add_field2(descriptor, name, record);
	field->is_array = true;
	field->array_size = array_size;	
}

RENDERER_API void struct_descriptor_begin(memory_allocator_t* allocator, struct_descriptor_t* descriptor, const char* name, u8 type)
{
	memzero(descriptor, struct_descriptor_t);
	descriptor->allocator = allocator;
	prvt_strncpy(descriptor->name, name);
	descriptor->type = type;
	descriptor->fields = NULL;
}

RENDERER_API void struct_descriptor_end(memory_allocator_t* allocator, struct_descriptor_t* descriptor)
{
	/* NOTE: check for buffer if it is NULL is needed, as it is lazily created */
	BUFFER* buffer = CAST_TO(BUFFER*, descriptor->fields);
	descriptor->fields = (buffer == NULL) ? NULL : buf_get_ptr(buffer);
	descriptor->field_count = (buffer == NULL) ? 0U : buf_get_element_count(buffer);
	if(buffer != NULL)
		memory_allocator_dealloc(allocator, buffer);
	struct_descriptor_recalculate(descriptor);
}

RENDERER_API void struct_descriptor_free(memory_allocator_t* allocator, struct_descriptor_t* descriptor)
{
	/* NOTE: it is allowed to call struct_descriptor_free even if it doesn't have any fields in it.
	 * that's because, we use struct_descriptor to even represent a single opaque variable in GLSL, 
	 * so, such struct_descriptor_t instances may not have non-zero field counts. */
	if(descriptor->field_count > 0)
	{
		for(u32 i = 0; i < descriptor->field_count; i++)
		{
			struct_field_t field = descriptor->fields[i];
			if(field.record != NULL)
				struct_descriptor_free(allocator, field.record);
		}
		memory_allocator_dealloc(allocator, descriptor->fields);
	}
}


#ifdef GLOBAL_DEBUG
static void check_precondition(struct_descriptor_t* descriptor, struct_field_handle_t handle)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(descriptor->ptr != NULL);
	_debug_assert__(descriptor->fields != NULL);
	// _debug_assert__(handle < descriptor->field_count);
}
#endif
