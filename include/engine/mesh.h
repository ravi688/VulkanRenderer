#ifndef __MESH_H__
#define __MESH_H__


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#ifdef MESH_REQUIRE_PRINT
#define PRINT_REQUIRE_ARRAY_PRINT
#include "print.h"
#endif

#include <engine/vec3/header_config.h>
#include <engine/vec3/vec3.h>

#include <engine/vec2/header_config.h>
#include <engine/vec2/vec2.h>
#include <buffer.h>

#define MESH_DEBUG


#define ensure_mesh_is_binded(return_value)\
if(binded_mesh == NULL)\
{\
	log_err("There is no binded , make sure to bind a mesh_t first by calling mesh_bind()");\
	return return_value;\
}

typedef unsigned short index_t;

typedef struct
{
	LIST(vec3_t(float)) positions; 
	LIST(vec2_t(float)) uvs;
	LIST(vec3_t(float)) normals;
	LIST(index_t) triangles;
} mesh_t;

mesh_t* mesh_new(void);
void mesh_destroy();
bool mesh_has_uvs(); 
bool mesh_has_positions(); 
bool mesh_has_normals(); 
bool mesh_has_triangles();
void mesh_bind(mesh_t*); 
void mesh_push_binded();
void mesh_pop_binded();
void mesh_unbind();
mesh_t* mesh_get_binded();
void mesh_calculate_normals(void);
void mesh_optimize_buffer();

void 		mesh_positions_bind();
void 		mesh_positions_new(uint32_t count);
void 		mesh_normals_bind();
void 		mesh_normals_new(uint32_t count);
void 		mesh_uvs_new(uint32_t count);
void 		mesh_uvs_bind();
void 		mesh_triangles_bind();
void 		mesh_triangles_new(uint32_t count);


void 		mesh_positions_add(float x, float y, float z);
void 		mesh_positions_add_vec3(vec3_t(float) position);


void 		mesh_positions_set_at(index_t index, float x, float y, float z);
void 		mesh_positions_set_at_vec3(index_t index, vec3_t(float) position);
void 		mesh_positions_set_at_x(index_t index, float x);
void 		mesh_positions_set_at_y(index_t index, float y);
void 		mesh_positions_set_at_z(index_t index, float z);


vec3_t(float)	mesh_positions_get_at(index_t index);
vec3_t(float)*mesh_positions_get_at_ptr(index_t index);
float 		mesh_positions_get_at_x(index_t index);
float 		mesh_positions_get_at_y(index_t index);
float 		mesh_positions_get_at_z(index_t index);



void 		mesh_normals_add(float x, float y, float z);
void 		mesh_normals_add_vec3(vec3_t(float) normal);


void 		mesh_normals_set_at(index_t index, float x, float y, float z);
void 		mesh_normals_set_at_vec3(index_t index, vec3_t(float)  normal);
void 		mesh_normals_set_at_x(index_t index, float x);
void 		mesh_normals_set_at_y(index_t index, float y); 
void 		mesh_normals_set_at_z(index_t index, float z);


vec3_t(float) mesh_normals_get_at(index_t index);
vec3_t(float)*mesh_normals_get_at_ptr(index_t index);
float 		mesh_normals_get_at_x(index_t index);
float 		mesh_normals_get_at_y(index_t index);
float 		mesh_normals_get_at_z(index_t index);



void 		mesh_triangles_add(index_t i0, index_t i1, index_t i2);
void 		mesh_triangles_add_vec3(vec3_t(int) triangle);

void		mesh_triangle_set_at(index_t index, index_t i0, index_t i1, index_t i2);
void		mesh_triangle_set_at_vec3(index_t index, vec3_t(int) triangle);
void 		mesh_triangle_set_at_0(index_t index, index_t i0);
void 		mesh_triangle_set_at_1(index_t index, index_t i1);
void 		mesh_triangle_set_at_2(index_t index, index_t i2);

vec3_t(int) 	mesh_triangle_get_at(index_t index);
vec3_t(int)* 	mesh_triangle_get_at_ptr(index_t index);
int     	mesh_triangle_get_at_0(index_t index);
int     	mesh_triangle_get_at_1(index_t index);
int     	mesh_triangle_get_at_2(index_t index);



void 		mesh_uvs_add(float x, float y);
void 		mesh_uvs_add_vec2(vec2_t(float) uv);

void 		mesh_uvs_set_at(index_t index, float x, float y);
void 		mesh_uvs_set_at_vec2(index_t index, vec2_t(float) uv);
void 		mesh_uvs_set_at_x(index_t index, float x);
void 		mesh_uvs_set_at_y(index_t index, float y);

vec2_t(float) mesh_uvs_get_at(index_t index);
vec2_t(float)*mesh_uvs_get_at_ptr(index_t index);
float 		mesh_uvs_get_at_y(index_t index);
float 		mesh_uvs_get_at_x(index_t index);


#endif