
#include <renderer/tests/TID-48.case2.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

TEST_DATA(TID_48_CASE_2)
{
	render_scene_t* scene;
	camera_t* camera;
	light_t* light;
	material_t* material;
	mesh_t* mesh;
	render_object_t* render_object;
	float angle;
	float angular_speed;
};

SETUP_TEST(TID_48_CASE_2);

TEST_ON_INITIALIZE(TID_48_CASE_2)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_render_area_relative(this->camera, 0, 0, 50, 100);

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/TID-48.case2.sb"), "MyMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));

	AUTO cubeMeshData = mesh3d_cube(renderer->allocator, 1);

	this->mesh = mesh_create(renderer, cubeMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);

	this->angle = 0;
	this->angular_speed = 60 DEG;
}

TEST_ON_TERMINATE(TID_48_CASE_2)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	light_destroy(this->light);
	light_release_resources(this->light);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TID_48_CASE_2)
{
	this->angle += this->angular_speed * deltaTime;
	render_object_set_transform(this->render_object, mat4_rotation_y(this->angle));
}

TEST_ON_RENDER(TID_48_CASE_2)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

