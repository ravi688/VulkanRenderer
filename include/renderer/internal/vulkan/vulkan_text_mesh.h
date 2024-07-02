/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_text_mesh.h is a part of VulkanRenderer

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

#include <renderer/defines.h>
#include <hpml/vec3.h>
#include <hpml/mat4.h>
#include <bufferlib/buffer.h>
#include <renderer/dictionary.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_text_types.h>
#include <renderer/internal/vulkan/vulkan_object.h>
#include <glslcommon/glsl_types.h>

typedef struct vulkan_text_mesh_glsl_glyph_render_data_t
{
	/* offset of the glyph instance */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t ofst;
	/* string id of this glyph instance */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 stid;
} vulkan_text_mesh_glsl_glyph_render_data_t ALIGN_AS(U32_MAX_OF(GLSL_STD140_VEC4_ALIGN, GLSL_STD140_UINT_ALIGN));

#define SIZEOF_VULKAN_TEXT_MESH_GLSL_GLYPH_RENDER_DATA_T 16
#define STRIDE_VULKAN_TEXT_MESH_GLSL_GLYPH_RENDER_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_VULKAN_TEXT_MESH_GLSL_GLYPH_RENDER_DATA_T, ALIGN_OF(vulkan_text_mesh_glsl_glyph_render_data_t))

typedef enum vulkan_text_mesh_render_space_type_t
{
	/* selects the 'screen projection matrix' from the camera's projection matrices */
	VULKAN_TEXT_MESH_RENDER_SPACE_TYPE_2D,
	/* selects the 'orthographic/perspective matrix' from the camera's projection matrices */
	VULKAN_TEXT_MESH_RENDER_SPACE_TYPE_3D
} vulkan_text_mesh_render_space_type_t;

typedef enum vulkan_text_mesh_render_surface_type_t
{
	/* the text will be rendered on to the camera's view */
	VULKAN_TEXT_MESH_RENDER_SURFACE_TYPE_CAMERA,
	/* the text will be rendered on to the screen (i.e. will not be affected by the any camera's transformations or projections) */
	VULKAN_TEXT_MESH_RENDER_SURFACE_TYPE_SCREEN
} vulkan_text_mesh_render_surface_type_t;

typedef buf_ucount_t vulkan_text_mesh_string_handle_t;
typedef dictionary_t /* (u16, sub_buffer_handle_t) */ vulkan_text_mesh_glyph_render_data_sub_buffer_handle_list_t;
typedef buffer_t vulkan_text_mesh_string_char_buffer_t;

/* <begin> typedefs facilitating text layout */

typedef vulkan_text_glyph_layout_data_t vulkan_text_mesh_glyph_layout_data_t;

typedef vulkan_text_glyph_layout_data_buffer_t vulkan_text_mesh_glyph_layout_data_buffer_t;

typedef vulkan_text_glyph_info_t vulkan_text_mesh_glyph_info_t;

typedef vulkan_text_glyph_layout_handler_input_data_t vulkan_text_mesh_glyph_layout_handler_input_data_t;

typedef vulkan_text_glyph_layout_handler_t vulkan_text_mesh_glyph_layout_handler_t;

typedef vulkan_text_glyph_strikethrough_handler_t vulkan_text_mesh_glyph_strikethrough_handler_t;

typedef vulkan_text_glyph_strikethrough_handler_t vulkan_text_mesh_glyph_strikethrough_handler_t;

typedef vulkan_text_glyph_underline_handler_t vulkan_text_mesh_glyph_underline_handler_t;

typedef vulkan_text_glyph_layout_handler_void_ptr_pair_t vulkan_text_mesh_glyph_layout_handler_void_ptr_pair_t;
typedef vulkan_text_glyph_strikethrough_handler_void_ptr_pair_t vulkan_text_mesh_glyph_strikethrough_handler_void_ptr_pair_t;
typedef vulkan_text_glyph_underline_handler_void_ptr_pair_t vulkan_text_mesh_glyph_underline_handler_void_ptr_pair_t;

/* <end> typedefs facilitating text layout */

typedef struct vulkan_text_mesh_string_t
{
	vulkan_text_mesh_string_handle_t next_handle;
	vulkan_text_mesh_string_handle_t handle;												// 	index of the next string in the buffer
	vulkan_text_mesh_glyph_render_data_sub_buffer_handle_list_t glyph_sub_buffer_handles;	//	dictionary of sub_buffer handles in the multi_buffer of glyphs
	vulkan_text_mesh_string_char_buffer_t str; 															//	buffer of characters to store the supplied string
	mat4_t transform;
	u32 point_size;
} vulkan_text_mesh_string_t;

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_material_t vulkan_material_t;
typedef struct vulkan_glyph_mesh_pool_t vulkan_glyph_mesh_pool_t;
typedef buffer_t /* vulkan_text_mesh_string_t */ vulkan_text_mesh_string_list_t;
typedef dictionary_t /* u16, vulkan_host_multibuffered_buffer_t */ vulkan_text_mesh_glyph_render_data_buffer_map_t;
typedef vulkan_host_buffered_buffer_t /* glsl_mat4_t */ vulkan_text_mesh_string_transform_buffer_t;
typedef struct font_t font_t;

typedef struct vulkan_text_mesh_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_material_t* material;
	u32 point_size;
	vulkan_text_mesh_render_space_type_t render_space_type;
	vulkan_text_mesh_render_surface_type_t render_surface_type;

	/* <begin> fields facilitating text layout */

		/* called whenever vulkan_text_mesh_string_setH is called */
		vulkan_text_mesh_glyph_layout_handler_void_ptr_pair_t glyph_layout_handler;
		/* called whenever vulkan_text_mesh_glyph_layout_data_t::is_strikethrough is true */
		vulkan_text_mesh_glyph_strikethrough_handler_void_ptr_pair_t glyph_strikethrough_handler;
		/* called whenever vulkan_text_mesh_glyph_layout_data_t::is_underline is true */
		vulkan_text_mesh_glyph_underline_handler_void_ptr_pair_t glyph_underline_handler;

		/* holds post processed info for each glyph */
		vulkan_text_mesh_glyph_layout_data_buffer_t glyph_layout_data_buffer;

	/* <end> fields facilitating text layout */

	/* CPU side */
	vulkan_glyph_mesh_pool_t* pool;												// pool from which the mesh_t should be queried from for a particular glyph
	vulkan_text_mesh_string_list_t strings;									// array of string (text_mesh_string_t) instances
	buf_ucount_t free_list;
	buf_ucount_t inuse_list;

	/* GPU Side */
	vulkan_text_mesh_glyph_render_data_buffer_map_t glyph_render_data_buffers;		// array of instance_buffer mapped to each unique glyph considering all the strings
	vulkan_text_mesh_string_transform_buffer_t text_string_transform_buffer;
} vulkan_text_mesh_t;

#define VULKAN_TEXT_MESH(ptr) VULKAN_OBJECT_UP_CAST(vulkan_text_mesh_t*, VULKAN_OBJECT_TYPE_TEXT_MESH, ptr)
#define VULKAN_TEXT_MESH_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_text_mesh_t*, VULKAN_OBJECT_TYPE_TEXT_MESH, ptr)

BEGIN_CPP_COMPATIBLE

// constructors and destructors
SGE_API vulkan_text_mesh_t* vulkan_text_mesh_new(memory_allocator_t* allocator);
SGE_API vulkan_text_mesh_t* vulkan_text_mesh_create(vulkan_renderer_t* renderer, vulkan_glyph_mesh_pool_t* pool);
SGE_API void vulkan_text_mesh_create_no_alloc(vulkan_renderer_t* renderer, vulkan_glyph_mesh_pool_t* pool, vulkan_text_mesh_t OUT text);
SGE_API void vulkan_text_mesh_destroy(vulkan_text_mesh_t* text);
SGE_API void vulkan_text_mesh_release_resources(vulkan_text_mesh_t* text);

// logic functions
SGE_API void vulkan_text_mesh_draw(vulkan_text_mesh_t* text);

// constructors and destructors
SGE_API vulkan_text_mesh_string_handle_t vulkan_text_mesh_string_create(vulkan_text_mesh_t* text_mesh);
SGE_API void vulkan_text_mesh_string_destroyH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle);

// setters
SGE_API void vulkan_text_mesh_set_glyph_layout_handler(vulkan_text_mesh_t* text, vulkan_text_mesh_glyph_layout_handler_t handler, void* user_data);
SGE_API void vulkan_text_mesh_set_glyph_strikethrough_handler(vulkan_text_mesh_t* text, vulkan_text_mesh_glyph_strikethrough_handler_t handler, void* user_data);
SGE_API void vulkan_text_mesh_set_glyph_underline_handler(vulkan_text_mesh_t* text, vulkan_text_mesh_glyph_underline_handler_t handler, void* user_data);
SGE_API void vulkan_text_mesh_set_point_size(vulkan_text_mesh_t* text_mesh, u32 point_size);
SGE_API void vulkan_text_mesh_set_material(vulkan_text_mesh_t* text_mesh, vulkan_material_t* material);
SGE_API void vulkan_text_mesh_set_render_space_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_space_type_t space_type);
SGE_API void vulkan_text_mesh_set_render_surface_type(vulkan_text_mesh_t* text, vulkan_text_mesh_render_surface_type_t surface_type);
SGE_API void vulkan_text_mesh_string_setH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, const char* string);
SGE_API void vulkan_text_mesh_string_set_point_sizeH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, u32 point_size);
SGE_API void vulkan_text_mesh_string_set_transformH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle, mat4_t transform);

// getters
SGE_API u32 vulkan_text_mesh_get_point_size(vulkan_text_mesh_t* text);
SGE_API const char* vulkan_text_mesh_string_getH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle);
SGE_API u32 vulkan_text_mesh_string_get_point_sizeH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle);
SGE_API mat4_t vulkan_text_mesh_string_get_transformH(vulkan_text_mesh_t* text_mesh, vulkan_text_mesh_string_handle_t handle);
SGE_API font_t* vulkan_text_mesh_get_font(vulkan_text_mesh_t* text_mesh);

END_CPP_COMPATIBLE
