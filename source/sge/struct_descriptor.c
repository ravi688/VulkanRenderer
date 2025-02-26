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


#include <sge/struct_descriptor.h>
#include <sge/assert.h>
#include <sge/debug.h>
#include <sge/memory_allocator.h>
#include <common/alloc.h>
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

SGE_API struct_descriptor_t* struct_descriptor_create(memory_allocator_t* allocator)
{
	struct_descriptor_t* descriptor = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_STRUCT_DESCRIPTOR, struct_descriptor_t);
	memzero(descriptor, struct_descriptor_t);
	OBJECT_INIT(descriptor, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_INTERNAL);
	descriptor->allocator = allocator;
	return descriptor;
}

static field_type_align_t field_type_align_getter(void* user_data, u32 index)
{	
	AUTO descriptor = CAST_TO(struct_descriptor_t*, user_data);
	_debug_assert__(index < descriptor->field_count);
	AUTO field = &descriptor->fields[index];
	if(field->record != NULL)
	{
		return (field_type_align_t)
		{
			.type = 0,
			.is_array = field->is_array,
			.align = struct_descriptor_alignof(field->record),
			.size = struct_descriptor_is_variable_sized(field->record) ? 0 : struct_descriptor_sizeof(field->record)
		};
	}
	return (field_type_align_t)
	{
		.type = field->type,
		.is_array = field->is_array
	};
}

SGE_API u32 struct_field_array_get_stride(struct_field_t* field)
{
	_debug_assert__(field->is_array);
	return com_get_stride_in_array(field->size, field->alignment);
}

static u32 get_array_size(struct_field_t* field)
{
	_debug_assert__(field->is_array);
	_debug_assert__(field->array_size != U32_MAX);
	return struct_field_array_get_stride(field) * field->array_size;
}

SGE_API void struct_descriptor_recalculate(struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	if((descriptor->fields == 0) || (descriptor->field_count == 0) || (descriptor->field_count == 0xFFFF))
		return;
	struct_field_t* fields = descriptor->fields;
	u32 offset = 0;
	for(u16 i = 0; i < descriptor->field_count; i++)
	{
		u16 field_align = fields[i].alignment;
		fields[i].offset = ((field_align - (offset % field_align)) % field_align) + offset;
		if(fields[i].is_array && (fields[i].array_size == U32_MAX))
		{
			_debug_assert__(i == (descriptor->field_count - 1u));
			break;
		}
		else offset = fields[i].offset + (fields[i].is_array ? get_array_size(&fields[i]) : fields[i].size);
	}
	descriptor->align = descriptor->layout_callbacks.get_struct_align(field_type_align_getter, descriptor, descriptor->field_count, false);
	descriptor->size = COM_GET_STRIDE_IN_ARRAY(offset, descriptor->align);
}

SGE_API void struct_descriptor_map(struct_descriptor_t* descriptor, void* ptr)
{
	_debug_assert__(descriptor != NULL);
	descriptor->ptr = ptr;
}

SGE_API void* struct_descriptor_get_mapped(struct_descriptor_t* descriptor)
{
	return descriptor->ptr;
}

SGE_API void struct_descriptor_unmap(struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	descriptor->ptr = NULL;
}

SGE_API u32 struct_descriptor_sizeof(const struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(!struct_descriptor_is_variable_sized(descriptor));
	return descriptor->size;
}

SGE_API u32 struct_descriptor_min_sizeof(const struct_descriptor_t* descriptor)
{
	return descriptor->size;
}

SGE_API u32 struct_descriptor_alignof(const struct_descriptor_t* descriptor)
{
	_debug_assert__(descriptor != NULL);
	return descriptor->align;
}

SGE_API u32 struct_descriptor_offsetof(const struct_descriptor_t* descriptor, const char* name)
{
	AUTO handle = struct_descriptor_get_field_handle(descriptor, name);
	return BIT64_UNPACK32(handle, 1);
}

SGE_API bool struct_descriptor_is_variable_sized(const struct_descriptor_t* descriptor)
{
	for(u32 i = 0; i < descriptor->field_count; i++)
	{
		AUTO field = descriptor->fields[i];
		if((field.is_array && (field.array_size == U32_MAX)) || (field.record != NULL && struct_descriptor_is_variable_sized(field.record)))
			return true;
	}
	return false;
}

SGE_API void* struct_field_get_base_address(struct_descriptor_t* descriptor, struct_field_t* field)
{
	if(descriptor->ptr == NULL)
	{
		debug_assert__(false, "Struct Descriptor %s is not mapped to any memory", descriptor->name);
		return NULL;
	}
	return descriptor->ptr + field->offset;
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

SGE_API struct_field_handle_t struct_descriptor_get_field_handle(const struct_descriptor_t* descriptor, const char* name)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(descriptor->field_count < 0xFFFF);
	_debug_assert__(name != NULL);

	const char* ptr = strchr(name, '.');
	AUTO field = get_field(CAST_TO(struct_descriptor_t*, descriptor), name, (ptr == NULL) ? strlen(name) : (ptr - name));

	if(field == NULL)
		return STRUCT_FIELD_INVALID_HANDLE;

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

SGE_API struct_field_t* struct_descriptor_get_field(struct_descriptor_t* descriptor, const char* name)
{
	_debug_assert__(descriptor != NULL);
	_debug_assert__(descriptor->field_count < 0xFFFF);
	_debug_assert__(name != NULL);

	const char* ptr = strchr(name, '.');
	AUTO field = get_field(descriptor, name, (ptr == NULL) ? strlen(name) : (ptr - name));

	release_assert__(field != NULL, "The field \"%s\" is not found", name);

	/* if this is the last name in the fully qualified name */
	if(ptr == NULL)
		return field;

	_debug_assert__(field->record != NULL);
	
	/* get the field from next level records recursively */
	return struct_descriptor_get_field(field->record, ptr + 1);
}

SGE_API void struct_descriptor_set_array_size(struct_descriptor_t* descriptor, const char* name, u32 size)
{
	struct_field_t* field = struct_descriptor_get_field(descriptor, name);
	struct_field_set_array_size(descriptor, field, size);
}

SGE_API void struct_field_set_array_size(struct_descriptor_t* descriptor, struct_field_t* field, u32 size)
{
	_debug_assert__(field != NULL);
	release_assert__(field->is_array, "The field \"%s\" is not an array so it couldn't be variable sized", field->name);
	field->array_size = size;
	struct_descriptor_recalculate(descriptor);
}

SGE_API u32 struct_descriptor_get_array_size(struct_descriptor_t* descriptor, const char* name)
{
	struct_field_t* field = struct_descriptor_get_field(descriptor, name);
	_debug_assert__(field != NULL);
	release_assert__(field->is_array, "The field \"%s\" is not an array so it couldn't be variable sized", name);
	return field->array_size;
}

#define cpy_data_from(descriptor, handle, data) __cpy_data_from(descriptor, handle, data)
#define cpy_data_to(descriptor, handle, data) __cpy_data_to(descriptor, handle, data)

static inline void __cpy_data_from(struct_descriptor_t* descriptor, struct_field_handle_t handle, const void* const data)
{
	/* the struct_descriptor_t object must be mapped to some memory first */
	if(!descriptor->ptr)
	{
		debug_log_error("Unhandled error, Failed to copy data from source, Struct descriptor isn't mapped");
		return;
	}
	if(handle != STRUCT_FIELD_INVALID_HANDLE)
		memcopyv(descriptor->ptr + BIT64_UNPACK32(handle, 1), data, u8, BIT64_UNPACK32(handle, 0));
	else
		debug_log_error("Unhandled error, Failed to copy data from source, Struct field handle is invalid");
}

static inline void __cpy_data_to(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const data)
{
	/* the struct_descriptor_t object must be mapped to some memory first */
	if(!descriptor->ptr)
	{
		debug_log_error("Unhandled error, Failed to copy data at destination, Struct descriptor isn't mapped");
		return;
	}
	if(handle != STRUCT_FIELD_INVALID_HANDLE)
		memcopyv(data, descriptor->ptr + BIT64_UNPACK32(handle, 1), u8, BIT64_UNPACK32(handle, 0));
	else
		debug_log_error("Unhandled error, Failed to copy data at destination, Struct field handle is invalid");
}

SGE_API void struct_descriptor_set_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, const void* const in)
{
	cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_set_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in)
{
	__cpy_data_from(descriptor, handle, in);
}

SGE_API void struct_descriptor_get_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const out)
{
	cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
}

SGE_API void struct_descriptor_get_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out)
{
	__cpy_data_to(descriptor, handle, out);
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

static struct_field_t* prvt_add_field(struct_descriptor_t* descriptor, const char* name, u8 type, bool is_array)
{
	if(struct_descriptor_is_variable_sized(descriptor))
		debug_log_fetal_error("The interface layout descriptor is variable sized, so no more fields can be added");
	struct_field_t* field = create_field(descriptor);
	prvt_strncpy(field->name, name);
	field->type = type;
	field->size = descriptor->layout_callbacks.get_type_size(type);
	field->alignment = descriptor->layout_callbacks.get_type_align(type, is_array);
	field->record = NULL;
	return field;
}

SGE_API void struct_descriptor_add_field(struct_descriptor_t* descriptor, const char* name, u8 type)
{
	prvt_add_field(descriptor, name, type, false);
}

static struct_field_t* prvt_add_field2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record, bool is_array)
{
	if(struct_descriptor_is_variable_sized(descriptor))
		debug_log_fetal_error("The interface layout descriptor is variable sized, so no more fields can be added");
	struct_field_t* field = create_field(descriptor);
	prvt_strncpy(field->name, name);
	field->type = record->type;
	struct_descriptor_recalculate(record);
	field->size = struct_descriptor_sizeof(record);
	if(is_array)
		field->alignment = record->layout_callbacks.get_struct_align(field_type_align_getter, record, record->field_count, true);
	else 
		field->alignment = struct_descriptor_alignof(record);
	field->record = record;
	return field;
}

SGE_API void struct_descriptor_add_field2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record)
{
	prvt_add_field2(descriptor, name, record, false);
}

SGE_API void struct_descriptor_add_field_array(struct_descriptor_t* descriptor, const char* name, u8 type, u32 array_size)
{
	struct_field_t* field = prvt_add_field(descriptor, name, type, true);
	field->is_array = true;
	field->array_size = array_size;
}

SGE_API void struct_descriptor_add_field_array2(struct_descriptor_t* descriptor, const char* name, struct_descriptor_t* record, u32 array_size)
{
	release_assert__(!struct_descriptor_is_variable_sized(record), "The record \"%s\" is variable sized record so it can't be declared as an array at all", record->name);
	struct_field_t* field = prvt_add_field2(descriptor, name, record, true);
	field->is_array = true;
	field->array_size = array_size;	
}

SGE_API void struct_descriptor_begin(memory_allocator_t* allocator, struct_descriptor_t* descriptor, const char* name, u8 type, memory_layout_provider_callbacks_t layout_callbacks)
{
	OBJECT_MEMZERO(descriptor, struct_descriptor_t);
	descriptor->allocator = allocator;
	prvt_strncpy(descriptor->name, name);
	descriptor->type = type;
	descriptor->layout_callbacks = layout_callbacks;
	descriptor->fields = NULL;
}

SGE_API void struct_descriptor_end(memory_allocator_t* allocator, struct_descriptor_t* descriptor)
{
	/* NOTE: check for buffer if it is NULL is needed, as it is lazily created */
	BUFFER* buffer = CAST_TO(BUFFER*, descriptor->fields);
	descriptor->fields = (buffer == NULL) ? NULL : buf_get_ptr(buffer);
	descriptor->field_count = (buffer == NULL) ? 0U : buf_get_element_count(buffer);
	if(buffer != NULL)
		memory_allocator_dealloc(allocator, buffer);
	struct_descriptor_recalculate(descriptor);
}

SGE_API void struct_descriptor_set_name(struct_descriptor_t* descriptor, const char* name)
{
	prvt_strncpy(descriptor->name, name);
}

SGE_API void struct_descriptor_destroy(memory_allocator_t* allocator, struct_descriptor_t* descriptor)
{
	/* NOTE: it is allowed to call struct_descriptor_destroy even if it doesn't have any fields in it.
	 * that's because, we use struct_descriptor to even represent a single opaque variable in GLSL, 
	 * so, such struct_descriptor_t instances may not have non-zero field counts. */
	if(descriptor->field_count > 0)
	{
		for(u32 i = 0; i < descriptor->field_count; i++)
		{
			struct_field_t field = descriptor->fields[i];
			if(field.record != NULL)
				struct_descriptor_destroy(allocator, field.record);
		}
		memory_allocator_dealloc(allocator, descriptor->fields);
	}
	if(OBJECT_IS_INTERNAL(descriptor))
		memory_allocator_dealloc(allocator, descriptor);
}

static void _struct_descriptor_clone(struct_descriptor_t* descriptor, struct_descriptor_t* const clone)
{
	/* copy bit-wise */
	OBJECT_MEMCOPY(clone, descriptor, struct_descriptor_t);

	/* if there are fields in the descriptor */
	if(descriptor->field_count > 0)
	{
		/* iterate over each fields and perform bit-wise copy */
		clone->fields = memory_allocator_alloc_obj_array(descriptor->allocator, MEMORY_ALLOCATION_TYPE_OBJ_STRUCT_FIELD_ARRAY, struct_field_t, descriptor->field_count);
		for(u16 i = 0; i < descriptor->field_count; i++)
		{
			memcopy(&clone->fields[i], &descriptor->fields[i], struct_field_t);
			/* if this field is another struct descriptor, then call the clone function recursively */
			if(descriptor->fields[i].record != NULL)
				clone->fields[i].record = struct_descriptor_clone_p(descriptor->fields[i].record);
		}
	}
}

SGE_API struct_descriptor_t* struct_descriptor_clone_p(struct_descriptor_t* descriptor)
{
	AUTO clone = struct_descriptor_create(descriptor->allocator);
	_struct_descriptor_clone(descriptor, clone);
	return clone;
}

SGE_API struct_descriptor_t struct_descriptor_clone(struct_descriptor_t* descriptor)
{
	struct_descriptor_t clone;
	OBJECT_INIT(&clone, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	_struct_descriptor_clone(descriptor, &clone);
	return clone;
}