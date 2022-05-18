
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
#include <renderer/internal/vulkan/vulkan_material.h>
typedef struct vulkan_texture_t vulkan_texture_t;
typedef vulkan_texture_t texture_t;
typedef vulkan_shader_t shader_t;
typedef vukan_material_t material_t;
typedef vulkan_material_field_handle_t material_field_handle_t;
#endif

#ifdef RENDERER_OPENGL_DRIVER
#error "OpenGL is not supported yet!"
#endif

#ifdef RENDERER_DIRECTX_DRIVER
#error "DirectX is not supported yet!"
#endif

#ifdef RENDERER_METAL_DRIVER
#error "Metal is not supported yet!"
#endif

#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/mat2/header_config.h>
#include <hpml/mat2/mat2.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API material_t* material_new();
RENDERER_API material_t* material_create(renderer_t* renderer, shader_t* shader);
RENDERER_API void material_create_no_alloc(renderer_t* renderer, shader_t* shader, material_t* material);
RENDERER_API void material_destroy(material_t* material);
RENDERER_API void material_release_resources(material_t* material);

RENDERER_API material_field_handle_t material_get_field_handle(material_t* material, const char* name);

RENDERER_API void material_set_push_floatH(material_t* material, material_field_handle_t handle, float value);
RENDERER_API void material_set_push_intH(material_t* material, material_field_handle_t handle, int value);
RENDERER_API void material_set_push_uintH(material_t* material, material_field_handle_t handle, uint value);
RENDERER_API void material_set_push_vec2H(material_t* material, material_field_handle_t handle, vec2_t(float) value);
RENDERER_API void material_set_push_vec3H(material_t* material, material_field_handle_t handle, vec3_t(float) value);
RENDERER_API void material_set_push_vec4H(material_t* material, material_field_handle_t handle, vec4_t(float) value);
RENDERER_API void material_set_push_mat2H(material_t* material, material_field_handle_t handle, mat2_t(float) value);
RENDERER_API void material_set_push_mat4H(material_t* material, material_field_handle_t handle, mat4_t(float) value);

RENDERER_API void material_set_push_float(material_t* material, const char* name, float value);
RENDERER_API void material_set_push_int(material_t* material, const char* name, int value);
RENDERER_API void material_set_push_uint(material_t* material, const char* name, uint value);
RENDERER_API void material_set_push_vec2(material_t* material, const char* name, vec2_t(float) v);
RENDERER_API void material_set_push_vec3(material_t* material, const char* name, vec3_t(float) v);
RENDERER_API void material_set_push_vec4(material_t* material, const char* name, vec4_t(float) v);
RENDERER_API void material_set_push_mat2(material_t* material, const char* name, mat2_t(float) m);
RENDERER_API void material_set_push_mat4(material_t* material, const char* name, mat4_t(float) m);

RENDERER_API float material_get_push_floatH(material_t* material, material_field_handle_t handle);
RENDERER_API int material_get_push_intH(material_t* material, material_field_handle_t handle);
RENDERER_API uint material_get_push_uintH(material_t* material, material_field_handle_t handle);
RENDERER_API vec2_t(float) material_get_push_vec2H(material_t* material, material_field_handle_t handle);
RENDERER_API vec3_t(float) material_get_push_vec3H(material_t* material, material_field_handle_t handle);
RENDERER_API vec4_t(float) material_get_push_vec4H(material_t* material, material_field_handle_t handle);
RENDERER_API mat2_t(float) material_get_push_mat2H(material_t* material, material_field_handle_t handle);
RENDERER_API mat4_t(float) material_get_push_mat4H(material_t* material, material_field_handle_t handle);

RENDERER_API float material_get_push_float(material_t* material, const char* name);
RENDERER_API int material_get_push_int(material_t* material, const char* name);
RENDERER_API uint material_get_push_uint(material_t* material, const char* name);
RENDERER_API vec2_t(float) material_get_push_vec2(material_t* material, const char* name);
RENDERER_API vec3_t(float) material_get_push_vec3(material_t* material, const char* name);
RENDERER_API vec4_t(float) material_get_push_vec4(material_t* material, const char* name);
RENDERER_API mat2_t(float) material_get_push_mat2(material_t* material, const char* name);
RENDERER_API mat4_t(float) material_get_push_mat4(material_t* material, const char* name);


RENDERER_API void material_set_floatH(material_t* material, material_field_handle_t handle, float value);
RENDERER_API void material_set_intH(material_t* material, material_field_handle_t handle, int value);
RENDERER_API void material_set_uintH(material_t* material, material_field_handle_t handle, uint value);
RENDERER_API void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t(float) v);
RENDERER_API void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t(float) v);
RENDERER_API void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t(float) v);
RENDERER_API void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t(float) m);
RENDERER_API void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t(float) m);
RENDERER_API void material_set_textureH(material_t* material, material_field_handle_t handle, texture_t* texture);

RENDERER_API void material_set_float(material_t* material, const char* name, float value);
RENDERER_API void material_set_int(material_t* material, const char* name, int value);
RENDERER_API void material_set_uint(material_t* material, const char* name, uint value);
RENDERER_API void material_set_vec2(material_t* material, const char* name, vec2_t(float) v);
RENDERER_API void material_set_vec3(material_t* material, const char* name, vec3_t(float) v);
RENDERER_API void material_set_vec4(material_t* material, const char* name, vec4_t(float) v);
RENDERER_API void material_set_mat2(material_t* material, const char* name, mat2_t(float) m);
RENDERER_API void material_set_mat4(material_t* material, const char* name, mat4_t(float) m);
RENDERER_API void material_set_texture(material_t* material, const char* name, texture_t* texture);


RENDERER_API float material_get_floatH(material_t* material, material_field_handle_t handle);
RENDERER_API int material_get_intH(material_t* material, material_field_handle_t handle);
RENDERER_API uint material_get_uintH(material_t* material, material_field_handle_t handle);
RENDERER_API vec2_t(float) material_get_vec2H(material_t* material, material_field_handle_t handle);
RENDERER_API vec3_t(float) material_get_vec3H(material_t* material, material_field_handle_t handle);
RENDERER_API vec4_t(float) material_get_vec4H(material_t* material, material_field_handle_t handle);
RENDERER_API mat2_t(float) material_get_mat2H(material_t* material, material_field_handle_t handle);
RENDERER_API mat4_t(float) material_get_mat4H(material_t* material, material_field_handle_t handle);
RENDERER_API texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle);

RENDERER_API float material_get_float(material_t* material, const char* name);
RENDERER_API int material_get_int(material_t* material, const char* name);
RENDERER_API uint material_get_uint(material_t* material, const char* name);
RENDERER_API vec2_t(float) material_get_vec2(material_t* material, const char* name);
RENDERER_API vec3_t(float) material_get_vec3(material_t* material, const char* name);
RENDERER_API vec4_t(float) material_get_vec4(material_t* material, const char* name);
RENDERER_API mat2_t(float) material_get_mat2(material_t* material, const char* name);
RENDERER_API mat4_t(float) material_get_mat4(material_t* material, const char* name);
RENDERER_API texture_t* material_get_texture2d(material_t* material, const char* name);

END_CPP_COMPATIBLE
