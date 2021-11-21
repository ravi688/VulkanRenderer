#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#ifdef MESH3d_REQUIRE_PRINT
#define PRINT_REQUIRE_ARRAY_PRINT
#include "print.h"
#endif

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>

#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <buffer.h>

#define MESH3d_DEBUG


#define ensure_mesh3d_is_binded(return_value)\
if(binded_mesh3d == NULL)\
{\
	log_err("There is no binded , make sure to bind a mesh3d_t first by calling mesh3d_bind()");\
	return return_value;\
}

typedef unsigned short index_t;

typedef struct
{
	LIST(vec3_t(float)) positions; 
	LIST(vec2_t(float)) uvs;
	LIST(vec3_t(float)) normals;
	LIST(index_t) triangles;
} mesh3d_t;

mesh3d_t* mesh3d_new(void);
void mesh3d_destroy();
bool mesh3d_has_uvs(); 
bool mesh3d_has_positions(); 
bool mesh3d_has_normals(); 
bool mesh3d_has_triangles();
void mesh3d_bind(mesh3d_t*); 
void mesh3d_push_binded();
void mesh3d_pop_binded();
void mesh3d_unbind();
mesh3d_t* mesh3d_get_binded();
void mesh3d_calculate_normals(void);
void mesh3d_optimize_buffer();

void 		mesh3d_positions_bind();
void 		mesh3d_positions_new(uint32_t count);
void 		mesh3d_normals_bind();
void 		mesh3d_normals_new(uint32_t count);
void 		mesh3d_uvs_new(uint32_t count);
void 		mesh3d_uvs_bind();
void 		mesh3d_triangles_bind();
void 		mesh3d_triangles_new(uint32_t count);


void 		mesh3d_positions_add(float x, float y, float z);
void 		mesh3d_positions_add_vec3(vec3_t(float) position);


void 		mesh3d_positions_set_at(index_t index, float x, float y, float z);
void 		mesh3d_positions_set_at_vec3(index_t index, vec3_t(float) position);
void 		mesh3d_positions_set_at_x(index_t index, float x);
void 		mesh3d_positions_set_at_y(index_t index, float y);
void 		mesh3d_positions_set_at_z(index_t index, float z);


vec3_t(float)	mesh3d_positions_get_at(index_t index);
vec3_t(float)*mesh3d_positions_get_at_ptr(index_t index);
float 		mesh3d_positions_get_at_x(index_t index);
float 		mesh3d_positions_get_at_y(index_t index);
float 		mesh3d_positions_get_at_z(index_t index);



void 		mesh3d_normals_add(float x, float y, float z);
void 		mesh3d_normals_add_vec3(vec3_t(float) normal);


void 		mesh3d_normals_set_at(index_t index, float x, float y, float z);
void 		mesh3d_normals_set_at_vec3(index_t index, vec3_t(float)  normal);
void 		mesh3d_normals_set_at_x(index_t index, float x);
void 		mesh3d_normals_set_at_y(index_t index, float y); 
void 		mesh3d_normals_set_at_z(index_t index, float z);


vec3_t(float) mesh3d_normals_get_at(index_t index);
vec3_t(float)*mesh3d_normals_get_at_ptr(index_t index);
float 		mesh3d_normals_get_at_x(index_t index);
float 		mesh3d_normals_get_at_y(index_t index);
float 		mesh3d_normals_get_at_z(index_t index);



void 		mesh3d_triangles_add(index_t i0, index_t i1, index_t i2);
void 		mesh3d_triangles_add_vec3(vec3_t(int) triangle);

void		mesh3d_triangle_set_at(index_t index, index_t i0, index_t i1, index_t i2);
void		mesh3d_triangle_set_at_vec3(index_t index, vec3_t(int) triangle);
void 		mesh3d_triangle_set_at_0(index_t index, index_t i0);
void 		mesh3d_triangle_set_at_1(index_t index, index_t i1);
void 		mesh3d_triangle_set_at_2(index_t index, index_t i2);

vec3_t(int) 	mesh3d_triangle_get_at(index_t index);
vec3_t(int)* 	mesh3d_triangle_get_at_ptr(index_t index);
int     	mesh3d_triangle_get_at_0(index_t index);
int     	mesh3d_triangle_get_at_1(index_t index);
int     	mesh3d_triangle_get_at_2(index_t index);



void 		mesh3d_uvs_add(float x, float y);
void 		mesh3d_uvs_add_vec2(vec2_t(float) uv);

void 		mesh3d_uvs_set_at(index_t index, float x, float y);
void 		mesh3d_uvs_set_at_vec2(index_t index, vec2_t(float) uv);
void 		mesh3d_uvs_set_at_x(index_t index, float x);
void 		mesh3d_uvs_set_at_y(index_t index, float y);

vec2_t(float) mesh3d_uvs_get_at(index_t index);
vec2_t(float)*mesh3d_uvs_get_at_ptr(index_t index);
float 		mesh3d_uvs_get_at_y(index_t index);
float 		mesh3d_uvs_get_at_x(index_t index);

