#pragma once

#ifdef GLOBAL_DEBUG
#	define MESH3D_DEBUG
#elif defined(GLOBAL_RELEASE)
#	define MESH3D_RELEASE
#endif

#if !defined(MESH3D_RELEASE) && !defined(MESH3D_DEBUG)
#	warning "None of MESH3D_RELEASE or MESH3D_DEBUG is defined, using MESH3D_DEBUG"
#	define MESH3D_DEBUG
#elif defined(MESH3D_DEBUG) && defined(MESH3D_RELEASE)
#	warning "Both of MESH3D_RELEASE and MESH3D_DEBUG are defined, using MESH3D_DEBUG"
#	undef MESH3D_RELEASE
#endif

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

typedef struct mesh3d_t
{
	BUFFER* positions; 
	BUFFER* normals;
	BUFFER* uvs;
	BUFFER* colors;
	BUFFER* triangles;
} mesh3d_t;

function_signature_void(mesh3d_t*, mesh3d_new);
function_signature(void, mesh3d_destroy, mesh3d_t* mesh);

function_signature(void, mesh3d_positions_new, mesh3d_t* mesh, u32 count);
function_signature(void, mesh3d_normals_new, mesh3d_t* mesh, u32 count);
function_signature(void, mesh3d_triangles_new, mesh3d_t* mesh, u32 count);
function_signature(void, mesh3d_uvs_new, mesh3d_t* mesh, u32 count);
function_signature(void, mesh3d_colors_new, mesh3d_t* mesh, u32 count);

function_signature(bool, mesh3d_has_uvs, mesh3d_t* mesh); 
function_signature(bool, mesh3d_has_positions, mesh3d_t* mesh); 
function_signature(bool, mesh3d_has_normals, mesh3d_t* mesh); 
function_signature(bool, mesh3d_has_triangles, mesh3d_t* mesh);
function_signature(bool, mesh3d_has_colors, mesh3d_t* mesh);
function_signature(u32, mesh3d_positions_count, mesh3d_t* mesh);
function_signature(u32, mesh3d_normals_count, mesh3d_t* mesh);
function_signature(u32, mesh3d_uvs_count, mesh3d_t* mesh);
function_signature(u32, mesh3d_colors_count, mesh3d_t* mesh);
function_signature(u32, mesh3d_triangles_count, mesh3d_t* mesh);

function_signature(void, mesh3d_calculate_normals, mesh3d_t* mesh);
function_signature(void, mesh3d_optimize_buffer, mesh3d_t* mesh);

function_signature(void, mesh3d_position_add, mesh3d_t* mesh, float x, float y, float z);
function_signature(void, mesh3d_position_add_vec3, mesh3d_t* mesh, vec3_t(float) position);
function_signature(void, mesh3d_position_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
function_signature(void, mesh3d_position_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float) position);
function_signature(void, mesh3d_position_set_x, mesh3d_t* mesh, index_t index, float x);
function_signature(void, mesh3d_position_set_y, mesh3d_t* mesh, index_t index, float y);
function_signature(void, mesh3d_position_set_z, mesh3d_t* mesh, index_t index, float z);
function_signature(vec3_t(float), mesh3d_position_get, mesh3d_t* mesh, index_t index);
function_signature(vec3_t(float)*, mesh3d_position_get_ptr, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_position_get_x, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_position_get_y, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_position_get_z, mesh3d_t* mesh, index_t index);

function_signature(void, mesh3d_normal_add, mesh3d_t* mesh, float x, float y, float z);
function_signature(void, mesh3d_normal_add_vec3, mesh3d_t* mesh, vec3_t(float) normal);
function_signature(void, mesh3d_normal_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
function_signature(void, mesh3d_normal_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float)  normal);
function_signature(void, mesh3d_normal_set_x, mesh3d_t* mesh, index_t index, float x);
function_signature(void, mesh3d_normal_set_y, mesh3d_t* mesh, index_t index, float y); 
function_signature(void, mesh3d_normal_set_z, mesh3d_t* mesh, index_t index, float z);
function_signature(vec3_t(float), mesh3d_normal_get, mesh3d_t* mesh, index_t index);
function_signature(vec3_t(float)*, mesh3d_normal_get_ptr, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_normal_get_x, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_normal_get_y, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_normal_get_z, mesh3d_t* mesh, index_t index);

function_signature(void, mesh3d_triangle_add, mesh3d_t* mesh, index_t i0, index_t i1, index_t i2);
function_signature(void, mesh3d_triangle_add_vec3, mesh3d_t* mesh, vec3_t(int) triangle);
function_signature(void, mesh3d_triangle_set, mesh3d_t* mesh, index_t index, index_t i0, index_t i1, index_t i2);
function_signature(void, mesh3d_triangle_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(int) triangle);
function_signature(void, mesh3d_triangle_set_0, mesh3d_t* mesh, index_t index, index_t i0);
function_signature(void, mesh3d_triangle_set_1, mesh3d_t* mesh, index_t index, index_t i1);
function_signature(void, mesh3d_triangle_set_2, mesh3d_t* mesh, index_t index, index_t i2);
function_signature(vec3_t(int), mesh3d_triangle_get, mesh3d_t* mesh, index_t index);
function_signature(vec3_t(int)*, mesh3d_triangle_get_ptr, mesh3d_t* mesh, index_t index);
function_signature(int, mesh3d_triangle_get_0, mesh3d_t* mesh, index_t index);
function_signature(int, mesh3d_triangle_get_1, mesh3d_t* mesh, index_t index);
function_signature(int, mesh3d_triangle_get_2, mesh3d_t* mesh, index_t index);

function_signature(void, mesh3d_uv_add, mesh3d_t* mesh, float x, float y);
function_signature(void, mesh3d_uv_add_vec2, mesh3d_t* mesh, vec2_t(float) uv);
function_signature(void, mesh3d_uv_set, mesh3d_t* mesh, index_t index, float x, float y);
function_signature(void, mesh3d_uv_set_vec2, mesh3d_t* mesh, index_t index, vec2_t(float) uv);
function_signature(void, mesh3d_uv_set_x, mesh3d_t* mesh, index_t index, float x);
function_signature(void, mesh3d_uv_set_y, mesh3d_t* mesh, index_t index, float y);
function_signature(vec2_t(float), mesh3d_uv_get, mesh3d_t* mesh, index_t index);
function_signature(vec2_t(float)*, mesh3d_uv_get_ptr, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_uv_get_y, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_uv_get_x, mesh3d_t* mesh, index_t index);

function_signature(void, mesh3d_color_add, mesh3d_t* mesh, float x, float y, float z);
function_signature(void, mesh3d_color_add_vec3, mesh3d_t* mesh, vec3_t(float) color);
function_signature(void, mesh3d_color_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
function_signature(void, mesh3d_color_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float)  color);
function_signature(void, mesh3d_color_set_x, mesh3d_t* mesh, index_t index, float x);
function_signature(void, mesh3d_color_set_y, mesh3d_t* mesh, index_t index, float y); 
function_signature(void, mesh3d_color_set_z, mesh3d_t* mesh, index_t index, float z);
function_signature(vec3_t(float), mesh3d_color_get, mesh3d_t* mesh, index_t index);
function_signature(vec3_t(float)*, mesh3d_color_get_ptr, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_color_get_x, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_color_get_y, mesh3d_t* mesh, index_t index);
function_signature(float, mesh3d_color_get_z, mesh3d_t* mesh, index_t index);

#define mesh3d_new(...) define_alias_function_void_macro(mesh3d_new)
#define mesh3d_destroy(...) define_alias_function_macro(mesh3d_destroy, __VA_ARGS__)

#define mesh3d_positions_new(...) define_alias_function_macro(mesh3d_positions_new, __VA_ARGS__)
#define mesh3d_normals_new(...) define_alias_function_macro(mesh3d_normals_new, __VA_ARGS__)
#define mesh3d_triangles_new(...) define_alias_function_macro(mesh3d_triangles_new, __VA_ARGS__)
#define mesh3d_uvs_new(...) define_alias_function_macro(mesh3d_uvs_new, __VA_ARGS__)
#define mesh3d_colors_new(...) define_alias_function_macro(mesh3d_colors_new, __VA_ARGS__)

#define mesh3d_has_uvs(...) define_alias_function_macro(mesh3d_has_uvs, __VA_ARGS__) 
#define mesh3d_has_positions(...) define_alias_function_macro(mesh3d_has_positions, __VA_ARGS__) 
#define mesh3d_has_normals(...) define_alias_function_macro(mesh3d_has_normals, __VA_ARGS__) 
#define mesh3d_has_triangles(...) define_alias_function_macro(mesh3d_has_triangles, __VA_ARGS__)
#define mesh3d_has_colors(...) define_alias_function_macro(mesh3d_has_colors, __VA_ARGS__)
#define mesh3d_positions_count(...) define_alias_function_macro(mesh3d_positions_count, __VA_ARGS__)
#define mesh3d_normals_count(...) define_alias_function_macro(mesh3d_normals_count, __VA_ARGS__)
#define mesh3d_uvs_count(...) define_alias_function_macro(mesh3d_uvs_count, __VA_ARGS__)
#define mesh3d_colors_count(...) define_alias_function_macro(mesh3d_colors_count, __VA_ARGS__)
#define mesh3d_triangles_count(...) define_alias_function_macro(mesh3d_triangles_count, __VA_ARGS__)

#define mesh3d_calculate_normals(...) define_alias_function_macro(mesh3d_calculate_normals, __VA_ARGS__)
#define mesh3d_optimize_buffer(...) define_alias_function_macro(mesh3d_optimize_buffer, __VA_ARGS__)

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

#define mesh3d_color_add(...) define_alias_function_macro(mesh3d_color_add, __VA_ARGS__)
#define mesh3d_color_add_vec3(...) define_alias_function_macro(mesh3d_color_add_vec3, __VA_ARGS__)
#define mesh3d_color_set(...) define_alias_function_macro(mesh3d_color_set, __VA_ARGS__)
#define mesh3d_color_set_vec3(...) define_alias_function_macro(mesh3d_color_set_vec3, __VA_ARGS__)
#define mesh3d_color_set_x(...) define_alias_function_macro(mesh3d_color_set_x, __VA_ARGS__)
#define mesh3d_color_set_y(...) define_alias_function_macro(mesh3d_color_set_y, __VA_ARGS__)
#define mesh3d_color_set_z(...) define_alias_function_macro(mesh3d_color_set_z, __VA_ARGS__)
#define mesh3d_color_get(...) define_alias_function_macro(mesh3d_color_get, __VA_ARGS__)
#define mesh3d_color_get_ptr(...) define_alias_function_macro(mesh3d_color_get_ptr, __VA_ARGS__)
#define mesh3d_color_get_x(...) define_alias_function_macro(mesh3d_color_get_x, __VA_ARGS__)
#define mesh3d_color_get_y(...) define_alias_function_macro(mesh3d_color_get_y, __VA_ARGS__)
#define mesh3d_color_get_z(...) define_alias_function_macro(mesh3d_color_get_z, __VA_ARGS__)


function_signature(mesh3d_t*, mesh3d_cube, float size);
#define mesh3d_cube(...) define_alias_function_macro(mesh3d_cube, __VA_ARGS__)

