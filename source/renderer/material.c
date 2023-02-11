#include <renderer/material.h>
#include <renderer/renderer.h>

RENDERER_API material_t* material_new(memory_allocator_t* allocator)
{
	return vulkan_material_new(allocator);
}

RENDERER_API material_t* material_create(renderer_t* renderer, shader_t* shader)
{
	return vulkan_material_create(renderer->vulkan_handle, shader);
}

RENDERER_API void material_create_no_alloc(renderer_t* renderer, shader_t* shader, material_t* material)
{
	vulkan_material_create_no_alloc(renderer->vulkan_handle, shader, material);
}

RENDERER_API void material_destroy(material_t* material)
{
	vulkan_material_destroy(material);
}

RENDERER_API void material_release_resources(material_t* material)
{
	vulkan_material_release_resources(material);
}

RENDERER_API material_field_handle_t material_get_field_handle(material_t* material, const char* name)
{
	return vulkan_material_get_field_handle(material, name);	
}


/* ------------------------------------------ BEGIN: PUSH CONSTANTS -------------------------------------------*/

RENDERER_API void material_set_push_floatH(material_t* material, material_field_handle_t handle, float value)
{
	vulkan_material_set_push_floatH(material, handle, value);
}

RENDERER_API void material_set_push_intH(material_t* material, material_field_handle_t handle, int value)
{
	vulkan_material_set_push_intH(material, handle, value);
}

RENDERER_API void material_set_push_uintH(material_t* material, material_field_handle_t handle, uint value)
{
	vulkan_material_set_push_uintH(material, handle, value);
}

RENDERER_API void material_set_push_vec2H(material_t* material, material_field_handle_t handle, vec2_t value)
{
	vulkan_material_set_push_vec2H(material, handle, value);
}

RENDERER_API void material_set_push_vec3H(material_t* material, material_field_handle_t handle, vec3_t value)
{
	vulkan_material_set_push_vec3H(material, handle, value);
}

RENDERER_API void material_set_push_vec4H(material_t* material, material_field_handle_t handle, vec4_t value)
{
	vulkan_material_set_push_vec4H(material, handle, value);
}

RENDERER_API void material_set_push_mat2H(material_t* material, material_field_handle_t handle, mat2_t value)
{
	vulkan_material_set_push_mat2H(material, handle, value);
}

RENDERER_API void material_set_push_mat4H(material_t* material, material_field_handle_t handle, mat4_t value)
{
	vulkan_material_set_push_mat4H(material, handle, value);
}

// getters
RENDERER_API float material_get_push_floatH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_floatH(material, handle);
}

RENDERER_API int material_get_push_intH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_intH(material, handle);
}

RENDERER_API uint material_get_push_uintH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_uintH(material, handle);
}

RENDERER_API vec2_t material_get_push_vec2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec2H(material, handle);
}

RENDERER_API vec3_t material_get_push_vec3H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec3H(material, handle);
}

RENDERER_API vec4_t material_get_push_vec4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_vec4H(material, handle);
}

RENDERER_API mat2_t material_get_push_mat2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_mat2H(material, handle);
}

RENDERER_API mat4_t material_get_push_mat4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_push_mat4H(material, handle);
}

/* functions accepting strings */
RENDERER_API void material_set_push_float(material_t* material, const char* name, float value)
{
	vulkan_material_set_push_float(material, name, value);
}

RENDERER_API void material_set_push_int(material_t* material, const char* name, int value)
{
	vulkan_material_set_push_int(material, name, value);
}

RENDERER_API void material_set_push_uint(material_t* material, const char* name, uint value)
{
	vulkan_material_set_push_uint(material, name, value);
}

RENDERER_API void material_set_push_vec2(material_t* material, const char* name, vec2_t value)
{
	vulkan_material_set_push_vec2(material, name, value);
}

RENDERER_API void material_set_push_vec3(material_t* material, const char* name, vec3_t value)
{
	vulkan_material_set_push_vec3(material, name, value);
}

RENDERER_API void material_set_push_vec4(material_t* material, const char* name, vec4_t value)
{
	vulkan_material_set_push_vec4(material, name, value);
}

RENDERER_API void material_set_push_mat2(material_t* material, const char* name, mat2_t value)
{
	vulkan_material_set_push_mat2(material, name, value);
}

RENDERER_API void material_set_push_mat4(material_t* material, const char* name, mat4_t value)
{
	vulkan_material_set_push_mat4(material, name, value);
}

RENDERER_API float material_get_push_float(material_t* material, const char* name)
{
	return vulkan_material_get_push_float(material, name);
}

RENDERER_API int material_get_push_int(material_t* material, const char* name)
{
	return vulkan_material_get_push_int(material, name);
}

RENDERER_API uint material_get_push_uint(material_t* material, const char* name)
{
	return vulkan_material_get_push_uint(material, name);
}

RENDERER_API vec2_t material_get_push_vec2(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec2(material, name);
}

RENDERER_API vec3_t material_get_push_vec3(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec3(material, name);
}

RENDERER_API vec4_t material_get_push_vec4(material_t* material, const char* name)
{
	return vulkan_material_get_push_vec4(material, name);
}

RENDERER_API mat2_t material_get_push_mat2(material_t* material, const char* name)
{
	return vulkan_material_get_push_mat2(material, name);
}

RENDERER_API mat4_t material_get_push_mat4(material_t* material, const char* name)
{
	return vulkan_material_get_push_mat4(material, name);
}

/* ------------------------------------------ END: PUSH CONSTANTS -------------------------------------------*/

RENDERER_API void material_set_floatH(material_t* material, material_field_handle_t handle, float value)
{
	vulkan_material_set_floatH(material, handle, value);
}

RENDERER_API void material_set_intH(material_t* material, material_field_handle_t handle, int value)
{
	vulkan_material_set_intH(material, handle, value);
}

RENDERER_API void material_set_uintH(material_t* material, material_field_handle_t handle, uint value)
{
	vulkan_material_set_uintH(material, handle, value);
}

RENDERER_API void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t value)
{
	vulkan_material_set_vec2H(material, handle, value);
}

RENDERER_API void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t value)
{
	vulkan_material_set_vec3H(material, handle, value);
}

RENDERER_API void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t value)
{
	vulkan_material_set_vec4H(material, handle, value);
}

RENDERER_API void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t value)
{
	vulkan_material_set_mat2H(material, handle, value);
}

RENDERER_API void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t value)
{
	vulkan_material_set_mat4H(material, handle, value);
}

RENDERER_API void material_set_textureH(material_t* material, material_field_handle_t handle, texture_t* texture)
{
	vulkan_material_set_textureH(material, handle, texture);
}

RENDERER_API float material_get_floatH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_floatH(material, handle);
}

RENDERER_API int material_get_intH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_intH(material, handle);
}

RENDERER_API uint material_get_uintH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_intH(material, handle);
}

RENDERER_API vec2_t material_get_vec2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec2H(material, handle);
}

RENDERER_API vec3_t material_get_vec3H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec3H(material, handle);
}

RENDERER_API vec4_t material_get_vec4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_vec4H(material, handle);
}

RENDERER_API mat2_t material_get_mat2H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_mat2H(material, handle);
}

RENDERER_API mat4_t material_get_mat4H(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_mat4H(material, handle);
}

RENDERER_API texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle)
{
	return vulkan_material_get_texture2dH(material, handle);
}

/* functions accepting strings */
RENDERER_API void material_set_float(material_t* material, const char* name, float value)
{
	vulkan_material_set_float(material, name, value);
}

RENDERER_API void material_set_int(material_t* material, const char* name, int value)
{
	vulkan_material_set_int(material, name, value);
}

RENDERER_API void material_set_uint(material_t* material, const char* name, uint value)
{
	vulkan_material_set_uint(material, name, value);
}

RENDERER_API void material_set_vec2(material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_vec2(material, name, v);
}

RENDERER_API void material_set_vec3(material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_vec3(material, name, v);
}

RENDERER_API void material_set_vec4(material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_vec4(material, name, v);
}

RENDERER_API void material_set_mat2(material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_mat2(material, name, m);
}

RENDERER_API void material_set_mat4(material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_mat4(material, name, m);
}

RENDERER_API void material_set_texture(material_t* material, const char* name, texture_t* texture)
{
	vulkan_material_set_texture(material, name, texture);
}

RENDERER_API float material_get_float(material_t* material, const char* name)
{
	return vulkan_material_get_float(material, name);
}

RENDERER_API int material_get_int(material_t* material, const char* name)
{
	return vulkan_material_get_int(material, name);
}

RENDERER_API uint material_get_uint(material_t* material, const char* name)
{
	return vulkan_material_get_uint(material, name);
}

RENDERER_API vec2_t material_get_vec2(material_t* material, const char* name)
{
	return vulkan_material_get_vec2(material, name);
}

RENDERER_API vec3_t material_get_vec3(material_t* material, const char* name)
{
	return vulkan_material_get_vec3(material, name);
}

RENDERER_API vec4_t material_get_vec4(material_t* material, const char* name)
{
	return vulkan_material_get_vec4(material, name);
}

RENDERER_API mat2_t material_get_mat2(material_t* material, const char* name)
{
	return vulkan_material_get_mat2(material, name);
}

RENDERER_API mat4_t material_get_mat4(material_t* material, const char* name)
{
	return vulkan_material_get_mat4(material, name);
}

RENDERER_API texture_t* material_get_texture2d(material_t* material, const char* name)
{
	return vulkan_material_get_texture2d(material, name);
}
