#ifndef __MESH_FILTER_H__
#define __MESH_FILTER_H__

#include <engine/object.h>
#include <engine/component.h>
#include <debug.h>
#include <engine/mesh.h>
#include <buffer.h>

#define mesh_filter_t_TYPE_ID ((uint64_t)(Component_TYPE_ID | (((uint64_t)6) << 32)))

typedef struct
{
	component_t component;
	LIST(mesh_t*) meshes; 			//sub meshes
} mesh_filter_t;

void mesh_filter_meshes_new(mesh_filter_t* mesh_filter, uint32_t mesh_count);
LIST(mesh_t*) mesh_filter_get_mesh_list(mesh_filter_t* mesh_filter);
void mesh_filter_set_mesh(mesh_filter_t* mesh_filter, mesh_t* mesh, uint32_t index);
void mesh_filter_destroy(mesh_filter_t* mesh_filter);


component_declare_callbacks(mesh_filter_t);
#endif/*__MESH_FILTER_H__*/