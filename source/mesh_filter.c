#include <engine/mesh_filter.h>

OnInstantiate(mesh_filter_t,
	BindOnDestroy(mesh_filter_t);
	BindOnStart(mesh_filter_t); 
	BindOnAwake(mesh_filter_t);
	this->meshes = NULL;
)

OnStart(mesh_filter_t,
	log_msg("mesh_filter_t::OnStart is called");
)


OnAwake(mesh_filter_t,

	log_msg("mesh_filter_t::OnAwake is called");
)

OnDestroy(mesh_filter_t,
	mesh_filter_destroy(this);
	log_msg("mesh_filter_t component is Destroyed");
)

void mesh_filter_meshes_new(mesh_filter_t* mesh_filter, uint32_t mesh_count)
{
	if(mesh_filter != NULL)
	{
		LIST() _list = LIST_GET_BINDED();
		if(mesh_filter->meshes != NULL)
		{
			LIST_BIND(mesh_filter->meshes); 
			LIST_DESTROY();
		}
		mesh_filter->meshes = LIST_NEW(mesh_t*, mesh_count);
		BUFbind(mesh_filter->meshes); 
		BUFset_element_count(mesh_count);
		LIST_BIND(_list);
	}
}

void mesh_filter_destroy(mesh_filter_t* mesh_filter)
{
	if(mesh_filter == NULL)
		return;
	if(mesh_filter->meshes != NULL)
	{
		LIST() _list = LIST_GET_BINDED();
		LIST_BIND(mesh_filter->meshes); 
		LIST_DESTROY();
		LIST_BIND(_list);
		mesh_filter->meshes = NULL;
	}
}

void mesh_filter_set_mesh(mesh_filter_t* mesh_filter, mesh_t* mesh, uint32_t index)
{
	if((mesh_filter != NULL) && (mesh_filter->meshes != NULL))
	{
		BUFpush_binded();
		BUFbind(mesh_filter->meshes); 
		BUFset_at(index, &mesh);
		BUFpop_binded();
	}
}

LIST(mesh_t*) mesh_filter_get_mesh_list(mesh_filter_t* mesh_filter)
{
	if(mesh_filter != NULL)
		return mesh_filter->meshes;
	return NULL;
}
