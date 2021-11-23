#pragma once


#ifdef MESH3d_REQUIRE_PRINT
#define PRINT_REQUIRE_ARRAY_PRINT
#include <print.h>
#endif

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>

#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <buffer.h>

#include <renderer/defines.h>
#include <renderer/debug.h>

typedef u32 index_t;

typedef struct
{
	LIST(vec3_t(float)) positions; 
	LIST(vec2_t(float)) uvs;
	LIST(vec3_t(float)) normals;
	LIST(index_t) triangles;
} mesh3d_t;

function_signature_void(mesh3d_t*, mesh3d_new);
function_signature_void(mesh3d_t*, mesh3d_get_binded);
function_signature(void, mesh3d_bind, mesh3d_t*);
function_signature_void(void, mesh3d_unbind);
function_signature_void(void, mesh3d_push_binded);
function_signature_void(void, mesh3d_pop_binded);
function_signature_void(void, mesh3d_destroy);

function_signature(void, mesh3d_positions_new, u32 count);
function_signature(void, mesh3d_normals_new, u32 count);
function_signature(void, mesh3d_triangles_new, u32 count);
function_signature(void, mesh3d_uvs_new, u32 count);
function_signature_void(void, mesh3d_positions_bind);
function_signature_void(void, mesh3d_normals_bind);
function_signature_void(void, mesh3d_triangles_bind);
function_signature_void(void, mesh3d_uvs_bind);

function_signature_void(bool, mesh3d_has_uvs); 
function_signature_void(bool, mesh3d_has_positions); 
function_signature_void(bool, mesh3d_has_normals); 
function_signature_void(bool, mesh3d_has_triangles);

function_signature_void(void, mesh3d_calculate_normals);
function_signature_void(void, mesh3d_optimize_buffer);

function_signature(void, mesh3d_position_add, float x, float y, float z);
function_signature(void, mesh3d_position_add_vec3, vec3_t(float) position);
function_signature(void, mesh3d_position_set, index_t index, float x, float y, float z);
function_signature(void, mesh3d_position_set_vec3, index_t index, vec3_t(float) position);
function_signature(void, mesh3d_position_set_x, index_t index, float x);
function_signature(void, mesh3d_position_set_y, index_t index, float y);
function_signature(void, mesh3d_position_set_z, index_t index, float z);
function_signature(vec3_t(float), mesh3d_position_get, index_t index);
function_signature(vec3_t(float)*, mesh3d_position_get_ptr, index_t index);
function_signature(float, mesh3d_position_get_x, index_t index);
function_signature(float, mesh3d_position_get_y, index_t index);
function_signature(float, mesh3d_position_get_z, index_t index);

function_signature(void, mesh3d_normal_add, float x, float y, float z);
function_signature(void, mesh3d_normal_add_vec3, vec3_t(float) normal);
function_signature(void, mesh3d_normal_set, index_t index, float x, float y, float z);
function_signature(void, mesh3d_normal_set_vec3, index_t index, vec3_t(float)  normal);
function_signature(void, mesh3d_normal_set_x, index_t index, float x);
function_signature(void, mesh3d_normal_set_y, index_t index, float y); 
function_signature(void, mesh3d_normal_set_z, index_t index, float z);
function_signature(vec3_t(float), mesh3d_normal_get, index_t index);
function_signature(vec3_t(float)*, mesh3d_normal_get_ptr, index_t index);
function_signature(float, mesh3d_normal_get_x, index_t index);
function_signature(float, mesh3d_normal_get_y, index_t index);
function_signature(float, mesh3d_normal_get_z, index_t index);

function_signature(void, mesh3d_triangle_add, index_t i0, index_t i1, index_t i2);
function_signature(void, mesh3d_triangle_add_vec3, vec3_t(int) triangle);
function_signature(void, mesh3d_triangle_set, index_t index, index_t i0, index_t i1, index_t i2);
function_signature(void, mesh3d_triangle_set_vec3, index_t index, vec3_t(int) triangle);
function_signature(void, mesh3d_triangle_set_0, index_t index, index_t i0);
function_signature(void, mesh3d_triangle_set_1, index_t index, index_t i1);
function_signature(void, mesh3d_triangle_set_2, index_t index, index_t i2);
function_signature(vec3_t(int), mesh3d_triangle_get, index_t index);
function_signature(vec3_t(int)*, mesh3d_triangle_get_ptr, index_t index);
function_signature(int, mesh3d_triangle_get_0, index_t index);
function_signature(int, mesh3d_triangle_get_1, index_t index);
function_signature(int, mesh3d_triangle_get_2, index_t index);

function_signature(void, mesh3d_uv_add, float x, float y);
function_signature(void, mesh3d_uv_add_vec2, vec2_t(float) uv);
function_signature(void, mesh3d_uv_set, index_t index, float x, float y);
function_signature(void, mesh3d_uv_set_vec2, index_t index, vec2_t(float) uv);
function_signature(void, mesh3d_uv_set_x, index_t index, float x);
function_signature(void, mesh3d_uv_set_y, index_t index, float y);
function_signature(vec2_t(float), mesh3d_uv_get, index_t index);
function_signature(vec2_t(float)*, mesh3d_uv_get_ptr, index_t index);
function_signature(float, mesh3d_uv_get_y, index_t index);
function_signature(float, mesh3d_uv_get_x, index_t index);


#define mesh3d_new(...) define_alias_function_void_macro(mesh3d_new, __VA_ARGS__)
#define mesh3d_get_binded(...) define_alias_function_void_macro(mesh3d_get_binded, __VA_ARGS__)
#define mesh3d_bind(...) define_alias_function_macro(mesh3d_bind, __VA_ARGS__)
#define mesh3d_unbind(...) define_alias_function_void_macro(mesh3d_unbind)
#define mesh3d_push_binded(...) define_alias_function_void_macro(mesh3d_push_binded)
#define mesh3d_pop_binded(...) define_alias_function_void_macro(mesh3d_pop_binded)
#define mesh3d_destroy(...) define_alias_function_void_macro(mesh3d_destroy)

#define mesh3d_positions_new(...) define_alias_function_macro(mesh3d_positions_new, __VA_ARGS__)
#define mesh3d_normals_new(...) define_alias_function_macro(mesh3d_normals_new, __VA_ARGS__)
#define mesh3d_triangles_new(...) define_alias_function_macro(mesh3d_triangles_new, __VA_ARGS__)
#define mesh3d_uvs_new(...) define_alias_function_macro(mesh3d_uvs_new, __VA_ARGS__)
#define mesh3d_positions_bind() define_alias_function_void_macro(mesh3d_positions_bind)
#define mesh3d_normals_bind() define_alias_function_void_macro(mesh3d_normals_bind)
#define mesh3d_triangles_bind() define_alias_function_void_macro(mesh3d_triangles_bind)
#define mesh3d_uvs_bind() define_alias_function_void_macro(mesh3d_uvs_bind)

#define mesh3d_has_uvs() define_alias_function_void_macro(mesh3d_has_uvs) 
#define mesh3d_has_positions() define_alias_function_void_macro(mesh3d_has_positions) 
#define mesh3d_has_normals() define_alias_function_void_macro(mesh3d_has_normals) 
#define mesh3d_has_triangles() define_alias_function_void_macro(mesh3d_has_triangles)

#define mesh3d_calculate_normals() define_alias_function_void_macro(mesh3d_calculate_normals)
#define mesh3d_optimize_buffer() define_alias_function_void_macro(mesh3d_optimize_buffer)

#define mesh3d_position_add(...) define_alias_function_macro(mesh3d_position_add, __VA_ARGS__)
#define mesh3d_position_add_vec3(...) define_alias_function_macro(mesh3d_position_add_vec3, __VA_ARGS__)
#define mesh3d_position_set(...) define_alias_function_macro(mesh3d_position_set, __VA_ARGS__)
#define mesh3d_position_set_vec3(...) define_alias_function_macro(mesh3d_position_set_vec3, __VA_ARGS__)
#define mesh3d_position_set_x(...) define_alias_function_macro(mesh3d_position_set_x, __VA_ARGS__)
#define mesh3d_position_set_y(...) define_alias_function_macro(mesh3d_position_set_y, __VA_ARGS__)
#define mesh3d_position_set_z(...) define_alias_function_macro(mesh3d_position_set_z, __VA_ARGS__)
#define mesh3d_position_get(...) define_alias_function_macro(mesh3d_position_get, __VA_ARGS__)
#define mesh3d_position_get_ptr(...) define_alias_function_macro(mesh3d_position_get_ptr, __VA_ARGS__)
#define mesh3d_position_get_x(...) define_alias_function_macro(mesh3d_position_get_x, __VA_ARGS__)
#define mesh3d_position_get_y(...) define_alias_function_macro(mesh3d_position_get_y, __VA_ARGS__)
#define mesh3d_position_get_z(...) define_alias_function_macro(mesh3d_position_get_z, __VA_ARGS__)

#define mesh3d_normal_add(...) define_alias_function_macro(mesh3d_normal_add, __VA_ARGS__)
#define mesh3d_normal_add_vec3(...) define_alias_function_macro(mesh3d_normal_add_vec3, __VA_ARGS__)
#define mesh3d_normal_set(...) define_alias_function_macro(mesh3d_normal_set, __VA_ARGS__)
#define mesh3d_normal_set_vec3(...) define_alias_function_macro(mesh3d_normal_set_vec3, __VA_ARGS__)
#define mesh3d_normal_set_x(...) define_alias_function_macro(mesh3d_normal_set_x, __VA_ARGS__)
#define mesh3d_normal_set_y(...) define_alias_function_macro(mesh3d_normal_set_y, __VA_ARGS__)
#define mesh3d_normal_set_z(...) define_alias_function_macro(mesh3d_normal_set_z, __VA_ARGS__)
#define mesh3d_normal_get(...) define_alias_function_macro(mesh3d_normal_get, __VA_ARGS__)
#define mesh3d_normal_get_ptr(...) define_alias_function_macro(mesh3d_normal_get_ptr, __VA_ARGS__)
#define mesh3d_normal_get_x(...) define_alias_function_macro(mesh3d_normal_get_x, __VA_ARGS__)
#define mesh3d_normal_get_y(...) define_alias_function_macro(mesh3d_normal_get_y, __VA_ARGS__)
#define mesh3d_normal_get_z(...) define_alias_function_macro(mesh3d_normal_get_z, __VA_ARGS__)

#define mesh3d_triangle_add(...) define_alias_function_macro(mesh3d_triangle_add, __VA_ARGS__)
#define mesh3d_triangle_add_vec3(...) define_alias_function_macro(mesh3d_triangle_add_vec3, __VA_ARGS__)
#define mesh3d_triangle_set(...) define_alias_function_macro(mesh3d_triangle_set, __VA_ARGS__)
#define mesh3d_triangle_set_vec3(...) define_alias_function_macro(mesh3d_triangle_set_vec3, __VA_ARGS__)
#define mesh3d_triangle_set_0(...) define_alias_function_macro(mesh3d_triangle_set_0, __VA_ARGS__)
#define mesh3d_triangle_set_1(...) define_alias_function_macro(mesh3d_triangle_set_1, __VA_ARGS__)
#define mesh3d_triangle_set_2(...) define_alias_function_macro(mesh3d_triangle_set_2, __VA_ARGS__)
#define mesh3d_triangle_get(...) define_alias_function_macro(mesh3d_triangle_get, __VA_ARGS__)
#define mesh3d_triangle_get_ptr(...) define_alias_function_macro(mesh3d_triangle_get_ptr, __VA_ARGS__)
#define mesh3d_triangle_get_0(...) define_alias_function_macro(mesh3d_triangle_get_0, __VA_ARGS__)
#define mesh3d_triangle_get_1(...) define_alias_function_macro(mesh3d_triangle_get_1, __VA_ARGS__)
#define mesh3d_triangle_get_2(...) define_alias_function_macro(mesh3d_triangle_get_2, __VA_ARGS__)

#define mesh3d_uv_add(...) define_alias_function_macro(mesh3d_uv_add, __VA_ARGS__)
#define mesh3d_uv_add_vec2(...) define_alias_function_macro(mesh3d_uv_add_vec2, __VA_ARGS__)
#define mesh3d_uv_set(...) define_alias_function_macro(mesh3d_uv_set, __VA_ARGS__)
#define mesh3d_uv_set_vec2(...) define_alias_function_macro(mesh3d_uv_set_vec2, __VA_ARGS__)
#define mesh3d_uv_set_x(...) define_alias_function_macro(mesh3d_uv_set_x, __VA_ARGS__)
#define mesh3d_uv_set_y(...) define_alias_function_macro(mesh3d_uv_set_y, __VA_ARGS__)
#define mesh3d_uv_get(...) define_alias_function_macro(mesh3d_uv_get, __VA_ARGS__)
#define mesh3d_uv_get_ptr(...) define_alias_function_macro(mesh3d_uv_get_ptr, __VA_ARGS__)
#define mesh3d_uv_get_y(...) define_alias_function_macro(mesh3d_uv_get_y, __VA_ARGS__)
#define mesh3d_uv_get_x(...) define_alias_function_macro(mesh3d_uv_get_x, __VA_ARGS__)

