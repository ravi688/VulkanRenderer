#include <engine/mesh_renderer.h>

static void mesh_renderer_render(mesh_t* mesh, material_t* material);

OnInstantiate(mesh_renderer_t,
	
	BindOnDestroy(mesh_renderer_t); 
	BindOnAwake(mesh_renderer_t); 
	BindOnStart(mesh_renderer_t);
	BindOnPreRender(mesh_renderer_t);

	this->mesh_filter = NULL;
	this->materials = NULL;
	this->default_material = NULL;
	log_msg("mesh_renderer_t is instantiated");
)

OnDestroy(mesh_renderer_t,
	mesh_renderer_destroy(this);
	log_msg("mesh_renderer_t component is Destroyed");
)


OnAwake(mesh_renderer_t,
	this->default_material = material_new("DefaultMat"); 
	material_set_color4(this->default_material, 0.6f, 0.9f, 0.9f, 1.0f);
	log_msg("mesh_renderer_t awake is called");
)

OnStart(mesh_renderer_t,

	log_msg("mesh_renderer_t start is called");
)

OnPreRender(mesh_renderer_t, 
	if(this->mesh_filter == NULL)
		return;
	BUFFER* list = BUFget_binded();
	BUFFER* meshes = mesh_filter_get_mesh_list(this->mesh_filter);
	if(meshes != NULL)
	{
		BUFFER* materials = this->materials;
		BUFbind(meshes);
		uint32_t count = BUFget_element_count(); 
		material_t* material = NULL;
		mesh_t* mesh = NULL;
		for(int i = 0; i < count; i++)
		{
			mesh = *((mesh_t**)BUFget_ptr_at(i));
			if(materials != NULL)
			{
				BUFbind(materials);
				if(BUFget_element_count() > i)
					material  = *((material_t**)BUFget_ptr_at(i));
				else 
					material = this->default_material;
			}
			else material = this->default_material;
			mesh_renderer_render(mesh, material);
		}
	}
	BUFbind(list);
)


static void mesh_renderer_render(mesh_t* mesh, material_t* material)
{
	BUFFER* list = BUFget_binded();
	//cairo_set_source_rgb(cairo_context, material->color.r, material->color.g, material->color.b);

	BUFFER* triangles = mesh->triangles; 
	vec3_t(float)* vertex_positions = NULL;
	if(triangles != NULL)
	{
		BUFbind(mesh->positions);
		vertex_positions = ((vec3_t(float)*)BUFget_ptr());
		BUFbind(triangles);
		uint32_t triangle_count = BUFget_element_count(); 
		vec3_t(int)  triangle;
		vec3_t(float) position;
		vec2_t(int) screen_point;
		for(int i = 0; i < triangle_count; i++)
		{
			triangle = *(vec3_t(int)*)BUFget_ptr_at(i);
			position = vertex_positions[triangle.x];
			//core_convert_clip_space_to_screen_space(&position, &screen_point);
			//cairo_move_to(cairo_context, screen_point.x, screen_point.y); 
			position = vertex_positions[triangle.y];
			//core_convert_clip_space_to_screen_space(&position, &screen_point);
			//cairo_line_to(cairo_context, screen_point.x, screen_point.y);
			position = vertex_positions[triangle.z];
			//core_convert_clip_space_to_screen_space(&position, &screen_point);
			//cairo_line_to(cairo_context, screen_point.x, screen_point.y);
		}
		//cairo_fill(cairo_context);
	}
	BUFbind(list);
}	

void mesh_renderer_materials_new(mesh_renderer_t* mesh_renderer, uint32_t count)
{
	if(mesh_renderer == NULL)
	{
		log_wrn("mesh_renderer_t is NULL"); 
		return;
	}

	BUFFER* list = BUFget_binded();
	if(mesh_renderer->materials != NULL)
	{
		BUFbind(mesh_renderer->materials); 
		BUFfree();
	}
	mesh_renderer->materials = LIST_NEW(material_t*, count);
	BUFbind(mesh_renderer->materials);
	BUFset_element_count(count);
	BUFbind(list);
}

void mesh_renderer_destroy(mesh_renderer_t* mesh_renderer)
{
	if(mesh_renderer == NULL)
		return;
	if(mesh_renderer->materials != NULL)
	{
		LIST() _list = LIST_GET_BINDED();
		LIST_BIND(mesh_renderer->materials); 
		LIST_DESTROY();
		LIST_BIND(_list);
		mesh_renderer->materials = NULL;
	}
	if(mesh_renderer->default_material != NULL)
	{
		material_destroy(mesh_renderer->default_material);
		mesh_renderer->default_material = NULL;
	}
}

LIST(material_t*) mesh_renderer_get_material_list(mesh_renderer_t* mesh_renderer)
{
	if(mesh_renderer != NULL)
		return mesh_renderer->materials;
	return NULL;
}
void mesh_renderer_set_material(mesh_renderer_t* mesh_renderer, material_t* material, uint32_t index)
{
	BUFFER* list = BUFget_binded();
	if(mesh_renderer->materials != NULL)
	{
		BUFbind(mesh_renderer->materials);
		BUFset_at(index, &material);
	}
	else
		log_wrn("there is no materials buffer in mesh_renderer, first call mesh_renderer_materials_new()");
	BUFbind(list);	
}