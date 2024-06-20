/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: mesh3d.h is a part of VulkanRenderer

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

#include <hpml/vec3.h>
#include <hpml/vec2.h>

//For mesh3d_set_transform(), mesh3d_get_transform()
#include <hpml/mat4.h>

#include <bufferlib/buffer.h>
#include <renderer/defines.h>
#include <renderer/debug.h>

#define MESH3D_MAX_ATTRIBUTE_COUNT 5
#define MESH3D_INDEX_SIZE 4 //sizeof(u32)
#define MESH3D_VERTEX_ATTRIB_POSITION_SIZE sizeof(vec4_t)
#define MESH3D_VERTEX_ATTRIB_NORMAL_SIZE sizeof(vec4_t)
#define MESH3D_VERTEX_ATTRIB_COLOR_SIZE sizeof(vec4_t)
#define MESH3D_VERTEX_ATTRIB_UV_SIZE sizeof(vec2_t)
#define MESH3D_VERTEX_ATTRIB_TANGENT_SIZE sizeof(vec4_t)
#define MESH3D_VERTEX_ATTRIB_TRIANGLE_SIZE sizeof(vec3uint_t)

typedef u32 index_t;

typedef struct vec3uint_t
{
	u32 x;
	u32 y;
	u32 z;
} vec3uint_t;

typedef struct mesh3d_t
{
	//Ordering is important here, see: mesh_create
	BUFFER* positions; 	// location = 0
	BUFFER* normals; 	// location = 1
	BUFFER* colors;		// location = 2
	BUFFER* uvs;		// location = 3
	BUFFER* tangents; 	// location = 4
	BUFFER* triangles;
	mat4_t transform;
	memory_allocator_t* allocator;
} mesh3d_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API function_signature(mesh3d_t*, mesh3d_new, memory_allocator_t* allocator);
RENDERER_API function_signature(void, mesh3d_destroy, mesh3d_t* mesh);

RENDERER_API function_signature(void, mesh3d_positions_new, mesh3d_t* mesh, index_t count);
RENDERER_API function_signature(void, mesh3d_normals_new, mesh3d_t* mesh, index_t count);
RENDERER_API function_signature(void, mesh3d_tangents_new, mesh3d_t* mesh, index_t count);
RENDERER_API function_signature(void, mesh3d_triangles_new, mesh3d_t* mesh, index_t count);
RENDERER_API function_signature(void, mesh3d_uvs_new, mesh3d_t* mesh, index_t count);
RENDERER_API function_signature(void, mesh3d_colors_new, mesh3d_t* mesh, index_t count);

RENDERER_API function_signature(void, mesh3d_positions_free, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_normals_free, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_tangents_free, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_colors_free, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_triangles_free, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_uvs_free, mesh3d_t* mesh);

RENDERER_API function_signature(bool, mesh3d_has_uvs, mesh3d_t* mesh); 
RENDERER_API function_signature(bool, mesh3d_has_positions, mesh3d_t* mesh); 
RENDERER_API function_signature(bool, mesh3d_has_normals, mesh3d_t* mesh); 
RENDERER_API function_signature(bool, mesh3d_has_tangents, mesh3d_t* mesh); 
RENDERER_API function_signature(bool, mesh3d_has_triangles, mesh3d_t* mesh);
RENDERER_API function_signature(bool, mesh3d_has_colors, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_positions_count, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_normals_count, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_tangents_count, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_uvs_count, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_colors_count, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_triangles_count, mesh3d_t* mesh);

RENDERER_API function_signature(void, mesh3d_flip_triangles, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_calculate_normals, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_calculate_tangents, mesh3d_t* mesh);
RENDERER_API function_signature(void, mesh3d_optimize_buffer, mesh3d_t* mesh);

RENDERER_API function_signature(void, mesh3d_position_add, mesh3d_t* mesh, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_position_add_vec3, mesh3d_t* mesh, vec3_t position);
RENDERER_API function_signature(void, mesh3d_position_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_position_set_vec3, mesh3d_t* mesh, index_t index, vec3_t position);
RENDERER_API function_signature(void, mesh3d_position_set_x, mesh3d_t* mesh, index_t index, float x);
RENDERER_API function_signature(void, mesh3d_position_set_y, mesh3d_t* mesh, index_t index, float y);
RENDERER_API function_signature(void, mesh3d_position_set_z, mesh3d_t* mesh, index_t index, float z);
RENDERER_API function_signature(vec3_t, mesh3d_position_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec3_t*, mesh3d_position_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_position_get_x, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_position_get_y, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_position_get_z, mesh3d_t* mesh, index_t index);

RENDERER_API function_signature(void, mesh3d_normal_add, mesh3d_t* mesh, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_normal_add_vec3, mesh3d_t* mesh, vec3_t normal);
RENDERER_API function_signature(void, mesh3d_normal_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_normal_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  normal);
RENDERER_API function_signature(void, mesh3d_normal_set_x, mesh3d_t* mesh, index_t index, float x);
RENDERER_API function_signature(void, mesh3d_normal_set_y, mesh3d_t* mesh, index_t index, float y); 
RENDERER_API function_signature(void, mesh3d_normal_set_z, mesh3d_t* mesh, index_t index, float z);
RENDERER_API function_signature(vec3_t, mesh3d_normal_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec3_t*, mesh3d_normal_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_normal_get_x, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_normal_get_y, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_normal_get_z, mesh3d_t* mesh, index_t index);

RENDERER_API function_signature(void, mesh3d_tangent_add, mesh3d_t* mesh, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_tangent_add_vec3, mesh3d_t* mesh, vec3_t tangent);
RENDERER_API function_signature(void, mesh3d_tangent_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_tangent_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  tangent);
RENDERER_API function_signature(void, mesh3d_tangent_set_x, mesh3d_t* mesh, index_t index, float x);
RENDERER_API function_signature(void, mesh3d_tangent_set_y, mesh3d_t* mesh, index_t index, float y); 
RENDERER_API function_signature(void, mesh3d_tangent_set_z, mesh3d_t* mesh, index_t index, float z);
RENDERER_API function_signature(vec3_t, mesh3d_tangent_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec3_t*, mesh3d_tangent_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_tangent_get_x, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_tangent_get_y, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_tangent_get_z, mesh3d_t* mesh, index_t index);

RENDERER_API function_signature(void, mesh3d_triangle_add, mesh3d_t* mesh, index_t i0, index_t i1, index_t i2);
RENDERER_API function_signature(void, mesh3d_triangle_add_vec3, mesh3d_t* mesh, vec3uint_t triangle);
RENDERER_API function_signature(void, mesh3d_triangle_set, mesh3d_t* mesh, index_t index, index_t i0, index_t i1, index_t i2);
RENDERER_API function_signature(void, mesh3d_triangle_set_vec3, mesh3d_t* mesh, index_t index, vec3uint_t triangle);
RENDERER_API function_signature(void, mesh3d_triangle_set_0, mesh3d_t* mesh, index_t index, index_t i0);
RENDERER_API function_signature(void, mesh3d_triangle_set_1, mesh3d_t* mesh, index_t index, index_t i1);
RENDERER_API function_signature(void, mesh3d_triangle_set_2, mesh3d_t* mesh, index_t index, index_t i2);
RENDERER_API function_signature(vec3uint_t, mesh3d_triangle_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec3uint_t*, mesh3d_triangle_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(int, mesh3d_triangle_get_0, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(int, mesh3d_triangle_get_1, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(int, mesh3d_triangle_get_2, mesh3d_t* mesh, index_t index);

RENDERER_API function_signature(void, mesh3d_uv_add, mesh3d_t* mesh, float x, float y);
RENDERER_API function_signature(void, mesh3d_uv_add_vec2, mesh3d_t* mesh, vec2_t uv);
RENDERER_API function_signature(void, mesh3d_uv_set, mesh3d_t* mesh, index_t index, float x, float y);
RENDERER_API function_signature(void, mesh3d_uv_set_vec2, mesh3d_t* mesh, index_t index, vec2_t uv);
RENDERER_API function_signature(void, mesh3d_uv_set_x, mesh3d_t* mesh, index_t index, float x);
RENDERER_API function_signature(void, mesh3d_uv_set_y, mesh3d_t* mesh, index_t index, float y);
RENDERER_API function_signature(vec2_t, mesh3d_uv_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec2_t*, mesh3d_uv_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_uv_get_y, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_uv_get_x, mesh3d_t* mesh, index_t index);

RENDERER_API function_signature(void, mesh3d_color_add, mesh3d_t* mesh, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_color_add_vec3, mesh3d_t* mesh, vec3_t color);
RENDERER_API function_signature(void, mesh3d_color_set, mesh3d_t* mesh, index_t index, float x, float y, float z);
RENDERER_API function_signature(void, mesh3d_color_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  color);
RENDERER_API function_signature(void, mesh3d_color_set_x, mesh3d_t* mesh, index_t index, float x);
RENDERER_API function_signature(void, mesh3d_color_set_y, mesh3d_t* mesh, index_t index, float y); 
RENDERER_API function_signature(void, mesh3d_color_set_z, mesh3d_t* mesh, index_t index, float z);
RENDERER_API function_signature(vec3_t, mesh3d_color_get, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(vec3_t*, mesh3d_color_get_ptr, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_color_get_x, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_color_get_y, mesh3d_t* mesh, index_t index);
RENDERER_API function_signature(float, mesh3d_color_get_z, mesh3d_t* mesh, index_t index);

#define mesh3d_new(...) define_alias_function_macro(mesh3d_new, __VA_ARGS__)
#define mesh3d_destroy(...) define_alias_function_macro(mesh3d_destroy, __VA_ARGS__)

#define mesh3d_positions_new(...) define_alias_function_macro(mesh3d_positions_new, __VA_ARGS__)
#define mesh3d_normals_new(...) define_alias_function_macro(mesh3d_normals_new, __VA_ARGS__)
#define mesh3d_tangents_new(...) define_alias_function_macro(mesh3d_tangents_new, __VA_ARGS__)
#define mesh3d_triangles_new(...) define_alias_function_macro(mesh3d_triangles_new, __VA_ARGS__)
#define mesh3d_uvs_new(...) define_alias_function_macro(mesh3d_uvs_new, __VA_ARGS__)
#define mesh3d_colors_new(...) define_alias_function_macro(mesh3d_colors_new, __VA_ARGS__)

#define mesh3d_positions_free(...) define_alias_function_macro(mesh3d_positions_free, __VA_ARGS__)
#define mesh3d_normals_free(...) define_alias_function_macro(mesh3d_normals_free, __VA_ARGS__)
#define mesh3d_tangents_free(...) define_alias_function_macro(mesh3d_tangents_free, __VA_ARGS__)
#define mesh3d_triangles_free(...) define_alias_function_macro(mesh3d_triangles_free, __VA_ARGS__)
#define mesh3d_uvs_free(...) define_alias_function_macro(mesh3d_uvs_free, __VA_ARGS__)
#define mesh3d_colors_free(...) define_alias_function_macro(mesh3d_colors_free, __VA_ARGS__)

#define mesh3d_has_uvs(...) define_alias_function_macro(mesh3d_has_uvs, __VA_ARGS__) 
#define mesh3d_has_positions(...) define_alias_function_macro(mesh3d_has_positions, __VA_ARGS__) 
#define mesh3d_has_normals(...) define_alias_function_macro(mesh3d_has_normals, __VA_ARGS__) 
#define mesh3d_has_tangents(...) define_alias_function_macro(mesh3d_has_tangents, __VA_ARGS__) 
#define mesh3d_has_triangles(...) define_alias_function_macro(mesh3d_has_triangles, __VA_ARGS__)
#define mesh3d_has_colors(...) define_alias_function_macro(mesh3d_has_colors, __VA_ARGS__)
#define mesh3d_positions_count(...) define_alias_function_macro(mesh3d_positions_count, __VA_ARGS__)
#define mesh3d_normals_count(...) define_alias_function_macro(mesh3d_normals_count, __VA_ARGS__)
#define mesh3d_tangents_count(...) define_alias_function_macro(mesh3d_tangents_count, __VA_ARGS__)
#define mesh3d_uvs_count(...) define_alias_function_macro(mesh3d_uvs_count, __VA_ARGS__)
#define mesh3d_colors_count(...) define_alias_function_macro(mesh3d_colors_count, __VA_ARGS__)
#define mesh3d_triangles_count(...) define_alias_function_macro(mesh3d_triangles_count, __VA_ARGS__)

#define mesh3d_flip_triangles(...) define_alias_function_macro(mesh3d_flip_triangles, __VA_ARGS__)
#define mesh3d_calculate_normals(...) define_alias_function_macro(mesh3d_calculate_normals, __VA_ARGS__)
#define mesh3d_calculate_tangents(...) define_alias_function_macro(mesh3d_calculate_tangents, __VA_ARGS__)
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

#define mesh3d_tangent_add(...) define_alias_function_macro(mesh3d_tangent_add, __VA_ARGS__)
#define mesh3d_tangent_add_vec3(...) define_alias_function_macro(mesh3d_tangent_add_vec3, __VA_ARGS__)
#define mesh3d_tangent_set(...) define_alias_function_macro(mesh3d_tangent_set, __VA_ARGS__)
#define mesh3d_tangent_set_vec3(...) define_alias_function_macro(mesh3d_tangent_set_vec3, __VA_ARGS__)
#define mesh3d_tangent_set_x(...) define_alias_function_macro(mesh3d_tangent_set_x, __VA_ARGS__)
#define mesh3d_tangent_set_y(...) define_alias_function_macro(mesh3d_tangent_set_y, __VA_ARGS__)
#define mesh3d_tangent_set_z(...) define_alias_function_macro(mesh3d_tangent_set_z, __VA_ARGS__)
#define mesh3d_tangent_get(...) define_alias_function_macro(mesh3d_tangent_get, __VA_ARGS__)
#define mesh3d_tangent_get_ptr(...) define_alias_function_macro(mesh3d_tangent_get_ptr, __VA_ARGS__)
#define mesh3d_tangent_get_x(...) define_alias_function_macro(mesh3d_tangent_get_x, __VA_ARGS__)
#define mesh3d_tangent_get_y(...) define_alias_function_macro(mesh3d_tangent_get_y, __VA_ARGS__)
#define mesh3d_tangent_get_z(...) define_alias_function_macro(mesh3d_tangent_get_z, __VA_ARGS__)

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


RENDERER_API function_signature(mesh3d_t*, mesh3d_cube, memory_allocator_t* allocator, float size);
#define mesh3d_cube(...) define_alias_function_macro(mesh3d_cube, __VA_ARGS__)
RENDERER_API function_signature(mesh3d_t*, mesh3d_plane, memory_allocator_t* allocator, float size);
#define mesh3d_plane(...) define_alias_function_macro(mesh3d_plane, __VA_ARGS__)

RENDERER_API function_signature(index_t, mesh3d_sizeof_position, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_sizeof_normal, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_sizeof_tangent, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_sizeof_color, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_sizeof_uv, mesh3d_t* mesh);
RENDERER_API function_signature(index_t, mesh3d_sizeof_index, mesh3d_t* mesh);


#define mesh3d_sizeof_position(...) define_alias_function_macro(mesh3d_sizeof_position, __VA_ARGS__)
#define mesh3d_sizeof_normal(...) define_alias_function_macro(mesh3d_sizeof_normal, __VA_ARGS__)
#define mesh3d_sizeof_tangent(...) define_alias_function_macro(mesh3d_sizeof_tangent, __VA_ARGS__)
#define mesh3d_sizeof_color(...) define_alias_function_macro(mesh3d_sizeof_color, __VA_ARGS__)
#define mesh3d_sizeof_uv(...) define_alias_function_macro(mesh3d_sizeof_uv, __VA_ARGS__)
#define mesh3d_sizeof_index(...) define_alias_function_macro(mesh3d_sizeof_index, __VA_ARGS__)


#define mesh3d_load(...) define_alias_function_macro(mesh3d_load, __VA_ARGS__)
RENDERER_API function_signature(mesh3d_t*, mesh3d_load, memory_allocator_t* allocator, const char* file_path);

#define mesh3d_make_centroid_origin(...) define_alias_function_macro(mesh3d_make_centroid_origin, __VA_ARGS__)
RENDERER_API function_signature(void, mesh3d_make_centroid_origin, mesh3d_t* mesh);

#define mesh3d_transform_set(...) define_alias_function_macro(mesh3d_transform_set, __VA_ARGS__)
RENDERER_API function_signature(void, mesh3d_transform_set, mesh3d_t* mesh, mat4_t transform);

#define mesh3d_transform_get(...) define_alias_function_macro(mesh3d_transform_get, __VA_ARGS__)
RENDERER_API function_signature(mat4_t, mesh3d_transform_get, mesh3d_t* mesh);

#define mesh3d_positions_foreach(...) define_alias_function_macro(mesh3d_positions_foreach, __VA_ARGS__)
#define mesh3d_normals_foreach(...) define_alias_function_macro(mesh3d_normals_foreach, __VA_ARGS__)
#define mesh3d_tangents_foreach(...) define_alias_function_macro(mesh3d_tangents_foreach, __VA_ARGS__)
#define mesh3d_colors_foreach(...) define_alias_function_macro(mesh3d_colors_foreach, __VA_ARGS__)
#define mesh3d_uvs_foreach(...) define_alias_function_macro(mesh3d_uvs_foreach, __VA_ARGS__)
#define mesh3d_triangles_foreach(...) define_alias_function_macro(mesh3d_triangles_foreach, __VA_ARGS__)

RENDERER_API function_signature(void, mesh3d_positions_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* position, void* user_data), void* user_data);
RENDERER_API function_signature(void, mesh3d_normals_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* normal, void* user_data), void* user_data);
RENDERER_API function_signature(void, mesh3d_tangents_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* normal, void* user_data), void* user_data);
RENDERER_API function_signature(void, mesh3d_colors_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* color, void* user_data), void* user_data);
RENDERER_API function_signature(void, mesh3d_uvs_foreach, mesh3d_t* mesh, void (*visitor)(vec2_t* uv, void* user_data), void* user_data);
RENDERER_API function_signature(void, mesh3d_triangles_foreach, mesh3d_t* mesh, void (*visitor)(vec3uint_t* triangle, void* user_data), void* user_data);


END_CPP_COMPATIBLE
