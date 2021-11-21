// #include "Square.h"


// OnInstantiate(Square,
// 	BindOnAwake(Square);
// 	BindOnStart(Square);
// 	BindOnDestroy(Square);

// 	this->side_length = 0.5f;
// 	this->mesh_filter = NULL; 
// 	this->mesh_renderer = NULL;	
// 	this->material = NULL;
// );

// OnDestroy(Square,
// 	mesh_t* mesh = mesh_get_binded();
// 	mesh_bind(this->mesh);
// 	mesh_destroy(); 
// 	mesh_bind(mesh);
// 	material_destroy(this->material);
// );

// OnAwake(Square, 
// 	object_attach_component(COMPONENT(this)->object, mesh_renderer_t); 
// 	object_attach_component(COMPONENT(this)->object, mesh_filter_t);
// 	this->mesh_renderer = object_get_component(COMPONENT(this)->object, mesh_renderer_t); 
// 	this->mesh_filter = object_get_component(COMPONENT(this)->object, mesh_filter_t);
// 	this->mesh = mesh_new();
// 	this->material = material_new("RedColor");
// 	material_set_color4(this->material, 1.0f, 0.0f, 0.0f, 1.0f);
// 	this->mesh_renderer->mesh_filter = this->mesh_filter;
// 	this->position = vec3(float)(0.1f, 0.1f, 0.0f);
// ); 

// OnStart(Square,

// 	mesh_t* _mesh = mesh_get_binded();
// 	BUFFER* buf = BUFget_binded();

// 	mesh_bind(this->mesh); 
// 	mesh_positions_new(3); 
// 	mesh_triangles_new(1); 

// 	mesh_positions_bind(); 
// 	float height = cos(3.1415926f / 6) * this->side_length;
// 	mesh_positions_add(0 + this->position.x, height * ((float)2 / 3) + this->position.y, 0); 
// 	mesh_positions_add(this->side_length * 0.5f + this->position.x, -height * ((float) 1 / 3) + this->position.y, 0);
// 	mesh_positions_add(-this->side_length * 0.5f + this->position.x, -height * ((float) 1 / 3) + this->position.y, 0);

// 	mesh_triangles_bind(); 
// 	mesh_triangles_add(0, 1, 2);

// 	mesh_optimize_buffer();

// 	mesh_renderer_materials_new(this->mesh_renderer, 1);
// 	mesh_renderer_set_material(this->mesh_renderer, this->material, 0);

// 	mesh_filter_meshes_new(this->mesh_filter, 1); 
// 	mesh_filter_set_mesh(this->mesh_filter, this->mesh, 0);

// 	mesh_bind(_mesh);
// 	BUFbind(buf);
// );
