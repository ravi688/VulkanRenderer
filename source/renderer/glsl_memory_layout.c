#include <renderer/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>

u32 glsl_std140_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_STD140);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD140);
}

u32 glsl_std140_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD140);
}

typedef struct glsl_type_traits_callback_user_data_t
{ 
	struct_field_type_getter_t type_getter;
	void* user_data;
} glsl_type_traits_callback_user_data_t;

static glsl_type_layout_traits_t type_layout_traits_callback_handler(void* user_data, u32 type_index)
{
	AUTO _user_data = CAST_TO(glsl_type_traits_callback_user_data_t*, user_data);
	field_type_align_t type_align = _user_data->type_getter(_user_data->user_data, type_index);
	return (glsl_type_layout_traits_t)
	{
		.type = (type_align.type == 0) ? GLSL_TYPE_UNDEFINED : type_align.type,
		.align = type_align.align,
		.size = type_align.size
	};
}

u32 glsl_std140_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD140);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD140);
}

u32 glsl_std430_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_STD430);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD430);
}
u32 glsl_std430_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_STD430);
}
u32 glsl_std430_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD430);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_STD430);
}

u32 glsl_scalar_get_type_align(u8 type, bool is_array)
{
	if(is_array)
		return alignof_glsl_type_array(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
	return alignof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
}
u32 glsl_scalar_get_type_size(u8 type)
{
	return sizeof_glsl_type(CAST_TO(glsl_type_t, type), GLSL_SCALAR);
}
u32 glsl_scalar_get_struct_align(struct_field_type_getter_t getter, void* user_data, u32 field_count, bool is_array)
{
	glsl_type_traits_callback_user_data_t _user_data = { getter, user_data };
	if(is_array)
		return alignof_glsl_type_struct_array(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_SCALAR);
	return alignof_glsl_type_struct(type_layout_traits_callback_handler, &_user_data, field_count, GLSL_SCALAR);
}