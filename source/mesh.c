
#include <engine/mesh.h>


 mesh_t* binded_mesh;
 mesh_t* temp_binded_mesh;

#ifdef MESH_REQUIRE_PRINT
 void mesh_print()
{
	ensure_mesh_is_binded();
	log_msg("[Positions]");
	mesh_positions_bind();
	print_array(LIST_GET_DATA(vec3_t(float)), sizeof(vec3_t(float)), LIST_GET_COUNT(), vec3float_print);

	log_msg("[Triangles]"); 
	mesh_triangles_bind(); 
	print_array(LIST_GET_DATA(vec3_t(int)), sizeof(vec3_t(int)), LIST_GET_COUNT(), vec3int_print);

	log_msg("[Normals]"); 
	mesh_normals_bind(); 
	print_array(LIST_GET_DATA(vec3_t(float)), sizeof(vec3_t(float)), LIST_GET_COUNT(), vec3float_print);

	log_msg("[UVs]"); 
	mesh_uvs_bind(); 
	print_array(LIST_GET_DATA(vec2_t(float)), sizeof(vec2_t(float)), LIST_GET_COUNT(), vec2float_print);
}
#endif

 void  mesh_push_binded()
{
	temp_binded_mesh = binded_mesh;
	return;
}
 void mesh_pop_binded()
{
	binded_mesh = temp_binded_mesh;
	return;
}

 void mesh_bind(mesh_t* mesh) { binded_mesh = mesh; } 
 void mesh_unbind() { binded_mesh = NULL; } 
 mesh_t* mesh_get_binded() { return binded_mesh; } 

 mesh_t* mesh_new()
{
	mesh_t* mesh  = (mesh_t*)malloc(sizeof(mesh_t)); 
	mesh->positions = LIST_NEW(vec3_t(float), 1);
	mesh->uvs = LIST_NEW(vec2_t(float), 1);
	mesh->normals = LIST_NEW(vec3_t(float), 1);
	mesh->triangles = LIST_NEW(vec3_t(int), 1);
	#ifdef MESH_DEBUG
	log_msg("mesh_t created successfully"); 
	#endif
	return mesh;
}
 void mesh_positions_new(uint32_t count)
{
	ensure_mesh_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh->positions != NULL)
	{	
		LIST_BIND(binded_mesh->positions);
		LIST_DESTROY();
	}
	binded_mesh->positions = LIST_NEW(vec3_t(float), count); 
	LIST_BIND(_list);
	#ifdef MESH_DEBUG
	log_msg("New Position buffer created, length: %d", count);
	#endif
}
 void mesh_normals_new(uint32_t count)
{
	ensure_mesh_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh->normals != NULL)
	{
		LIST_BIND(binded_mesh->normals); 
		LIST_DESTROY();
	}
	binded_mesh->normals = LIST_NEW(vec3_t(float), count); 
	LIST_BIND(_list);
	#ifdef MESH_DEBUG
	log_msg("New Normals buffer created, length: %d", count);
	#endif
}
 void mesh_triangles_new(uint32_t count)
{
	ensure_mesh_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh->triangles != NULL)
	{
		LIST_BIND(binded_mesh->triangles); 
		LIST_DESTROY();
	}
	binded_mesh->triangles = LIST_NEW(vec3_t(int), count); 
	LIST_BIND(_list);
	#ifdef MESH_DEBUG
	log_msg("New Triangle buffer created, length: %d", count);
	#endif	
}
 void mesh_uvs_new(uint32_t count)
{
	ensure_mesh_is_binded();
	LIST() _list = LIST_GET_BINDED(); 
	if(binded_mesh->uvs != NULL)
	{
		LIST_BIND(binded_mesh->uvs); 
		LIST_DESTROY();
	}
	binded_mesh->uvs = LIST_NEW(vec3_t(float), count); 
	LIST_BIND(_list);
	#ifdef MESH_DEBUG
	log_msg("New UV buffer created, length: %d", count);
	#endif	
}
 void mesh_destroy()
{	
	ensure_mesh_is_binded();
	LIST() _list = LIST_GET_BINDED();
	if(binded_mesh->positions != NULL)
	{
		LIST_BIND(binded_mesh->positions);
		LIST_DESTROY(); 
	}

	if(binded_mesh->uvs != NULL)
	{
		LIST_BIND(binded_mesh->uvs);	
		LIST_DESTROY();
	}
	
	if(binded_mesh->normals != NULL)
	{
		LIST_BIND(binded_mesh->normals); 
		LIST_DESTROY();
	}
	
	if(binded_mesh->triangles != NULL)
	{
		LIST_BIND(binded_mesh->triangles); 
		LIST_DESTROY();
	}
	
	LIST_BIND(_list);
	#ifdef MESH_DEBUG
	log_msg("mesh_t is destroyed successfully");
	#endif
}

 void mesh_optimize_buffer()
{
	ensure_mesh_is_binded();
	BUFFER* buf = BUFget_binded();
	if(binded_mesh->positions != NULL)
	{
		mesh_positions_bind();
		BUFfit();
	}
	if(binded_mesh->triangles != NULL)
	{
		mesh_triangles_bind();
		BUFfit();
	}
	if(binded_mesh->uvs != NULL)
	{
		mesh_uvs_bind();
		BUFfit();
	}
	if(binded_mesh->normals != NULL)
	{
		mesh_normals_bind();
		BUFfit();
	}
	BUFbind(buf);
	#ifdef MESH_DEBUG
	log_msg("mesh_t optimized successfully");
	#endif
}

 bool mesh_has_normals() { ensure_mesh_is_binded(false); mesh_normals_bind(); return (LIST_GET_COUNT() > 0); } 
 bool mesh_has_positions()  { ensure_mesh_is_binded(false); mesh_positions_bind(); return (LIST_GET_COUNT() > 0); } 
 bool mesh_has_uvs() { ensure_mesh_is_binded(false); mesh_uvs_bind(); return (LIST_GET_COUNT() > 0); } 
 bool mesh_has_triangles() { ensure_mesh_is_binded(false); mesh_triangles_bind(); return (LIST_GET_COUNT() > 0); } 

 void mesh_calculate_normals(void)
{
	ensure_mesh_is_binded();
	log_err("Undefined method, from mesh_calculate_normals(void)");
}

 void 		mesh_positions_bind(){ ensure_mesh_is_binded(); LIST_BIND(binded_mesh->positions); }
 void 		mesh_normals_bind(){ ensure_mesh_is_binded(); LIST_BIND(binded_mesh->normals); }
 void 		mesh_uvs_bind(){ ensure_mesh_is_binded(); LIST_BIND(binded_mesh->uvs); }
 void 		mesh_triangles_bind(){ ensure_mesh_is_binded(); LIST_BIND(binded_mesh->triangles); }
 void 		mesh_positions_add(float x, float y, float z) { ensure_mesh_is_binded(); vec3_t(float) v = {x, y, z}; LIST_ADD(&v); }
 void 		mesh_positions_add_vec3(vec3_t(float)  v) { ensure_mesh_is_binded(); LIST_ADD((void*)(&v)); }

 void 		mesh_positions_set_at(index_t index, float x, float y, float z) { ensure_mesh_is_binded(); vec3_t(float)* v = LIST_GET_PTR(vec3_t(float), (index)); v->x = (x); v->y = (y); v->z = (z); }
 void 		mesh_positions_set_at_vec3(index_t index, vec3_t(float)  position){ ensure_mesh_is_binded(); LIST_SET((index), (void*)(&position)); }
 void 		mesh_positions_set_at_x(index_t index, float x) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(float), (index))->x = (x); }
 void 		mesh_positions_set_at_y(index_t index, float y) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(float), (index))->y = (y); }
 void 		mesh_positions_set_at_z(index_t index, float z) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(float), (index))->z = (z); }

 vec3_t(float) mesh_positions_get_at(index_t index){ vec3_t(float) v; ensure_mesh_is_binded(v); return (*LIST_GET_PTR(vec3_t(float), (index))); }
 vec3_t(float)*mesh_positions_get_at_ptr(index_t index) { ensure_mesh_is_binded(NULL); return LIST_GET_PTR(vec3_t(float)	, (index)); }
 float 		mesh_positions_get_at_x(index_t index) { ensure_mesh_is_binded(0.0f); return (LIST_GET_PTR(vec3_t(float), (index))->x); }
 float 		mesh_positions_get_at_y(index_t index) { ensure_mesh_is_binded(0.0f); return (LIST_GET_PTR(vec3_t(float), (index))->y); }
 float 		mesh_positions_get_at_z(index_t index) { ensure_mesh_is_binded(0.0f); return (LIST_GET_PTR(vec3_t(float), (index))->z); }



 void 		mesh_normals_add(float x, float y, float z) { ensure_mesh_is_binded(); vec3_t(float) n = {x, y, z}; LIST_ADD(&n); }
 void 		mesh_normals_add_vec3(vec3_t(float)  v) {ensure_mesh_is_binded(); LIST_ADD((void*)(&v)); }

 void 		mesh_normals_set_at(index_t index, float x, float y, float z) { ensure_mesh_is_binded(); vec3_t(float)* n = LIST_GET_PTR(vec3_t(float), (index)); n->x = (x); n->y = (y); n->z = (z); }
 void 		mesh_normals_set_at_vec3(index_t index, vec3_t(float)  normal){ ensure_mesh_is_binded(); LIST_SET((index), (void*)(&normal)); }
 void 		mesh_normals_set_at_x(index_t index, float x) { ensure_mesh_is_binded();LIST_GET_PTR(vec3_t(float), (index))->x = (x); }
 void 		mesh_normals_set_at_y(index_t index, float y) { ensure_mesh_is_binded();LIST_GET_PTR(vec3_t(float), (index))->y = (y); } 
 void 		mesh_normals_set_at_z(index_t index, float z) { ensure_mesh_is_binded();LIST_GET_PTR(vec3_t(float), (index))->z = (z); }

 vec3_t(float) mesh_normals_get_at(index_t index) {vec3_t(float) v; ensure_mesh_is_binded(v); return (*LIST_GET_PTR(vec3_t(float), (index))); }
 vec3_t(float)*mesh_normals_get_at_ptr(index_t index) {ensure_mesh_is_binded(NULL); return LIST_GET_PTR(vec3_t(float), (index)); }
 float 		mesh_normals_get_at_x(index_t index) { ensure_mesh_is_binded(0.0f);return (LIST_GET_PTR(vec3_t(float), (index))->x); }
 float 		mesh_normals_get_at_y(index_t index) { ensure_mesh_is_binded(0.0f);return (LIST_GET_PTR(vec3_t(float), (index))->y); }
 float 		mesh_normals_get_at_z(index_t index) { ensure_mesh_is_binded(0.0f);return (LIST_GET_PTR(vec3_t(float), (index))->z); }



 void 		mesh_triangles_add(index_t i0, index_t i1, index_t i2) { ensure_mesh_is_binded();vec3_t(int) i = {i0, i1, i2}; LIST_ADD(&i); }
 void 		mesh_triangles_add_vec3(vec3_t(int)  v) { ensure_mesh_is_binded();LIST_ADD((void*)(&v)); }

 void		mesh_triangles_set_at(index_t index, index_t i0, index_t i1, index_t i2) { ensure_mesh_is_binded();vec3_t(int)* t =  LIST_GET_PTR(vec3_t(int), (index)); t->x = i0; t->y = i1; t->z = i2; }
 void 		mesh_triangles_set_at_vec3(index_t index, vec3_t(int)  v) {ensure_mesh_is_binded(); LIST_SET(index, (void*)(&v)); }
 void 		mesh_triangles_set_at_0(index_t index, index_t i0) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(int), index)->x = i0; }
 void 		mesh_triangles_set_at_1(index_t index, index_t i1) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(int), index)->x = i1; }
 void 		mesh_triangles_set_at_2(index_t index, index_t i2) { ensure_mesh_is_binded(); LIST_GET_PTR(vec3_t(int), index)->x = i2; }

 vec3_t(int) 	mesh_triangles_get_at(index_t index) { vec3_t(int) v; ensure_mesh_is_binded(v); return (*LIST_GET_PTR(vec3_t(int), index)); }
 vec3_t(int)* 	mesh_triangles_get_at_ptr(index_t index) {ensure_mesh_is_binded(NULL); return LIST_GET_PTR(vec3_t(int), index); }
 int     	mesh_triangles_get_at_0(index_t index){ensure_mesh_is_binded(0); return (LIST_GET_PTR(vec3_t(int), index)->x); }
 int     	mesh_triangles_get_at_1(index_t index){ensure_mesh_is_binded(0); return (LIST_GET_PTR(vec3_t(int), index)->y); }
 int     	mesh_triangles_get_at_2(index_t index){ensure_mesh_is_binded(0); return (LIST_GET_PTR(vec3_t(int), index)->z); }



 void 		mesh_uvs_add(float x, float y) { ensure_mesh_is_binded();vec2_t(float) uv = {x, y}; LIST_ADD(&uv); }
 void 		mesh_uvs_add_vec2(vec2_t(float)   v) {ensure_mesh_is_binded(); LIST_ADD((void*)(&v)); }

 void 		mesh_uvs_set_at(index_t index, float x, float y) {ensure_mesh_is_binded(); vec2_t(float)* v = LIST_GET_PTR(vec2_t(float), (index)); v->x = x; v->y = y; }
 void 		mesh_uvs_set_at_vec2(index_t index, vec2_t(float)  uv){ ensure_mesh_is_binded();LIST_SET(index, (void*)(&uv)); }
 void 		mesh_uvs_set_at_x(index_t index, float x) { ensure_mesh_is_binded();LIST_GET_PTR(vec2_t(float), (index))->x = (x); }
 void 		mesh_uvs_set_at_y(index_t index, float y) { ensure_mesh_is_binded();LIST_GET_PTR(vec2_t(float), (index))->y = (y); }

 vec2_t(float) mesh_uvs_get_at(index_t index){ vec2_t(float) v; ensure_mesh_is_binded(v);return (*LIST_GET_PTR(vec2_t(float), (index))); }
 vec2_t(float)* mesh_uvs_get_at_ptr(index_t index){ ensure_mesh_is_binded(NULL);return LIST_GET_PTR(vec2_t(float), (index)); }
 float 		mesh_uvs_get_at_y(index_t index){ ensure_mesh_is_binded(0.0f);return (LIST_GET_PTR(vec2_t(float), (index))->y); }
 float 		mesh_uvs_get_at_x(index_t index){ ensure_mesh_is_binded(0.0f);return (LIST_GET_PTR(vec2_t(float), (index))->x); }

/*
New Feature and Performance improvement request, must be implemented in BUFFERlib version 1.2
	1. contiguous and same type of data memory blocks defragmentation
	2. support for multiple value pushes in one function call, better utilization of cache memory
*/
