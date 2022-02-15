
#include <Plane.h>
#include <RenderSystem.h>
#include <calltrace/calltrace.h>
#include <assert.h>
#include <renderer/mesh3d.h>

static void loadAndSetup(Plane* this)
{
	this->shader = shader_load(RENDERER, "resource/shaders/DiffuseShader.sb");
	material_create_info_t create_info = 
	{
		.is_vertex_attrib_from_file = true,
		.shader = this->shader
	};
	this->material = material_create(RENDERER, &create_info);
	material_set_vec3(this->material, "light.dir", vec3f_normalize((vec3f) { 0, -1, 1 }));
	material_set_vec3(this->material, "light.color", vec3_one(float)());
	material_set_float(this->material, "light.intensity", 1.0f);
	material_set_vec4(this->material, "properties.color", VEC4 { 0.1f, 0.2f, 0.4f, 1 });
	mesh3d_t* mesh_data = mesh3d_load("resource/models/Plane.obj");
	mesh3d_make_centroid_origin(mesh_data);
	this->mesh = mesh_create(RENDERER, mesh_data);
	mesh3d_destroy(mesh_data);
}

void OnInstantiate(Plane)
{
	BindOnDestroy(Plane);
	BindOnStart(Plane);
	BindOnRender(Plane);
}

void OnStart(Plane)
{
	this->camera = (Camera*)scene_manager_find_component_of_type(SCENE_MANAGER, Camera);
	assert(this->camera != NULL);
	this->transform = mat4f_mul(mat4f_translate(VEC3 { 1, -1, 0 } ), mat4f_scale(VEC3 { 2, 2, 2 } ));
	loadAndSetup(this);
}

// void OnUpdate(Plane)
// {
// 	this->angle += 360.0f * DELTA_TIME * 0.04f;
// 	if(this->angle >= 360.0f)
// 		this->angle = 0.0f;
// 	this->transform = mat4f_mul(mat4f_rotate((vec3f) { 0, this->angle * DEG2RAD, 0 }), mat4f_scale((vec3f) { 0.5f, 0.5f, 0.5f }));
// }

void OnRender(Plane)
{
	material_bind(this->material);
	mat4f mvp = mat4_mul(float)(3, this->camera->projection, this->camera->view, this->transform);
	material_set_push_mat4(this->material, "push.mvpMatrix", mat4f_transpose(mvp));
	material_set_push_mat4(this->material, "push.modelMatrix", mat4f_transpose(this->transform));
	mesh_draw_indexed(this->mesh);
}

void OnDestroy(Plane)
{
	shader_destroy(this->shader);
	material_destroy(this->material);
	mesh_destroy(this->mesh);
	shader_release_resources(this->shader);
	material_release_resources(this->material);
	mesh_release_resources(this->mesh);
}
