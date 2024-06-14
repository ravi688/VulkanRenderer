
/* Implementation of memory_layout_provider_callbacks_t for GLSL memory layout */

#pragma once

#include <renderer/struct_descriptor.h> /* memory_layout_provider_callbacks_t */

u32 glsl_std140_get_type_align(u8 type, bool is_array);
u32 glsl_std140_get_type_size(u8 type);
u32 glsl_std140_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

u32 glsl_std430_get_type_align(u8 type, bool is_array);
u32 glsl_std430_get_type_size(u8 type);
u32 glsl_std430_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

u32 glsl_scalar_get_type_align(u8 type, bool is_array);
u32 glsl_scalar_get_type_size(u8 type);
u32 glsl_scalar_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_std140_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_std140_get_type_align,
		.get_type_size = glsl_std140_get_type_size,
		.get_struct_align = glsl_std140_get_struct_align
	};
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_std430_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_std430_get_type_align,
		.get_type_size = glsl_std430_get_type_size,
		.get_struct_align = glsl_std430_get_struct_align
	};
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE memory_layout_provider_callbacks_t glsl_scalar_memory_layout_callbacks()
{
	return (memory_layout_provider_callbacks_t)
	{
		.get_type_align = glsl_scalar_get_type_align,
		.get_type_size = glsl_scalar_get_type_size,
		.get_struct_align = glsl_scalar_get_struct_align
	};
}


#define GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT glsl_std140_memory_layout_callbacks()
#define GLSL_STORAGE_BUFFER_MEMORY_LAYOUT glsl_std430_memory_layout_callbacks()
#define GLSL_PUSH_CONSTANT_BUFFER_MEMORY_LAYOUT GLSL_STORAGE_BUFFER_MEMORY_LAYOUT
#define GLSL_MEMORY_LAYOUT_CALLBACKS GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT
