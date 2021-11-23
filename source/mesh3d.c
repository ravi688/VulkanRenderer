
#include <renderer/mesh3d.h>
#include <renderer/defines.h>
#include <renderer/assert.h>
#include <memory.h>
#include <stdlib.h>

#define ensure_mesh3d_is_binded() ASSERT(binded_mesh3d != NULL, "binded_mesh3d == NULL , make sure to bind a mesh3d_t first by calling mesh3d_bind()\n")
#define __UVS_ARE_NOT_BOUND__ "mesh3d_t::uvs are not bound, make sure to call mesh3d_uvs_bind() first\n"
#define __NORMALS_ARE_NOT_BOUND__ "mesh3d_t::normals are not bound, make sure to call mesh3d_normals_bind() first\n"
#define __POSITIONS_ARE_NOT_BOUND__ "mesh3d_t::positions are not bound, make sure to call mesh3d_positions_bind() first\n"
#define __TRIANGLES_ARE_NOT_BOUND__ "mesh3d_t::triangles are not bound, make sure to call mesh3d_triangles_bind() first\n"

static mesh3d_t* binded_mesh3d;
static mesh3d_t* temp_binded_mesh3d;

#ifdef MESH3d_REQUIRE_PRINT
function_signature_void(void, mesh3d_print)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	log_msg("[Positions]");
	mesh3d_positions_bind();
	print_array(LIST_GET_DATA(vec3_t(float)), sizeof(vec3_t(float)), LIST_GET_COUNT(), vec3float_print);

	log_msg("[Triangles]"); 
	mesh3d_triangles_bind(); 
	print_array(LIST_GET_DATA(vec3_t(int)), sizeof(vec3_t(int)), LIST_GET_COUNT(), vec3int_print);

	log_msg("[Normals]"); 
	mesh3d_normals_bind(); 
	print_array(LIST_GET_DATA(vec3_t(float)), sizeof(vec3_t(float)), LIST_GET_COUNT(), vec3float_print);

	log_msg("[UVs]"); 
	mesh3d_uvs_bind(); 
	print_array(LIST_GET_DATA(vec2_t(float)), sizeof(vec2_t(float)), LIST_GET_COUNT(), vec2float_print);
	CALLTRACE_END();
}
#endif

function_signature_void(void, mesh3d_push_binded)
{
	CALLTRACE_BEGIN();
	temp_binded_mesh3d = binded_mesh3d;
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_pop_binded)
{
	CALLTRACE_BEGIN();
	binded_mesh3d = temp_binded_mesh3d;
	CALLTRACE_END();
}

function_signature(void, mesh3d_bind, mesh3d_t* mesh3d)
{
	CALLTRACE_BEGIN();
	binded_mesh3d = mesh3d;
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_unbind)
{
	CALLTRACE_BEGIN();
	binded_mesh3d = NULL;
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_unbind_buffers)
{
	CALLTRACE_BEGIN();
	LIST_UNBIND();
	CALLTRACE_END();
}

function_signature_void(mesh3d_t*, mesh3d_get_binded)
{
	CALLTRACE_BEGIN();
	CALLTRACE_RETURN(binded_mesh3d);
}

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

function_signature(void, mesh3d_positions_new, u32 count)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh3d->positions != NULL)
	{	
		LIST_BIND(binded_mesh3d->positions);
		LIST_DESTROY();
		binded_mesh3d->positions = NULL;
	}
	binded_mesh3d->positions = LIST_NEW(vec3_t(float), count); 
	LIST_BIND(_list);
	#ifdef MESH3D_DEBUG
	log_msg("New Position buffer created, length: %d", count);
	#endif
	CALLTRACE_END();
}

function_signature(void, mesh3d_normals_new, u32 count)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh3d->normals != NULL)
	{
		LIST_BIND(binded_mesh3d->normals); 
		LIST_DESTROY();
		binded_mesh3d->normals = NULL;
	}
	binded_mesh3d->normals = LIST_NEW(vec3_t(float), count); 
	LIST_BIND(_list);
	#ifdef MESH3D_DEBUG
	log_msg("New Normals buffer created, length: %d", count);
	#endif
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangles_new, u32 count)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh3d->triangles != NULL)
	{
		LIST_BIND(binded_mesh3d->triangles); 
		LIST_DESTROY();
		binded_mesh3d->triangles = NULL;
	}
	binded_mesh3d->triangles = LIST_NEW(vec3_t(int), count); 
	LIST_BIND(_list);
	#ifdef MESH3D_DEBUG
	log_msg("New Triangle buffer created, length: %d", count);
	#endif	
	CALLTRACE_END();
}

function_signature(void, mesh3d_uvs_new, u32 count)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh3d->uvs != NULL)
	{
		LIST_BIND(binded_mesh3d->uvs); 
		LIST_DESTROY();
		binded_mesh3d->uvs = NULL;
	}
	binded_mesh3d->uvs = LIST_NEW(vec2_t(float), count);
	LIST_BIND(_list);
	#ifdef MESH3D_DEBUG
	log_msg("New UV buffer created, length: %d\n", count);
	#endif	
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_destroy)
{	
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	LIST() _list = LIST_GET_BINDED();
	if(binded_mesh3d->positions != NULL)
	{
		LIST_BIND(binded_mesh3d->positions);
		LIST_DESTROY(); 
	}

	if(binded_mesh3d->uvs != NULL)
	{
		LIST_BIND(binded_mesh3d->uvs);	
		LIST_DESTROY();
	}
	
	if(binded_mesh3d->normals != NULL)
	{
		LIST_BIND(binded_mesh3d->normals); 
		LIST_DESTROY();
	}
	
	if(binded_mesh3d->triangles != NULL)
	{
		LIST_BIND(binded_mesh3d->triangles); 
		LIST_DESTROY();
	}
	
	LIST_BIND(_list);
	#ifdef MESH3D_DEBUG
	log_msg("mesh3d_t is destroyed successfully\n");
	#endif
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_optimize_buffer)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	BUFFER* buf = BUFget_binded();
	if(binded_mesh3d->positions != NULL)
	{
		mesh3d_positions_bind();
		BUFfit();
	}
	if(binded_mesh3d->triangles != NULL)
	{
		mesh3d_triangles_bind();
		BUFfit();
	}
	if(binded_mesh3d->uvs != NULL)
	{
		mesh3d_uvs_bind();
		BUFfit();
	}
	if(binded_mesh3d->normals != NULL)
	{
		mesh3d_normals_bind();
		BUFfit();
	}
	BUFbind(buf);
	#ifdef MESH3D_DEBUG
	log_msg("mesh3d_t optimized successfully\n");
	#endif
	CALLTRACE_END();
}

function_signature_void(bool, mesh3d_has_normals)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	CALLTRACE_RETURN((binded_mesh3d->normals != NULL) && (buf_get_element_count(binded_mesh3d->normals) > 0));
}

function_signature_void(bool, mesh3d_has_positions)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	CALLTRACE_RETURN((binded_mesh3d->positions != NULL) && (buf_get_element_count(binded_mesh3d->positions) > 0));
}

function_signature_void(bool, mesh3d_has_uvs)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	CALLTRACE_RETURN((binded_mesh3d->uvs != NULL) && (buf_get_element_count(binded_mesh3d->uvs) > 0));
}

function_signature_void(bool, mesh3d_has_triangles)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	CALLTRACE_RETURN((binded_mesh3d->triangles != NULL) && (buf_get_element_count(binded_mesh3d->triangles) > 0));
}

function_signature_void(void, mesh3d_calculate_normals)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	log_fetal_err("Undefined method\n");
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_positions_bind)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(binded_mesh3d->positions != NULL, "positions == NULL, you should first call mesh3d_uvs_positions() to create new buffer\n");
	LIST_BIND(binded_mesh3d->positions);
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_normals_bind)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(binded_mesh3d->normals != NULL, "normals == NULL, you should first call mesh3d_uvs_normals() to create new buffer\n");
	LIST_BIND(binded_mesh3d->normals);
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_uvs_bind)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(binded_mesh3d->uvs != NULL, "uvs == NULL, you should first call mesh3d_uvs_new() to create new buffer\n");
	LIST_BIND(binded_mesh3d->uvs);
	CALLTRACE_END();
}

function_signature_void(void, mesh3d_triangles_bind)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(binded_mesh3d->triangles != NULL, "triangles == NULL, you should first call mesh3d_uvs_triangles() to create new buffer\n");
	LIST_BIND(binded_mesh3d->triangles);
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_add, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	vec3_t(float) v = {x, y, z};
	LIST_ADD(&v);
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_add_vec3, vec3_t(float)  v)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	LIST_ADD((void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	vec3_t(float)* v = LIST_GET_PTR(vec3_t(float), index);
	v->x = x; v->y = y; v->z = z;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_vec3, index_t index, vec3_t(float)  position)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	LIST_SET(index, (void*)(&position));
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_x, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_y, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_position_set_z, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->z = z;
	CALLTRACE_END();
}

function_signature(vec3_t(float),  mesh3d_position_get, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(*LIST_GET_PTR(vec3_t(float), index));
}

function_signature(vec3_t(float)*, mesh3d_position_get_ptr, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index));
}

function_signature(float, mesh3d_position_get_x, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->x);
}

function_signature(float, mesh3d_position_get_y, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->y);
}

function_signature(float, mesh3d_position_get_z, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->positions, __POSITIONS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->z);
}

function_signature(void, mesh3d_normal_add, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	vec3_t(float) n = {x, y, z};
	LIST_ADD(&n);
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_add_vec3, vec3_t(float)  v)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	LIST_ADD((void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set, index_t index, float x, float y, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	vec3_t(float)* n = LIST_GET_PTR(vec3_t(float), index);
	n->x = x; n->y = y; n->z = z;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_vec3, index_t index, vec3_t(float)  normal)
{
	CALLTRACE_BEGIN();
 	ensure_mesh3d_is_binded();
 	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
 	LIST_SET(index, (void*)(&normal));
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_x, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_y, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_normal_set_z, index_t index, float z)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(float), index)->z = z;
	CALLTRACE_END();
}

function_signature(vec3_t(float), mesh3d_normal_get, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(*LIST_GET_PTR(vec3_t(float), index));
}

function_signature(vec3_t(float)*, mesh3d_normal_get_ptr, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index));
}

function_signature(float, mesh3d_normal_get_x, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->x);
}

function_signature(float, mesh3d_normal_get_y, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->y);
}

function_signature(float, mesh3d_normal_get_z, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->normals, __NORMALS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(float), index)->z);
}

function_signature(void, mesh3d_triangle_add, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	vec3_t(int) i = {i0, i1, i2};
	LIST_ADD(&i);
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_add_vec3, vec3_t(int)  v)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	LIST_ADD((void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set, index_t index, index_t i0, index_t i1, index_t i2)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	vec3_t(int)* t =  LIST_GET_PTR(vec3_t(int), index);
	t->x = i0; t->y = i1; t->z = i2;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_vec3, index_t index, vec3_t(int)  v)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	LIST_SET(index, (void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_0, index_t index, index_t i0)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(int), index)->x = i0;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_1, index_t index, index_t i1)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(int), index)->x = i1;
	CALLTRACE_END();
}

function_signature(void, mesh3d_triangle_set_2, index_t index, index_t i2)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec3_t(int), index)->x = i2;
	CALLTRACE_END();
}

function_signature(vec3_t(int),  mesh3d_triangle_get, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(*LIST_GET_PTR(vec3_t(int), index));
}

function_signature(vec3_t(int)*, mesh3d_triangle_get_ptr, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(int), index));
}

function_signature(int, mesh3d_triangle_get_0, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(int), index)->x);
}

function_signature(int, mesh3d_triangle_get_1, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(int), index)->y);
}

function_signature(int, mesh3d_triangle_get_2, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->triangles, __TRIANGLES_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec3_t(int), index)->z);
}

function_signature(void, mesh3d_uv_add, float x, float y)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	vec2_t(float) uv = {x, y};
	LIST_ADD(&uv);
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_add_vec2, vec2_t(float) v)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	LIST_ADD((void*)(&v));
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set, index_t index, float x, float y)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	vec2_t(float)* v = LIST_GET_PTR(vec2_t(float), index);
	v->x = x; v->y = y;
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_vec2, index_t index, vec2_t(float)  uv)
{
	CALLTRACE_BEGIN();
 	ensure_mesh3d_is_binded();
 	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
 	LIST_SET(index, (void*)(&uv));
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_x, index_t index, float x)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec2_t(float), index)->x = x;
	CALLTRACE_END();
}

function_signature(void, mesh3d_uv_set_y, index_t index, float y)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	LIST_GET_PTR(vec2_t(float), index)->y = y;
	CALLTRACE_END();
}

function_signature(vec2_t(float),  mesh3d_uv_get, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(*LIST_GET_PTR(vec2_t(float), index));
}

function_signature(vec2_t(float)*, mesh3d_uv_get_ptr, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec2_t(float), index));
}

function_signature(float, mesh3d_uv_get_y, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec2_t(float), index)->y);
}

function_signature(float, mesh3d_uv_get_x, index_t index)
{
	CALLTRACE_BEGIN();
	ensure_mesh3d_is_binded();
	ASSERT(BUFget_binded() == binded_mesh3d->uvs, __UVS_ARE_NOT_BOUND__);
	CALLTRACE_RETURN(LIST_GET_PTR(vec2_t(float), index)->x);
}

/*TODO:
New Feature and Performance improvement request, must be implemented in BUFFERlib version 1.2
	1. contiguous and same type of data memory blocks defragmentation
	2. support for multiple value pushes in one function call, better utilization of cache memory
*/
