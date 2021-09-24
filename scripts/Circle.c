#include "Circle.h"

static float PI = 3.1415926f;

OnInstantiate(Circle, 

	BindOnAwake(Circle);
	BindOnStart(Circle);
	BindOnDestroy(Circle);

	this->radius = 0.1f;
	this->divisons = 20;
	this->position = vec3(float)(-0.3f, 0.3f, 0.0f);
	this->mesh_filter = NULL; 
	this->mesh_renderer = NULL; 
	this->material = NULL;
	this->mesh = NULL;
	this->dangle = (2 * PI) / this->divisons;
)

OnDestroy(Circle, 

	mesh_t* mesh = mesh_get_binded(); 

	mesh_bind(this->mesh); 
	mesh_destroy();

	material_destroy(this->material);
	mesh_bind(mesh);
)


OnAwake(Circle, 

	object_attach_component(COMPONENT(this)->object, mesh_renderer_t);
	object_attach_component(COMPONENT(this)->object, mesh_filter_t); 

	this->mesh_renderer = object_get_component(COMPONENT(this)->object, mesh_renderer_t); 
	this->mesh_filter = object_get_component(COMPONENT(this)->object, mesh_filter_t);

	this->material = material_new("Blue");
	material_set_color4(this->material, 0.0f, 1.0f, 0.0f, 1.0f);

	this->mesh = mesh_new();
	this->mesh_renderer->mesh_filter = this->mesh_filter;
)

OnStart(Circle,

	BUFFER* buf = BUFget_binded();
	mesh_t* mesh = mesh_get_binded();

	mesh_bind(this->mesh);

	mesh_positions_new(this->divisons + 1);
	mesh_triangles_new(this->divisons);

	float angle = 0;
	vec3_t(float) position;
	for(int i = 0; i < this->divisons; i++, angle += this->dangle)
	{
		position.x = this->position.x + this->radius * cos(angle); 
		position.y = this->position.y + this->radius * sin(angle); 
		position.z = 0.0f;
		//vec_print(float, 3, &position);
		mesh_positions_bind();
		mesh_positions_add_vec3(position);
		mesh_triangles_bind(); 
		mesh_triangles_add(this->divisons, i, (i + 1) % (this->divisons));
		// /printf("(%d, %d, %d)", this->divisons, i, (i + 1) % (this->divisons));
	}

	mesh_positions_bind();
	mesh_positions_add_vec3(this->position);

	mesh_optimize_buffer();

	mesh_filter_meshes_new(this->mesh_filter, 1); 
	mesh_renderer_materials_new(this->mesh_renderer, 1); 

	mesh_filter_set_mesh(this->mesh_filter, this->mesh, 0); 
	mesh_renderer_set_material(this->mesh_renderer, this->material, 0);

	mesh_bind(mesh);
	BUFbind(buf);

)