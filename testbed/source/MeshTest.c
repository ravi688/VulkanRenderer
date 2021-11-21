// #include "MeshTest.h"
// #include "Square.h"
// #include "Circle.h"

// #include <vec3/vec3.h>

// OnInstantiate(MeshTest,
// 	BindOnAwake(MeshTest);
// 	BindOnStart(MeshTest);
// 	BindOnUpdate(MeshTest);
// 	BindOnDestroy(MeshTest);

// 	log_msg("MeshTest script is instantiated");
// 	this->width = 0.3f;
// 	this->height = 0.3f;
// )

// OnDestroy(MeshTest, 
// 	material_destroy(this->material);
// 	mesh_t* mesh = mesh_get_binded();
// 	mesh_bind(this->mesh);
// 	mesh_destroy(); 
// 	mesh_bind(mesh); 
// )

// OnAwake(MeshTest,
// 	log_msg("MeshTest script awake is called");

// 	object_t* child = object_new("Square"); 
// 	object_t* circle = object_new("Circle");
// 	object_set_parent(child, COMPONENT(this)->object);
// 	object_set_parent(circle, COMPONENT(this)->object);
// 	object_attach_component(child, Square);
// 	object_attach_component(circle, Circle);
// 	Square* square_script = object_get_component(child, Square);

// 	object_t* circle2 = object_new("Circle2");
// 	object_set_parent(circle2, COMPONENT(this)->object);
// 	object_attach_component(circle2, Circle);
// 	Circle* circle_script = object_get_component(circle2, Circle);

// 	circle_script->radius = 0.05f;
// 	circle_script->position = vec3(float)( 0.3f, 0.3f, 0);

// 	object_attach_component(COMPONENT(this)->object, mesh_renderer_t); 
// 	object_attach_component(COMPONENT(this)->object, mesh_filter_t);
// 	this->mesh_renderer = object_get_component(COMPONENT(this)->object, mesh_renderer_t); 
// 	this->mesh_filter = object_get_component(COMPONENT(this)->object, mesh_filter_t);
// 	this->mesh = mesh_new(); 
// 	this->material = material_new("Lambert"); 

// 	this->mesh_renderer->mesh_filter = this->mesh_filter;
// )

// static float clamp01(float value) { return ((value > 1.0f)  ? 1.0f : ((value < 0.0f) ? 0.0f : value)); }

// OnUpdate(MeshTest,
// 	// //puts("Changing color");
// 	// if(input_event->type == GDK_KEY_PRESS)
// 	// {
// 	// 	if(input_event->key.keyval == GDK_KEY_Up)
// 	// 		material_set_color4(this->material, clamp01(this->material->color.r + 0.1f), clamp01(this->material->color.g - 0.1f), 0.5f, 0.5f);
// 	// 	else
// 	// 		material_set_color4(this->material, clamp01(this->material->color.r - 0.1f), clamp01(this->material->color.g + 0.1f), 0.5f, 0.5f);
// 	// }
// )

// OnStart(MeshTest,
// 	log_msg("MeshTest script start is called");

// 	BUFFER* temp = BUFget_binded();
// 	mesh_t* _mesh = mesh_get_binded();
// 	mesh_bind(this->mesh);
// 	mesh_triangles_new(2); 
// 	mesh_positions_new(4);

// 	mesh_positions_bind();
// 	mesh_positions_add(this->width * 0.5f, this->height * 0.5f, 0.0f); 
// 	mesh_positions_add(this->width * 0.5f, -this->height * 0.5f, 0.0f); 
// 	mesh_positions_add(-this->width * 0.5f, -this->height * 0.5f, 0.0f); 
// 	mesh_positions_add(-this->width * 0.5f, this->height * 0.5f, 0.0f);

// 	mesh_triangles_bind();
// 	mesh_triangles_add(0, 1, 2); 
// 	mesh_triangles_add(2, 3, 0); 

// 	mesh_optimize_buffer(); 

// 	mesh_filter_meshes_new(this->mesh_filter, 1); 
// 	mesh_renderer_materials_new(this->mesh_renderer, 1); 

// 	mesh_filter_set_mesh(this->mesh_filter, this->mesh, 0); 
// 	mesh_renderer_set_material(this->mesh_renderer, this->material, 0); 

// 	material_set_color4(this->material, 0.0f, 0.5f, 0.5f, 0.5f);

// 	mesh_bind(_mesh);
// 	BUFbind(temp);
// )