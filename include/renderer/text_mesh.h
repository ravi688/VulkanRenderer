/*
	@https://en.wikipedia.org/wiki/Eigenvalues_and_eigenvectors
 	EIGEN VALUES AND EIGEN VECTORS OF TRANSFORMATION MATRICES
 	
 	mat = rot_mat * trans_mat * scale_mat

 	scale_mat * pos = pos + offset_vector;
 	(scale_mat - ident_mat) * pos = offset_vector
 	(S - I) * P = offset_vector
 	let direction of P equals to direction of offset_vector, then
 	S * P = lamda * P (characteristic equation)
 	where, lamda = eigen_value and P = eigen_vector

	(S - I * lamda) * P = null_vector
	
	let A = S - I * lamda, then,
	A * P = null_vector => A * inverse(A) * P = null_vector * inverse(A)
						=> P = null_vector * inverse(A) = null_vector
	But since we want a non-null vector solution to the characteristic solution, the inverse of A shouldn't exist
	So, 
		det(A) should be zero,
		=> det(S - I * lamda) = 0

	By applying fundamental theorem of algebra, 
		det(S - I * lamda) = (lamda1 - lamda) * (lamda2 - lamda) * (lamda3 - lamda) * (lamda4 - lamda) * ... * (lamdan - lamda)

	ALGEBRAIC MULTIPLICITY

	det(S - lamda * I) = pow((lamda1 - lamda), n1) * pow((lamda2 - lamda), n2) * pow((lamda3 - lamda), n3) ...
	__ n
	\  ni = order of the matrix S, and 1 <= ni <= order of the matrix S
	/_
	i = 1

	PROPERTIES OF EIGEN VALUES
	
	Sum of eigen values equals the trace of the matrix
				__ n
	trace(A) = 	\ 	lamda_i = lamda_1 + lamda_2 + lamda_3 + ... + lamda_n
				/_ i = 1

	Product of eigen values equals the determinant of the matrix
			 ___ n
	det(A) = | | lamda_i = lamda_1  * lamda_2 * lamda_3 * lamda_4 * ... * lamda_n
			 | | i = 1

	@https://en.wikipedia.org/wiki/Affine_transformation#Augmented_matrix
	AFFINE TRANSFORMATION
	
	Examples of affine transformations include 
	translation, 
	scaling, 
	homothety, @https://en.wikipedia.org/wiki/Homothety
	similarity,  
	reflection, 
	rotation, 
	shear mapping, 
	and compositions of them in any combination and sequence.

	An affine transformation preserves: 
	1. collinearity between points
	2. parallelism
	3. convexity of sets @https://en.wikipedia.org/wiki/Convex_set
	4. barycenters of weighted collections of points
	5. ratios of lengths of parallel line segments

	An affine map is a composition of two functions: a translation and a linear map
	y = f(x) = Ax + b => this could be represented by just one single augmented matrix

	@https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati/3554913
	DECOMPOSING TRANSFORMATION MATRIX
	
	TODO
*/

#pragma once

#include <renderer/glyph_mesh_pool.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

#include <buffer.h>

typedef struct renderer_t renderer_t;
typedef struct text_mesh_t text_mesh_t;

// constructors and destructors
text_mesh_t* text_mesh_new();
text_mesh_t* text_mesh_create(renderer_t* renderer, glyph_mesh_pool_t* pool);
void text_mesh_destroy(text_mesh_t* text);
void text_mesh_release_resources(text_mesh_t* text);

// logic functions
void text_mesh_draw(text_mesh_t* text);


typedef buf_ucount_t text_mesh_string_handle_t;

// constructors and destructors
text_mesh_string_handle_t text_mesh_string_create(text_mesh_t* text_mesh);
void text_mesh_string_destroyH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);

// setters
void text_mesh_string_setH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, const char* string);
void text_mesh_string_set_scaleH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t(float) scale);
void text_mesh_string_set_positionH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t(float) position);
void text_mesh_string_set_rotationH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, vec3_t(float) rotation);
void text_mesh_string_set_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle, mat4_t(float) transform);

// getters
const char* text_mesh_string_getH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
vec3_t(float) text_mesh_string_get_scaleH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
vec3_t(float) text_mesh_string_get_positionH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
vec3_t(float) text_mesh_string_get_rotationH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
mat4_t(float) text_mesh_string_get_transformH(text_mesh_t* text_mesh, text_mesh_string_handle_t handle);
