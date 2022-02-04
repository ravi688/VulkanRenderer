
#pragma once

typedef struct renderer_t renderer_t;
typedef struct material_t material_t;

#ifdef RENDERER_VULKAN_DRIVER
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef vulkan_texture_t texture_t;
typedef vulkan_shader_t shader_t;
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

#include <renderer/struct_descriptor.h>

#define MATERIAL_ALIGN(value, index) ((value) << ((index) * 5))

enum 
{
	MATERIAL_U8 	= 1ULL,
	MATERIAL_U16 	= 2ULL,
	MATERIAL_U32	= 3ULL,
	MATERIAL_U64 	= 4ULL,
	MATERIAL_S8 	= 5ULL,
	MATERIAL_S16 	= 6ULL,
	MATERIAL_S32 	= 7ULL,
	MATERIAL_S64 	= 8ULL,
	MATERIAL_F32 	= 9ULL,
	MATERIAL_VEC2 	= 10ULL,
	MATERIAL_VEC3 	= 11ULL,
	MATERIAL_VEC4 	= 12ULL,
	MATERIAL_MAT2 	= 13ULL,
	MATERIAL_MAT3 	= 14ULL,
	MATERIAL_MAT4	= 15ULL,

	MATERIAL_IVEC2 	= 16ULL,
	MATERIAL_IVEC3 	= 17ULL,
	MATERIAL_IVEC4 	= 18ULL,
	MATERIAL_UVEC2 	= 19ULL,
	MATERIAL_UVEC3 	= 20ULL,
	MATERIAL_UVEC4 	= 21ULL,

	MATERIAL_POSITION = MATERIAL_ALIGN(MATERIAL_VEC3, 0),
	MATERIAL_NORMAL = MATERIAL_ALIGN(MATERIAL_VEC3, 1),
	MATERIAL_COLOR = MATERIAL_ALIGN(MATERIAL_VEC3, 2),
	MATERIAL_TEXCOORD = MATERIAL_ALIGN(MATERIAL_VEC2, 3)
};

typedef struct material_create_info_t
{
	u64* per_vertex_attribute_bindings;
	u64* per_instance_attribute_bindings;
	shader_t* shader;
	u32 per_vertex_attribute_binding_count;
	u32 per_instance_attribute_binding_count;
	bool is_transparent;
} material_create_info_t;

// constructors and destructors
material_t* material_new();
material_t* material_create(renderer_t* renderer, material_create_info_t* create_info);
void material_create_no_alloc(renderer_t* renderer, material_create_info_t* create_info, material_t* material);
void material_destroy(material_t* material);
void material_release_resources(material_t* material);

void material_bind(material_t* material);

typedef struct material_field_handle_t
{
	u16 descriptor_index;
	struct_field_handle_t field_handle;
} material_field_handle_t;

material_field_handle_t material_get_field_handle(material_t* material, const char* name);

void material_set_push_floatH(material_t* material, material_field_handle_t handle, float value);
void material_set_push_intH(material_t* material, material_field_handle_t handle, int value);
void material_set_push_uintH(material_t* material, material_field_handle_t handle, uint value);
void material_set_push_vec2H(material_t* material, material_field_handle_t handle, vec2_t(float) value);
void material_set_push_vec3H(material_t* material, material_field_handle_t handle, vec3_t(float) value);
void material_set_push_vec4H(material_t* material, material_field_handle_t handle, vec4_t(float) value);
void material_set_push_mat2H(material_t* material, material_field_handle_t handle, mat2_t(float) value);
void material_set_push_mat4H(material_t* material, material_field_handle_t handle, mat4_t(float) value);

void material_set_push_float(material_t* material, const char* name, float value);
void material_set_push_int(material_t* material, const char* name, int value);
void material_set_push_uint(material_t* material, const char* name, uint value);
void material_set_push_vec2(material_t* material, const char* name, vec2_t(float) v);
void material_set_push_vec3(material_t* material, const char* name, vec3_t(float) v);
void material_set_push_vec4(material_t* material, const char* name, vec4_t(float) v);
void material_set_push_mat2(material_t* material, const char* name, mat2_t(float) m);
void material_set_push_mat4(material_t* material, const char* name, mat4_t(float) m);

float material_get_push_floatH(material_t* material, material_field_handle_t handle);
int material_get_push_intH(material_t* material, material_field_handle_t handle);
uint material_get_push_uintH(material_t* material, material_field_handle_t handle);
vec2_t(float) material_get_push_vec2H(material_t* material, material_field_handle_t handle);
vec3_t(float) material_get_push_vec3H(material_t* material, material_field_handle_t handle);
vec4_t(float) material_get_push_vec4H(material_t* material, material_field_handle_t handle);
mat2_t(float) material_get_push_mat2H(material_t* material, material_field_handle_t handle);
mat4_t(float) material_get_push_mat4H(material_t* material, material_field_handle_t handle);

float material_get_push_float(material_t* material, const char* name);
int material_get_push_int(material_t* material, const char* name);
uint material_get_push_uint(material_t* material, const char* name);
vec2_t(float) material_get_push_vec2(material_t* material, const char* name);
vec3_t(float) material_get_push_vec3(material_t* material, const char* name);
vec4_t(float) material_get_push_vec4(material_t* material, const char* name);
mat2_t(float) material_get_push_mat2(material_t* material, const char* name);
mat4_t(float) material_get_push_mat4(material_t* material, const char* name);


void material_set_floatH(material_t* material, material_field_handle_t handle, float value);
void material_set_intH(material_t* material, material_field_handle_t handle, int value);
void material_set_uintH(material_t* material, material_field_handle_t handle, uint value);
void material_set_vec2H(material_t* material, material_field_handle_t handle, vec2_t(float) v);
void material_set_vec3H(material_t* material, material_field_handle_t handle, vec3_t(float) v);
void material_set_vec4H(material_t* material, material_field_handle_t handle, vec4_t(float) v);
void material_set_mat2H(material_t* material, material_field_handle_t handle, mat2_t(float) m);
void material_set_mat4H(material_t* material, material_field_handle_t handle, mat4_t(float) m);
void material_set_textureH(material_t* material, material_field_handle_t handle, texture_t* texture);

void material_set_float(material_t* material, const char* name, float value);
void material_set_int(material_t* material, const char* name, int value);
void material_set_uint(material_t* material, const char* name, uint value);
void material_set_vec2(material_t* material, const char* name, vec2_t(float) v);
void material_set_vec3(material_t* material, const char* name, vec3_t(float) v);
void material_set_vec4(material_t* material, const char* name, vec4_t(float) v);
void material_set_mat2(material_t* material, const char* name, mat2_t(float) m);
void material_set_mat4(material_t* material, const char* name, mat4_t(float) m);
void material_set_texture(material_t* material, const char* name, texture_t* texture);


float material_get_floatH(material_t* material, material_field_handle_t handle);
int material_get_intH(material_t* material, material_field_handle_t handle);
uint material_get_uintH(material_t* material, material_field_handle_t handle);
vec2_t(float) material_get_vec2H(material_t* material, material_field_handle_t handle);
vec3_t(float) material_get_vec3H(material_t* material, material_field_handle_t handle);
vec4_t(float) material_get_vec4H(material_t* material, material_field_handle_t handle);
mat2_t(float) material_get_mat2H(material_t* material, material_field_handle_t handle);
mat4_t(float) material_get_mat4H(material_t* material, material_field_handle_t handle);
texture_t* material_get_texture2dH(material_t* material, material_field_handle_t handle);

float material_get_float(material_t* material, const char* name);
int material_get_int(material_t* material, const char* name);
uint material_get_uint(material_t* material, const char* name);
vec2_t(float) material_get_vec2(material_t* material, const char* name);
vec3_t(float) material_get_vec3(material_t* material, const char* name);
vec4_t(float) material_get_vec4(material_t* material, const char* name);
mat2_t(float) material_get_mat2(material_t* material, const char* name);
mat4_t(float) material_get_mat4(material_t* material, const char* name);
texture_t* material_get_texture2d(material_t* material, const char* name);
