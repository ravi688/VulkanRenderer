
#include <renderer/mesh3d.h>
#include <memory.h>
#include <stdlib.h>

//For mat4_vec4_visitor
#include <hpml/vec4.h>
#include <hpml/affine_transformation.h>

//For mesh3d_load()
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>
#include <meshlib/stl/readers/ascii.h>
#include <meshlib/stl/readers/binary.h>
#include <meshlib/obj/readers/ascii.h>
#include <meshlib/stl/parse_callbacks.h>
#include <meshlib/obj/parse_callbacks.h>
#include <string.h>
#define EXTENSION_STL ".stl"
#define EXTENSION_OBJ ".obj"
#define STL_ASCII_HEADER "solid"
#define STL_MINIMUM_ASCII_FILE_LENGTH 0
#define STL_MINIMUM_BINARY_FILE_LENGTH 0
#define OBJ_MINIMUM_ASCII_FILE_LENGTH 0
#define OBJ_MINIMUM_BINAIRY_FILE_LEGTH 0


//These header should be include at the last because there maybe previous definitions of ASSERT from other library headers
#include <renderer/defines.h>
#include <renderer/assert.h>

#include <renderer/alloc.h>

#define __UVS_ARE_NOT_FOUND__ "mesh3d_t::uvs are not found, make sure to call mesh3d_uvs_new(count) first\n"
#define __NORMALS_ARE_NOT_FOUND__ "mesh3d_t::normals are not found, make sure to call mesh3d_normals_new(count) first\n"
#define __TANGENTS_ARE_NOT_FOUND__ "mesh3d_t::tangents are not found, make sure to call mesh3d_tangents_new(count) first\n"
#define __COLORS_ARE_NOT_FOUND__ "mesh3d_t::colors are not found, make sure to call mesh3d_colors_new(count) first\n"
#define __POSITIONS_ARE_NOT_FOUND__ "mesh3d_t::positions are not found, make sure to call mesh3d_positions_new(count) first\n"
#define __TRIANGLES_ARE_NOT_FOUND__ "mesh3d_t::triangles are not found, make sure to call mesh3d_triangles_new(count) first\n"

#define mesh3d_foreach(...) define_alias_function_macro(mesh3d_foreach, __VA_ARGS__)
static function_signature(void, mesh3d_foreach, mesh3d_t* mesh, BUFFER* buffer, func_ptr_sig(void*, getter, mesh3d_t*, index_t), void (*visitor)(void*, void*), void* user_data);


RENDERER_API function_signature_void(mesh3d_t*, mesh3d_new)
{
	CALLTRACE_BEGIN();
	ASSERT(sizeof(buf_ucount_t) == sizeof(u64), "sizeof(buf_ucount_t) != sizeof(u64), this has to be 8 bytes because of VK_INDEX_TYPE_U64\n");
	mesh3d_t* mesh  = heap_new(mesh3d_t);
	memzero(mesh, mesh3d_t);
	mat4_move(mesh->transform, mat4_identity());
	CALLTRACE_RETURN(mesh);
}

RENDERER_API function_signature(void, mesh3d_positions_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->positions != NULL)
	{
		buf_free(mesh->positions);
		mesh->positions = NULL;
	}
	mesh->positions = BUFcreate(NULL, sizeof(vec3_t), count, 0);
	ASSERT(mesh->positions != NULL, "mesh->positions == NULL\n");
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normals_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->normals != NULL)
	{
		buf_free(mesh->normals);
		mesh->normals = NULL;
	}
	mesh->normals = BUFcreate(NULL, sizeof(vec3_t), count, 0);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangents_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->tangents != NULL)
	{
		buf_free(mesh->tangents);
		mesh->tangents = NULL;
	}
	mesh->tangents = BUFcreate(NULL, sizeof(vec3_t), count, 0);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_colors_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->colors != NULL)
	{
		buf_free(mesh->colors);
		mesh->colors = NULL;
	}
	mesh->colors = BUFcreate(NULL, sizeof(vec3_t), count, 0);
	CALLTRACE_END();
}


RENDERER_API function_signature(void, mesh3d_triangles_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->triangles != NULL)
	{
		buf_free(mesh->triangles);
		mesh->triangles = NULL;
	}
	mesh->triangles = BUFcreate(NULL, sizeof(vec3uint_t), count, 0);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uvs_new, mesh3d_t* mesh, index_t count)
{
	CALLTRACE_BEGIN();
	if(mesh->uvs != NULL)
	{
		buf_free(mesh->uvs);
		mesh->uvs = NULL;
	}
	mesh->uvs = BUFcreate(NULL, sizeof(vec2_t), count, 0);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_positions_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->positions == NULL) return;
	buf_free(mesh->positions);
	mesh->positions = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normals_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->normals == NULL) return;
	buf_free(mesh->normals);
	mesh->normals = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangents_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->tangents == NULL) return;
	buf_free(mesh->tangents);
	mesh->tangents = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_colors_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->colors == NULL) return;
	buf_free(mesh->colors);
	mesh->colors = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangles_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->triangles == NULL) return;
	buf_free(mesh->triangles);
	mesh->triangles = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uvs_free, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->uvs == NULL) return;
	buf_free(mesh->uvs);
	mesh->uvs = NULL;
	CALLTRACE_END();
}

RENDERER_API function_signature(index_t, mesh3d_positions_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->positions != NULL, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->positions));
}

RENDERER_API function_signature(index_t, mesh3d_normals_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->normals != NULL, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->normals));
}

RENDERER_API function_signature(index_t, mesh3d_tangents_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->tangents != NULL, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->tangents));
}

RENDERER_API function_signature(index_t, mesh3d_uvs_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->uvs != NULL, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->uvs));
}

RENDERER_API function_signature(index_t, mesh3d_colors_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->colors != NULL, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->colors));
}
RENDERER_API function_signature(index_t, mesh3d_triangles_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->triangles != NULL, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->triangles));
}

RENDERER_API function_signature(void, mesh3d_destroy, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();

	if(mesh->positions != NULL)
		buf_free(mesh->positions);

	if(mesh->normals != NULL)
		buf_free(mesh->normals);

	if(mesh->tangents != NULL)
		buf_free(mesh->tangents);

	if(mesh->colors != NULL)
		buf_free(mesh->colors);

	if(mesh->uvs != NULL)
		buf_free(mesh->uvs);

	if(mesh->positions != NULL)
		buf_free(mesh->triangles);

	heap_free(mesh);

	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_optimize_buffer, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	if(mesh->positions != NULL)
		buf_fit(mesh->positions);
	if(mesh->triangles != NULL)
		buf_fit(mesh->triangles);
	if(mesh->colors != NULL)
		buf_fit(mesh->colors);
	if(mesh->uvs != NULL)
		buf_fit(mesh->uvs);
	if(mesh->normals != NULL)
		buf_fit(mesh->normals);
	if(mesh->tangents != NULL)
		buf_fit(mesh->tangents);

	CALLTRACE_END();
}

RENDERER_API function_signature(bool, mesh3d_has_normals, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->normals != NULL) && (buf_get_element_count(mesh->normals) > 0));
}

RENDERER_API function_signature(bool, mesh3d_has_tangents, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->tangents != NULL) && (buf_get_element_count(mesh->tangents) > 0));
}

RENDERER_API function_signature(bool, mesh3d_has_colors, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->colors != NULL) && (buf_get_element_count(mesh->colors) > 0));
}


RENDERER_API function_signature(bool, mesh3d_has_positions, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->positions != NULL) && (buf_get_element_count(mesh->positions) > 0));
}

RENDERER_API function_signature(bool, mesh3d_has_uvs, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->uvs != NULL) && (buf_get_element_count(mesh->uvs) > 0));
}

RENDERER_API function_signature(bool, mesh3d_has_triangles, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN((mesh->triangles != NULL) && (buf_get_element_count(mesh->triangles) > 0));
}

RENDERER_API function_signature(void, mesh3d_flip_triangles, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	assert(mesh != NULL);
	buf_ucount_t triangle_count = mesh3d_triangles_count(mesh);
	for(buf_ucount_t i = 0; i < triangle_count; i++)
	{
		vec3uint_t* triangle = mesh3d_triangle_get_ptr(mesh, i);
		index_t t = triangle->x;
		triangle->x = triangle->y;
		triangle->y = t;
	}
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_calculate_normals, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	log_fetal_err("Undefined method\n");
	CALLTRACE_END();
}

/*
	CALCULATING TANGENTS

	v0 = (-1.5f, 0, 1.5f)
	v1 = (1.5f, 0, -1.5f)
	v2 = (1.5f, 0, 1.5f)

	(s0, t0) = (1, 0)
	(s1, t1) = (0, 1)
	(s2, t2) = (1, 1)

	e1 = (3, 0, -3)
	e2 = (3, 0, 0)
	duv1 (s, t) = (-1, 1)
	duv2 (s, t) = (0, 1)

	B = (1, 0, 0) 		- Expected
	T = (0, 0, 1) 		- Expected

	B * duv1.t + T * duv1.s = e1
	B * duv2.t + T * duv2.s = e2

	B = (e1 * duv2.s - e2 * duv1.s)	/ (duv1.t * duv2.s - duv1.s * duv2.t)
	T = (e1 * duv2.t - e2 * duv1.t) / (duv1.s * duv2.t - duv1.t * duv2.s)

 */

static vec3_t get_tangent(mesh3d_t* mesh, index_t i0, index_t i1, index_t i2)
{
	vec2_t duv1 = vec2_sub(mesh3d_uv_get(mesh, i1), mesh3d_uv_get(mesh, i0));
	vec2_t duv2 = vec2_sub(mesh3d_uv_get(mesh, i2), mesh3d_uv_get(mesh, i0));
	vec3_t e1 = vec3_sub(2, mesh3d_position_get(mesh, i1), mesh3d_position_get(mesh, i0));
	vec3_t e2 = vec3_sub(2, mesh3d_position_get(mesh, i2), mesh3d_position_get(mesh, i0));
	vec3_t tangent = vec3_sub(2, vec3_scale(e1, duv2.y), vec3_scale(e2, duv1.y));
	float value = 1 / (duv1.x * duv2.y - duv1.y * duv2.x);
	tangent = vec3_scale(tangent, value);
	return tangent;
}
RENDERER_API function_signature(void, mesh3d_calculate_tangents, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	assert(mesh != NULL);
	assert_wrn(mesh3d_positions_count(mesh) >= 3); 		// there should be at least one triangle

	// allocate memory for tangents and initialize them with vec3_zero
	buf_ucount_t vertex_count = mesh3d_positions_count(mesh);
	mesh3d_tangents_new(mesh, vertex_count);
	for(buf_ucount_t i = 0; i < vertex_count; i++)
		mesh3d_tangent_add_vec3(mesh, vec3_zero());

	buf_ucount_t triangle_count = mesh3d_triangles_count(mesh);
	for(buf_ucount_t i = 0; i < triangle_count; i++)
	{
		vec3uint_t triangle = mesh3d_triangle_get(mesh, i);
		// calculate the tangent for this triangle
		vec3_t tangent = get_tangent(mesh, triangle.x, triangle.y, triangle.z);
		// add this tangent to each corner/vertex of this triangle
		mesh3d_tangent_set_vec3(mesh, triangle.x, vec3_add(2, mesh3d_tangent_get(mesh, triangle.x), tangent));
		mesh3d_tangent_set_vec3(mesh, triangle.y, vec3_add(2, mesh3d_tangent_get(mesh, triangle.y), tangent));
		mesh3d_tangent_set_vec3(mesh, triangle.z, vec3_add(2, mesh3d_tangent_get(mesh, triangle.z), tangent));
	}

	// normalize all the tangents and Gram-Schimdt orthogonalize
	for(buf_ucount_t i = 0; i < vertex_count; i++)
	{
		vec3_t tangent = mesh3d_tangent_get(mesh, i);
		vec3_t normal = mesh3d_normal_get(mesh, i);
		tangent = vec3_sub(2, tangent, vec3_scale(normal, vec3_dot(tangent, normal)));
		mesh3d_tangent_set_vec3(mesh, i, vec3_normalize(tangent));
	}
	
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	vec3_t v = {x, y, z};
	buf_push(mesh->positions, &v);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_add_vec3, mesh3d_t* mesh, vec3_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_push(mesh->positions, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	vec3_t* v = buf_get_ptr_at_typeof(mesh->positions, vec3_t, index);
	v->x = x; v->y = y; v->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  position)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_set_at(mesh->positions, index, (void*)(&position));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->x = x;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_position_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec3_t,  mesh3d_position_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->positions, vec3_t, index));
}

RENDERER_API function_signature(vec3_t*, mesh3d_position_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t, index));
}

RENDERER_API function_signature(float, mesh3d_position_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->x);
}

RENDERER_API function_signature(float, mesh3d_position_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->y);
}

RENDERER_API function_signature(float, mesh3d_position_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t, index)->z);
}

RENDERER_API function_signature(void, mesh3d_normal_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	vec3_t n = {x, y, z};
	buf_push(mesh->normals, &n);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_add_vec3, mesh3d_t* mesh, vec3_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_push(mesh->normals, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	vec3_t* n = buf_get_ptr_at_typeof(mesh->normals, vec3_t, index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  normal)
{
	CALLTRACE_BEGIN();
 	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->normals, index, (void*)(&normal));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->x = x;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normal_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec3_t, mesh3d_normal_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->normals, vec3_t, index));
}

RENDERER_API function_signature(vec3_t*, mesh3d_normal_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t, index));
}

RENDERER_API function_signature(float, mesh3d_normal_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->x);
}

RENDERER_API function_signature(float, mesh3d_normal_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->y);
}

RENDERER_API function_signature(float, mesh3d_normal_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t, index)->z);
}


RENDERER_API function_signature(void, mesh3d_tangents_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	vec3_t n = {x, y, z};
	buf_push(mesh->tangents, &n);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_add_vec3, mesh3d_t* mesh, vec3_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	buf_push(mesh->tangents, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	vec3_t* n = buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  tangent)
{
	CALLTRACE_BEGIN();
 	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->tangents, index, (void*)(&tangent));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->x = x;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangent_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec3_t, mesh3d_tangent_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index));
}

RENDERER_API function_signature(vec3_t*, mesh3d_tangent_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index));
}

RENDERER_API function_signature(float, mesh3d_tangent_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->x);
}

RENDERER_API function_signature(float, mesh3d_tangent_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->y);
}

RENDERER_API function_signature(float, mesh3d_tangent_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->tangents, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->tangents, vec3_t, index)->z);
}

RENDERER_API function_signature(void, mesh3d_color_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	vec3_t n = {x, y, z};
	buf_push(mesh->colors, &n);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_add_vec3, mesh3d_t* mesh, vec3_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_push(mesh->colors, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	vec3_t* n = buf_get_ptr_at_typeof(mesh->colors, vec3_t, index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_set_vec3, mesh3d_t* mesh, index_t index, vec3_t  color)
{
	CALLTRACE_BEGIN();
 	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->colors, index, (void*)(&color));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->x = x;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_color_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->z = z;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec3_t, mesh3d_color_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->colors, vec3_t, index));
}

RENDERER_API function_signature(vec3_t*, mesh3d_color_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t, index));
}

RENDERER_API function_signature(float, mesh3d_color_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->x);
}

RENDERER_API function_signature(float, mesh3d_color_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->y);
}

RENDERER_API function_signature(float, mesh3d_color_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t, index)->z);
}


RENDERER_API function_signature(void, mesh3d_triangle_add, mesh3d_t* mesh, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	vec3uint_t i = {i0, i1, i2};
	buf_push(mesh->triangles, &i);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_add_vec3, mesh3d_t* mesh, vec3uint_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_push(mesh->triangles, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_set, mesh3d_t* mesh, index_t index, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	vec3uint_t* t =  buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index);
	t->x = i0; t->y = i1; t->z = i2;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_set_vec3, mesh3d_t* mesh, index_t index, vec3uint_t  v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_set_at(mesh->triangles, index, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_set_0, mesh3d_t* mesh, index_t index, index_t i0)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->x = i0;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_set_1, mesh3d_t* mesh, index_t index, index_t i1)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->x = i1;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_triangle_set_2, mesh3d_t* mesh, index_t index, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->x = i2;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec3uint_t,  mesh3d_triangle_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index));
}

RENDERER_API function_signature(vec3uint_t*, mesh3d_triangle_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index));
}

RENDERER_API function_signature(int, mesh3d_triangle_get_0, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->x);
}

RENDERER_API function_signature(int, mesh3d_triangle_get_1, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->y);
}

RENDERER_API function_signature(int, mesh3d_triangle_get_2, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3uint_t, index)->z);
}

RENDERER_API function_signature(void, mesh3d_uv_add, mesh3d_t* mesh, float x, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	vec2_t uv = {x, y};
	buf_push(mesh->uvs, &uv);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uv_add_vec2, mesh3d_t* mesh, vec2_t v)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_push(mesh->uvs, (void*)(&v));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uv_set, mesh3d_t* mesh, index_t index, float x, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	vec2_t* v = buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index);
	v->x = x; v->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uv_set_vec2, mesh3d_t* mesh, index_t index, vec2_t  uv)
{
	CALLTRACE_BEGIN();
 	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->uvs, index, (void*)(&uv));
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uv_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index)->x = x;
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uv_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index)->y = y;
	CALLTRACE_END();
}

RENDERER_API function_signature(vec2_t,  mesh3d_uv_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index));
}

RENDERER_API function_signature(vec2_t*, mesh3d_uv_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index));
}

RENDERER_API function_signature(float, mesh3d_uv_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index)->y);
}

RENDERER_API function_signature(float, mesh3d_uv_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t, index)->x);
}


RENDERER_API function_signature(index_t, mesh3d_sizeof_position, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->positions != NULL, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->positions));
}
RENDERER_API function_signature(index_t, mesh3d_sizeof_normal, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->normals != NULL, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->normals));
}
RENDERER_API function_signature(index_t, mesh3d_sizeof_tangent, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->tangents != NULL, __TANGENTS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->tangents));
}
RENDERER_API function_signature(index_t, mesh3d_sizeof_color, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->colors != NULL, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->colors));
}
RENDERER_API function_signature(index_t, mesh3d_sizeof_uv, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->uvs != NULL, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->uvs));
}
RENDERER_API function_signature(index_t, mesh3d_sizeof_index, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh->triangles != NULL, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_size(mesh->triangles) / 3);
}

RENDERER_API function_signature(mesh3d_t*, mesh3d_plane, float size)
{
	CALLTRACE_BEGIN();
	mesh3d_t* mesh = mesh3d_new();
	mesh3d_positions_new(mesh, 4);
	mesh3d_triangles_new(mesh, 2);
	mesh3d_normals_new(mesh, 4);
	mesh3d_uvs_new(mesh, 4);

	float half_size = size * 0.5f;

	mesh3d_position_add(mesh, half_size,  0, half_size);
	mesh3d_position_add(mesh, half_size,  0, -half_size);
	mesh3d_position_add(mesh, -half_size, 0, -half_size);
	mesh3d_position_add(mesh, -half_size, 0, half_size);

	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 0, 0);
	mesh3d_uv_add(mesh, 1, 0);

	mesh3d_triangle_add(mesh, 3, 1, 0);
	mesh3d_triangle_add(mesh, 3, 2, 1);

	mesh3d_normal_add(mesh, 0, 1, 0);
	mesh3d_normal_add(mesh, 0, 1, 0);
	mesh3d_normal_add(mesh, 0, 1, 0);
	mesh3d_normal_add(mesh, 0, 1, 0);

	mesh3d_optimize_buffer(mesh);
	CALLTRACE_RETURN(mesh);
}

RENDERER_API function_signature(mesh3d_t*, mesh3d_cube, float size)
{
	CALLTRACE_BEGIN();
	mesh3d_t* mesh = mesh3d_new();
	mesh3d_positions_new(mesh, 24);
	mesh3d_triangles_new(mesh, 12);
	mesh3d_normals_new(mesh, 24);
	mesh3d_uvs_new(mesh, 24);

	float half_size = 0.5f * size;

	//Bottom
	mesh3d_position_add(mesh, -half_size, -half_size, half_size );
	mesh3d_position_add(mesh, -half_size, -half_size, -half_size);
	mesh3d_position_add(mesh, half_size, -half_size, -half_size );
	mesh3d_position_add(mesh, half_size, -half_size, half_size  );
	mesh3d_uv_add(mesh, 1, 0);
	mesh3d_uv_add(mesh, 0, 0);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 1, 1);

	//Top
	mesh3d_position_add(mesh, half_size, half_size, half_size  );
	mesh3d_position_add(mesh, half_size, half_size, -half_size );
	mesh3d_position_add(mesh, -half_size, half_size, -half_size);
	mesh3d_position_add(mesh, -half_size, half_size, half_size );
	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 0, 0);
	mesh3d_uv_add(mesh, 1, 0);


	//Front
	mesh3d_position_add(mesh, -half_size, half_size, -half_size);
	mesh3d_position_add(mesh, -half_size, half_size, half_size );
	mesh3d_position_add(mesh, -half_size, -half_size, half_size );
	mesh3d_position_add(mesh, -half_size, -half_size, -half_size);
	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 0, 0);
	mesh3d_uv_add(mesh, 1, 0);

	//Left
	mesh3d_position_add(mesh, half_size, half_size, -half_size );
	mesh3d_position_add(mesh, -half_size, half_size, -half_size);
	mesh3d_position_add(mesh, -half_size, -half_size, -half_size);
	mesh3d_position_add(mesh, half_size, -half_size, -half_size );
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 1, 0);
	mesh3d_uv_add(mesh, 0, 0);


	//Right
	mesh3d_position_add(mesh, -half_size, half_size, half_size);
	mesh3d_position_add(mesh, half_size, half_size, half_size );
	mesh3d_position_add(mesh, half_size, -half_size, half_size );
	mesh3d_position_add(mesh, -half_size, -half_size, half_size);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 1, 0);
	mesh3d_uv_add(mesh, 0, 0);

	//Back
	mesh3d_position_add(mesh, half_size, half_size, half_size );
	mesh3d_position_add(mesh, half_size, half_size, -half_size);
	mesh3d_position_add(mesh, half_size, -half_size, -half_size);
	mesh3d_position_add(mesh, half_size, -half_size, half_size );
	mesh3d_uv_add(mesh, 1, 1);
	mesh3d_uv_add(mesh, 0, 1);
	mesh3d_uv_add(mesh, 0, 0);
	mesh3d_uv_add(mesh, 1, 0);



	//clockwise order
	mesh3d_triangle_add(mesh, 2, 1, 0);
	mesh3d_triangle_add(mesh, 3, 2, 0);
	mesh3d_triangle_add(mesh, 6, 5, 4),
	mesh3d_triangle_add(mesh, 7, 6, 4);
	mesh3d_triangle_add(mesh, 8, 9, 10);
	mesh3d_triangle_add(mesh, 8, 10, 11);
	mesh3d_triangle_add(mesh, 12, 13, 14);
	mesh3d_triangle_add(mesh, 12, 14, 15);
	mesh3d_triangle_add(mesh, 16, 17, 18);
	mesh3d_triangle_add(mesh, 16, 18, 19);
	mesh3d_triangle_add(mesh, 20, 21, 22);
	mesh3d_triangle_add(mesh, 20, 22, 23);

	// normals pointing down
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, -1.0f, 0);

	// normals pointing up
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 1.0f, 0);

	// normals pointing back
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, -1.0f, 0, 0);
	
	// normals pointing left
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, -1.0f);

	// normals pointing right
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, 1.0f);

	// normals pointing forward
	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 1.0f, 0, 0);


	mesh3d_optimize_buffer(mesh);
	CALLTRACE_RETURN(mesh);
}


RENDERER_API function_signature(void, mesh3d_positions_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* position, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_positions(mesh), "!mesh3d_has_positions(mesh)\n");
	mesh3d_foreach(mesh, mesh->positions, (void*)func_ptr(mesh3d_position_get_ptr), (void*)visitor, user_data);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_normals_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* normal, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_normals(mesh), "!mesh3d_has_normals(mesh)\n");
	mesh3d_foreach(mesh, mesh->normals, (void*)func_ptr(mesh3d_normal_get_ptr), (void*)visitor, user_data);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_tangents_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* normal, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_tangents(mesh), "!mesh3d_has_tangents(mesh)\n");
	mesh3d_foreach(mesh, mesh->tangents, (void*)func_ptr(mesh3d_tangent_get_ptr), (void*)visitor, user_data);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_colors_foreach, mesh3d_t* mesh, void (*visitor)(vec3_t* color, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_colors(mesh), "!mesh3d_has_colors(mesh)\n");
	mesh3d_foreach(mesh, mesh->colors, (void*)func_ptr(mesh3d_color_get_ptr), (void*)visitor, user_data);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, mesh3d_uvs_foreach, mesh3d_t* mesh, void (*visitor)(vec2_t* uv, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_uvs(mesh), "!mesh3d_has_uvs(mesh)\n");
	mesh3d_foreach(mesh, mesh->uvs, (void*)func_ptr(mesh3d_uv_get_ptr), (void*)visitor, user_data);
 	CALLTRACE_END();
}


RENDERER_API function_signature(void, mesh3d_triangles_foreach, mesh3d_t* mesh, void (*visitor)(vec3uint_t* triangle, void* user_data), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_triangles(mesh), "!mesh3d_has_triangles(mesh)\n");
	mesh3d_foreach(mesh, mesh->triangles, (void*)func_ptr(mesh3d_triangle_get_ptr), (void*)visitor, user_data);
	CALLTRACE_END();
}

/*[CurrentlyNotUsed]*/
static void direction_visitor(vec3_t* v, mat4_t* m);
static void position_visitor(vec3_t* v, mat4_t* m);

RENDERER_API function_signature(void, mesh3d_transform_set, mesh3d_t* mesh, mat4_t transform)
{
	mesh3d_positions_foreach(mesh, (void*)position_visitor, &transform);
	//TODO: mesh3d_normals_foreach(mesh, (void*)direction_visitor, &transform);
	mat4_move(mesh->transform, transform);
}

RENDERER_API function_signature(mat4_t, mesh3d_transform_get, mesh3d_t* mesh)
{
	return mesh->transform;
}

RENDERER_API function_signature(void, mesh3d_make_centroid_origin, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh3d_has_positions(mesh), "!mesh3d_has_positions(mesh)\n");
	vec3_t centroid = vec3_zero();
	for(buf_ucount_t i = 0; i < mesh3d_positions_count(mesh); i++)
		centroid = vec3_add(2, centroid, mesh3d_position_get(mesh, i));
	centroid = vec3_scale(centroid, (float)1 / mesh3d_positions_count(mesh));
	for(buf_ucount_t i = 0; i < mesh3d_positions_count(mesh); i++)
		mesh3d_position_set_vec3(mesh, i, vec3_sub(2, mesh3d_position_get(mesh, i), centroid));
	CALLTRACE_END();
}

static void stl_vertex_normal(float* normal, mesh3d_t* mesh);
static void stl_vertex_position(float* position, mesh3d_t* mesh);
static void obj_vertex_normal(float* normal, void* user_data);
static void obj_vertex_position(float* position, void* user_data);
static void obj_vertex_texcoord(float* texcoord, void* user_data);
static void obj_facet(u32* facet, u32 attribute_count, u32 face_vertex_count, void* user_data);

RENDERER_API function_signature(mesh3d_t*, mesh3d_load, const char* file_path)
{
	CALLTRACE_BEGIN();
	const char* extension = strrchr(file_path, '.');
	mesh3d_t* mesh = mesh3d_new();
	BUFFER* buffer = NULL;
	if(!strcmp(extension, EXTENSION_STL))
	{
		mesh3d_positions_new(mesh, 0);
		mesh3d_normals_new(mesh, 0);
		mesh3d_colors_new(mesh, 0);
		mesh3d_triangles_new(mesh, 0);
		stl_parse_callbacks_t parse_callbacks =
		{
			.user_data = mesh,
			.vertex_normal_callback = (void*)stl_vertex_normal,
			.vertex_position_callback = (void*)stl_vertex_position
		};

		//DETERMINE WHETHER THE FILE IS ASCII TEXT OR BINARY
		//Assume file is ascii text
		FILE* file = fopen(file_path, "r");
		//skip any whitespaces
		char ch = 0;
		do
		{
			ch = getc(file);
		} while((strchr(" \t", ch) != NULL) && (!feof(file)));
		if(ftell(file) > 0) fseek(file, -1, SEEK_CUR);
		//Read the header assuming text file
		u8 len = strlen(STL_ASCII_HEADER);
		char temp[len + 1]; temp[len] = 0;
		u8 count = 0;
		while((!feof(file)) && (count < len))
		{
			temp[count] = getc(file);
			++count;
		}
		ASSERT(count == len, "File \"%s\" is too short to parse\n", file_path);
		rewind(file); fclose(file);

		//This may be true in binary case also, but that would mean the binary file is corrupted or someone has tried to fake it as ASCII
		if(!strcmp(temp, STL_ASCII_HEADER))
		{
			//parse the file as ascii
			buffer = load_text_from_file(file_path);
			ASSERT(buf_get_element_count(buffer) > STL_MINIMUM_ASCII_FILE_LENGTH, "length of the file \"%s\" is less than STL_MINIMUM_ASCII_FILE_LENGTH\n", file_path);
			stl_parse_ascii(buffer->bytes, buffer->element_count, &parse_callbacks);
		}
		else
		{
			//parse the file as binary
			buffer = load_binary_from_file(file_path);
			ASSERT(buf_get_element_count(buffer) > STL_MINIMUM_BINARY_FILE_LENGTH, "length of the file \"%s\" is less than STL_MINIMUM_BINARY_FILE_LENGTH\n", file_path);
			stl_parse_binary(buffer->bytes, buffer->element_count, &parse_callbacks);
		}
	}
	//TODO: Obj implementation is incomplete because obj_parse_ascii is not generic; can't load all the obj files
	else if(!strcmp(extension, EXTENSION_OBJ))
	{
		buffer = load_text_from_file(file_path);
		ASSERT(buf_get_element_count(buffer) > OBJ_MINIMUM_ASCII_FILE_LENGTH, "length of the file \"%s\" is less than OBJ_MINIMUM_ASCII_FILE_LENGTH\n", file_path);
		mesh3d_positions_new(mesh, 0);
		mesh3d_triangles_new(mesh, 0);
		mesh3d_uvs_new(mesh, 0);
		mesh3d_normals_new(mesh, 0);
		struct 
		{ 
			mesh3d_t* mesh;
			BUFFER normal_buffer; 		// temporary buffer to store normals
			BUFFER position_buffer; 	// temporary buffer to store positions
			BUFFER texcoord_buffer;		// temporary buffer to store texture coordinates
		} user_data;
		user_data.mesh = mesh;
		user_data.normal_buffer = buf_create(sizeof(float) * 3, 0, 0);
		user_data.position_buffer = buf_create(sizeof(float) * 3, 0, 0);
		user_data.texcoord_buffer = buf_create(sizeof(float) * 2, 0, 0);
		obj_parse_callbacks_t parse_callbacks =
		{
			.user_data = &user_data,
			.vertex_normal_callback = (void*)obj_vertex_normal,
			.vertex_position_callback = (void*)obj_vertex_position,
			.vertex_texcoord_callback = (void*)obj_vertex_texcoord,
			.facet_callback = (void*)obj_facet
		};
		obj_parse_ascii(buffer->bytes, buf_get_element_count(buffer), &parse_callbacks);
		buf_free(&user_data.normal_buffer);
		buf_free(&user_data.position_buffer);
		buf_free(&user_data.texcoord_buffer);
		if(!mesh3d_has_uvs(mesh))
			mesh3d_uvs_free(mesh);
		if(!mesh3d_has_normals(mesh))
			mesh3d_normals_free(mesh);
	}
	else
		LOG_FETAL_ERR("Failed to open \"%s\", Unsupported file format\n", file_path);
	if(buffer != NULL)
		buf_free(buffer);
	mesh3d_optimize_buffer(mesh);
	CALLTRACE_RETURN(mesh);
}

static void stl_vertex_normal(float* normal, mesh3d_t* mesh)
{
	mesh3d_normal_add(mesh, normal[1], normal[2], normal[0]);
	mesh3d_normal_add(mesh, normal[1], normal[2], normal[0]);
	mesh3d_normal_add(mesh, normal[1], normal[2], normal[0]);
	index_t count = mesh3d_triangles_count(mesh) * 3;
	index_t vertex_count = mesh3d_positions_count(mesh) + 3;
	mesh3d_triangle_add(mesh, (count + 2) % vertex_count, count + 1, count + 0);
}

static void stl_vertex_position(float* position, mesh3d_t* mesh)
{
	vec3_t normal = mesh3d_normal_get(mesh, mesh3d_positions_count(mesh));
	mesh3d_position_add(mesh, position[1], position[2], position[0]);
	normal.x = (normal.x < 0) ? 1 : normal.x;
	normal.y = (normal.y < 0) ? 1 : normal.y;
	normal.z = (normal.z < 0) ? 1 : normal.z;
	mesh3d_color_add_vec3(mesh, normal);
}

#ifdef GLOBAL_DEBUG
static void print_normal(float* normal, void* ptr)
{
	printf("NORMAL -> (%f, %f, %f)\n", normal[0], normal[1], normal[2]);
}

static void print_position(float* position, void* ptr)
{
	printf("POSITION -> (%f, %f, %f)\n", position[0], position[1], position[2]);
}

static void print_texcoord(float* texcoord, void* ptr)
{
	printf("TEXCOORD -> (%f, %f)\n", texcoord[0], texcoord[1]);
}

static void print_facet(u32* facet, u32 attrib_count, u32 face_vertex_count, void* ptr)
{
	printf("FACET -> ");
	for(int i = 0; i < face_vertex_count; i++)
	{
		printf("(");
		for(int j = 0; j < attrib_count; j++)
			printf(" %u", facet[i * attrib_count + j]);
		printf(" ) ");
	}
	puts("");
}
#endif /* GLOBAL_DEBUG */

static void obj_vertex_normal(float* normal, void* user_data)
{
	// print_normal(normal, NULL);
	buf_push(&((BUFFER*)(user_data + sizeof(mesh3d_t*)))[0], normal);
}

static void obj_vertex_position(float* position, void* user_data)
{
	// print_position(position, NULL);
	buf_push(&((BUFFER*)(user_data + sizeof(mesh3d_t*)))[1], position);
}

static void obj_vertex_texcoord(float* texcoord, void* user_data)
{
	// print_texcoord(texcoord, NULL);
	buf_push(&((BUFFER*)(user_data + sizeof(mesh3d_t*)))[2], texcoord);
}

// TODO: Currently this creates duplicated vertices for smoothed polygon meshes, so create another version to load smoothed meshes (per vertex normals)
static void obj_facet(u32* facet, u32 attribute_count, u32 face_vertex_count, void* user_data)
{
	// print_facet(facet, attribute_count, face_vertex_count, NULL);
	mesh3d_t* mesh = *(mesh3d_t**)user_data;
	BUFFER* normal_buffer = user_data + sizeof(mesh3d_t*);
	BUFFER* position_buffer = normal_buffer + 1;
	BUFFER* texcoord_buffer = normal_buffer + 2;
	bool has_uvs = buf_get_element_count(texcoord_buffer) > 0;
	bool has_normals = buf_get_element_count(normal_buffer) > 0;

	u32 attrib_ptr = 0;
	buf_ucount_t position_count = buf_get_element_count(mesh->positions);
	buf_push(mesh->positions, buf_get_ptr_at(position_buffer, facet[attribute_count * 0 + attrib_ptr] - 1));
	buf_push(mesh->positions, buf_get_ptr_at(position_buffer, facet[attribute_count * 1 + attrib_ptr] - 1));
	buf_push(mesh->positions, buf_get_ptr_at(position_buffer, facet[attribute_count * 2 + attrib_ptr] - 1));
	++attrib_ptr;
	if(has_uvs)
	{
		buf_push(mesh->uvs, buf_get_ptr_at(texcoord_buffer, facet[attribute_count * 0 + attrib_ptr] - 1));
		buf_push(mesh->uvs, buf_get_ptr_at(texcoord_buffer, facet[attribute_count * 1 + attrib_ptr] - 1));
		buf_push(mesh->uvs, buf_get_ptr_at(texcoord_buffer, facet[attribute_count * 2 + attrib_ptr] - 1));
		++attrib_ptr;
	}
	if(has_normals)
	{
		buf_push(mesh->normals, buf_get_ptr_at(normal_buffer, facet[attribute_count * 0 + attrib_ptr] - 1));
		buf_push(mesh->normals, buf_get_ptr_at(normal_buffer, facet[attribute_count * 1 + attrib_ptr] - 1));
		buf_push(mesh->normals, buf_get_ptr_at(normal_buffer, facet[attribute_count * 2 + attrib_ptr] - 1));
	}
	vec3uint_t triangle;
	triangle.z = position_count + 0;
	triangle.y = position_count + 1;
	triangle.x = position_count + 2;
	buf_push(mesh->triangles, &triangle);
	if(face_vertex_count == 4)
	{
		attrib_ptr = 0;
		buf_push(mesh->positions, buf_get_ptr_at(position_buffer, facet[attribute_count * 3 + attrib_ptr] - 1));
		++attrib_ptr;
		if(has_uvs)
		{
			buf_push(mesh->uvs, buf_get_ptr_at(texcoord_buffer, facet[attribute_count * 3 + attrib_ptr] - 1));
			++attrib_ptr;
		}
		if(has_normals)
		{
			buf_push(mesh->normals, buf_get_ptr_at(normal_buffer, facet[attribute_count * 3 + attrib_ptr] - 1));
			++attrib_ptr;
		}
		triangle.z = position_count + 2;
		triangle.y = position_count + 3;
		triangle.x = position_count + 0;
		buf_push(mesh->triangles, &triangle);
	}
}

#define getter(...) define_alias_function_macro(getter, __VA_ARGS__)
static function_signature(void, mesh3d_foreach, mesh3d_t* mesh, BUFFER* buffer, func_ptr_sig(void*, getter, mesh3d_t*, index_t), void (*visitor)(void*, void*), void* user_data)
{
	CALLTRACE_BEGIN();
	ASSERT(visitor != NULL, "visitor == NULL\n");
	ASSERT(func_ptr(getter) != NULL, "getter == NULL\n");
	buf_ucount_t count = buf_get_element_count(buffer);
	for(buf_ucount_t i = 0; i < count; i++)
		visitor(getter(mesh, i), user_data);
	CALLTRACE_END();
}

/*[CurrentlyNotUsed]*/
static void direction_visitor(vec3_t* v, mat4_t* m)
{
	//TODO: This should be like:
	//		*v = mat4_mul_vec4(*m, v->x, v->y, v->z, 0).xyz;
	vec4_t _v = mat4_mul_vec4(*m, v->x, v->y, v->z, 0);
	v->x = _v.x;
	v->y = _v.y;
	v->z = _v.z;
}

static void position_visitor(vec3_t* v, mat4_t* m)
{
	//TODO: This should be like:
	//		*v = mat4_mul_vec4(*m, v->x, v->y, v->z, 0).xyz;
	vec4_t _v = mat4_mul_vec4(*m, v->x, v->y, v->z, 1.0f);
	v->x = _v.x;
	v->y = _v.y;
	v->z = _v.z;
}


/*TODO:
New Feature and Performance improvement request, must be implemented in BUFFERlib version 1.2
	1. contiguous and same type of data memory blocks defragmentation
	2. support for multiple value pushes in one function call, better utilization of cache memory
*/
