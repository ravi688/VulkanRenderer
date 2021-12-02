
#include <renderer/mesh3d.h>
#include <renderer/defines.h>
#include <renderer/assert.h>
#include <memory.h>
#include <stdlib.h>

#define __UVS_ARE_NOT_FOUND__ "mesh3d_t::uvs are not found, make sure to call mesh3d_uvs_new(count) first\n"
#define __NORMALS_ARE_NOT_FOUND__ "mesh3d_t::normals are not found, make sure to call mesh3d_normals_new(count) first\n"
#define __COLORS_ARE_NOT_FOUND__ "mesh3d_t::colors are not found, make sure to call mesh3d_colors_new(count) first\n"
#define __POSITIONS_ARE_NOT_FOUND__ "mesh3d_t::positions are not found, make sure to call mesh3d_positions_new(count) first\n"
#define __TRIANGLES_ARE_NOT_FOUND__ "mesh3d_t::triangles are not found, make sure to call mesh3d_triangles_new(count) first\n"


#ifdef MESH3d_REQUIRE_PRINT
function_signature(void, mesh3d_print, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->positions != NULL)
	{
		log_msg("[Positions]");
		print_array(buf_get_ptr(mesh->positions), sizeof(vec3_t(float)), buf_get_element_count(mesh->positions), vec3float_print);
	}

	if(mesh->triangles != NULL)
	{
		log_msg("[Triangles]");
		print_array(buf_get_ptr(mesh->triangles), sizeof(vec3_t(int)), buf_get_element_count(mesh->triangles), vec3int_print);
	}

	if(mesh->normals != NULL)
	{
		log_msg("[Normals]");
		print_array(buf_get_ptr(mesh->normals), sizeof(vec3_t(float)), buf_get_element_count(mesh->normals), vec3float_print);
	}

	if(mesh->colors != NULL)
	{
		log_msg("[Colors]");
		print_array(buf_get_ptr(mesh->colors), sizeof(vec3_t(float)), buf_get_element_count(mesh->colors), vec3float_print);
	}

	if(mesh->uvs != NULL)
	{
		log_msg("[UVs]");
		print_array(buf_get_ptr(mesh->uvs), sizeof(vec2_t(float)), buf_get_element_count(mesh->uvs), vec2float_print);
	}
	CALLTRACE_END();
}
#endif

function_signature_void(mesh3d_t*, mesh3d_new)
{
	CALLTRACE_BEGIN();
	//TODO: Allocation must be done with heap_new(mesh3d_t)
	mesh3d_t* mesh3d  = (mesh3d_t*)malloc(sizeof(mesh3d_t)); 
	memset(mesh3d, 0, sizeof(mesh3d_t));
	#ifdef MESH3D_DEBUG
	log_msg("mesh3d_t created successfully"); 
	#endif
	CALLTRACE_RETURN(mesh3d);
}

function_signature(void, mesh3d_positions_new, mesh3d_t* mesh, u32 count)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->positions != NULL)
	{	
		buf_free(mesh->positions);
		mesh->positions = NULL;
	}
	mesh->positions = BUFcreate(NULL, sizeof(vec3_t(float)), count, 0);
	ASSERT(mesh->positions != NULL, "mesh->positions == NULL\n");
	#ifdef MESH3D_DEBUG
	log_msg("New Position buffer created, length: %d", count);
	#endif
	CALLTRACE_END();
}

function_signature(void, mesh3d_normals_new, mesh3d_t* mesh, u32 count)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->normals != NULL)
	{
		buf_free(mesh->normals);
		mesh->normals = NULL;
	}
	mesh->normals = BUFcreate(NULL, sizeof(vec3_t(float)), count, 0);
	#ifdef MESH3D_DEBUG
	log_msg("New Normals buffer created, length: %d", count);
	#endif
	CALLTRACE_END();
}

function_signature(void, mesh3d_colors_new, mesh3d_t* mesh, u32 count)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->colors != NULL)
	{
		buf_free(mesh->colors);
		mesh->colors = NULL;
	}
	mesh->colors = BUFcreate(NULL, sizeof(vec3_t(float)), count, 0);
	#ifdef MESH3D_DEBUG
	log_msg("New Colors buffer created, length: %d", count);
	#endif
	CALLTRACE_END();
}


function_signature(void, mesh3d_triangles_new, mesh3d_t* mesh, u32 count)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->triangles != NULL)
	{
		buf_free(mesh->triangles);
		mesh->triangles = NULL;
	}
	mesh->triangles = BUFcreate(NULL, sizeof(vec3_t(int)), count, 0);
	#ifdef MESH3D_DEBUG
	log_msg("New Triangle buffer created, length: %d", count);
	#endif	
	CALLTRACE_END();
}

function_signature(void, mesh3d_uvs_new, mesh3d_t* mesh, u32 count)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	if(mesh->uvs != NULL)
	{
		buf_free(mesh->uvs);
		mesh->uvs = NULL;
	}
	mesh->uvs = BUFcreate(NULL, sizeof(vec2_t(float)), count, 0);
	#ifdef MESH3D_DEBUG
	log_msg("New UV buffer created, length: %d\n", count);
	#endif	
	CALLTRACE_END();
}

function_signature(u32, mesh3d_positions_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(mesh->positions != NULL, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->positions));
}

function_signature(u32, mesh3d_normals_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(mesh->normals != NULL, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->normals));
}

function_signature(u32, mesh3d_uvs_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(mesh->uvs != NULL, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->uvs));
}

function_signature(u32, mesh3d_colors_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(mesh->colors != NULL, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->colors));
}
function_signature(u32, mesh3d_triangles_count, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(mesh->triangles != NULL, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_element_count(mesh->triangles));
}

function_signature(void, mesh3d_destroy, mesh3d_t* mesh)
{	
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");

	if(mesh->positions != NULL)
		buf_free(mesh->positions);

	if(mesh->normals != NULL)
		buf_free(mesh->normals);

	if(mesh->colors != NULL)
		buf_free(mesh->colors);

	if(mesh->uvs != NULL)
		buf_free(mesh->uvs);

	if(mesh->positions != NULL)
		buf_free(mesh->triangles);

	#ifdef MESH3D_DEBUG
	log_msg("mesh3d_t is destroyed successfully\n");
	#endif
	CALLTRACE_END();
}

function_signature(void, mesh3d_optimize_buffer, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
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

	#ifdef MESH3D_DEBUG
	log_msg("mesh3d_t optimized successfully\n");
	#endif
	CALLTRACE_END();
}

function_signature(bool, mesh3d_has_normals, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	CALLTRACE_RETURN((mesh->normals != NULL) && (buf_get_element_count(mesh->normals) > 0));
}


function_signature(bool, mesh3d_has_colors, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	CALLTRACE_RETURN((mesh->colors != NULL) && (buf_get_element_count(mesh->colors) > 0));
}


function_signature(bool, mesh3d_has_positions, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	CALLTRACE_RETURN((mesh->positions != NULL) && (buf_get_element_count(mesh->positions) > 0));
}

function_signature(bool, mesh3d_has_uvs, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	CALLTRACE_RETURN((mesh->uvs != NULL) && (buf_get_element_count(mesh->uvs) > 0));
}

function_signature(bool, mesh3d_has_triangles, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	CALLTRACE_RETURN((mesh->triangles != NULL) && (buf_get_element_count(mesh->triangles) > 0));
}

function_signature(void, mesh3d_calculate_normals, mesh3d_t* mesh)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	log_fetal_err("Undefined method\n");
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	vec3_t(float) v = {x, y, z};
	buf_push(mesh->positions, &v);
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_add_vec3, mesh3d_t* mesh, vec3_t(float)  v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_push(mesh->positions, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	vec3_t(float)* v = buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index);
	v->x = x; v->y = y; v->z = z;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float)  position)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_set_at(mesh->positions, index, (void*)(&position));
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->z = z;
	CALLTRACE_END();
}

function_signature(vec3_t(float),  mesh3d_position_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index));
}

function_signature(vec3_t(float)*, mesh3d_position_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index));
}

function_signature(float, mesh3d_position_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->x);
}

function_signature(float, mesh3d_position_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->y);
}

function_signature(float, mesh3d_position_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->positions, __POSITIONS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->positions, vec3_t(float), index)->z);
}

function_signature(void, mesh3d_normal_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	vec3_t(float) n = {x, y, z};
	buf_push(mesh->normals, &n);
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_add_vec3, mesh3d_t* mesh, vec3_t(float)  v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_push(mesh->normals, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	vec3_t(float)* n = buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float)  normal)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
 	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->normals, index, (void*)(&normal));
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->z = z;
	CALLTRACE_END();
}

function_signature(vec3_t(float), mesh3d_normal_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index));
}

function_signature(vec3_t(float)*, mesh3d_normal_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index));
}

function_signature(float, mesh3d_normal_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->x);
}

function_signature(float, mesh3d_normal_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->y);
}

function_signature(float, mesh3d_normal_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->normals, __NORMALS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->normals, vec3_t(float), index)->z);
}


function_signature(void, mesh3d_color_add, mesh3d_t* mesh, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	vec3_t(float) n = {x, y, z};
	buf_push(mesh->colors, &n);
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_add_vec3, mesh3d_t* mesh, vec3_t(float)  v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_push(mesh->colors, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_set, mesh3d_t* mesh, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	vec3_t(float)* n = buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(float)  color)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
 	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->colors, index, (void*)(&color));
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_color_set_z, mesh3d_t* mesh, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->z = z;
	CALLTRACE_END();
}

function_signature(vec3_t(float), mesh3d_color_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index));
}

function_signature(vec3_t(float)*, mesh3d_color_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index));
}

function_signature(float, mesh3d_color_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->x);
}

function_signature(float, mesh3d_color_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->y);
}

function_signature(float, mesh3d_color_get_z, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->colors, __COLORS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->colors, vec3_t(float), index)->z);
}


function_signature(void, mesh3d_triangle_add, mesh3d_t* mesh, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	vec3_t(int) i = {i0, i1, i2};
	buf_push(mesh->triangles, &i);
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_add_vec3, mesh3d_t* mesh, vec3_t(int)  v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_push(mesh->triangles, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set, mesh3d_t* mesh, index_t index, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	vec3_t(int)* t =  buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index);
	t->x = i0; t->y = i1; t->z = i2;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_vec3, mesh3d_t* mesh, index_t index, vec3_t(int)  v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_set_at(mesh->triangles, index, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_0, mesh3d_t* mesh, index_t index, index_t i0)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->x = i0;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_1, mesh3d_t* mesh, index_t index, index_t i1)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->x = i1;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_2, mesh3d_t* mesh, index_t index, index_t i2)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->x = i2;
	CALLTRACE_END();
}

function_signature(vec3_t(int),  mesh3d_triangle_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index));
}

function_signature(vec3_t(int)*, mesh3d_triangle_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index));
}

function_signature(int, mesh3d_triangle_get_0, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->x);
}

function_signature(int, mesh3d_triangle_get_1, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->y);
}

function_signature(int, mesh3d_triangle_get_2, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->triangles, __TRIANGLES_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->triangles, vec3_t(int), index)->z);
}

function_signature(void, mesh3d_uv_add, mesh3d_t* mesh, float x, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	vec2_t(float) uv = {x, y};
	buf_push(mesh->uvs, &uv);
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_add_vec2, mesh3d_t* mesh, vec2_t(float) v)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_push(mesh->uvs, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set, mesh3d_t* mesh, index_t index, float x, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	vec2_t(float)* v = buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index);
	v->x = x; v->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_vec2, mesh3d_t* mesh, index_t index, vec2_t(float)  uv)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
 	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
 	buf_set_at(mesh->uvs, index, (void*)(&uv));
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_x, mesh3d_t* mesh, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_y, mesh3d_t* mesh, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(vec2_t(float),  mesh3d_uv_get, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(*buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index));
}

function_signature(vec2_t(float)*, mesh3d_uv_get_ptr, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index));
}

function_signature(float, mesh3d_uv_get_y, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index)->y);
}

function_signature(float, mesh3d_uv_get_x, mesh3d_t* mesh, index_t index)
{
	CALLTRACE_BEGIN();
	ASSERT(mesh != NULL, "mesh == NULL\n");
	ASSERT(NULL != mesh->uvs, __UVS_ARE_NOT_FOUND__);
	CALLTRACE_RETURN(buf_get_ptr_at_typeof(mesh->uvs, vec2_t(float), index)->x);
}

function_signature(mesh3d_t*, mesh3d_cube, float size)
{
	CALLTRACE_BEGIN();
	mesh3d_t* mesh = mesh3d_new();
	mesh3d_positions_new(mesh, 0);
	mesh3d_triangles_new(mesh, 0);
	mesh3d_colors_new(mesh, 0);
	mesh3d_normals_new(mesh, 0);

	float half_size = 0.5f * size;

	//Bottom
	mesh3d_position_add(mesh, -half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size );
	mesh3d_position_add(mesh, half_size, 0, half_size  );

	//Top
	mesh3d_position_add(mesh, half_size, size, half_size  );
	mesh3d_position_add(mesh, half_size, size, -half_size );
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, size, half_size );

	//Front
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, size, half_size );
	mesh3d_position_add(mesh, -half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, -half_size);

	//Left
	mesh3d_position_add(mesh, half_size, size, -half_size );
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size );

	//Right
	mesh3d_position_add(mesh, -half_size, size, half_size);
	mesh3d_position_add(mesh, half_size, size, half_size );
	mesh3d_position_add(mesh, half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, half_size);

	//Back
	mesh3d_position_add(mesh, half_size, size, half_size );
	mesh3d_position_add(mesh, half_size, size, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, half_size );


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

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, -1.0f, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 1.0f, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, 1.0f);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, -1.0f, 0, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 1.0f, 0, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, -1.0f);

	for(int i = 0; i < mesh3d_positions_count(mesh); i++)
		mesh3d_color_add(mesh, 1, 1, 1);

	mesh3d_optimize_buffer(mesh);
	CALLTRACE_RETURN(mesh);
}

/*TODO:
New Feature and Performance improvement request, must be implemented in BUFFERlib version 1.2
	1. contiguous and same type of data memory blocks defragmentation
	2. support for multiple value pushes in one function call, better utilization of cache memory
*/
