
#include <renderer/tests/spot_light.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

TEST_DATA(SPOT_LIGHT)
{
	render_scene_t* scene;

	/* cameras */
	camera_t* camera;

	/* lights */
	light_t* light;

	/* materials */
	material_t* material;
	material_t* unlitMaterial;

	/* meshes */
	mesh_t* mesh;
	mesh_t* wallMesh;
	mesh_t* cubeMesh;

	/* render objects*/
	render_object_t* renderObject;
	render_object_t* wallObject;
	render_object_t* cubeObject;
};

SETUP_TEST(SPOT_LIGHT);

TEST_ON_INITIALIZE(SPOT_LIGHT)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_BLACK, 1.0f);
	camera_set_active(this->camera, true);

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	this->light = light_create(renderer, LIGHT_TYPE_SPOT);
	light_set_rotation(this->light, vec3(float)(0, 0, 0));
	light_set_position(this->light, vec3(float)(-0.5, 0, 0));
	light_set_spot_angle(this->light, 90 DEG);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT), "SpotLightMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(float)(1, 1, 1, 1));

	this->unlitMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR), "UnlitMaterial"));

	material_set_vec4(this->unlitMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));

	AUTO sphereMeshData = mesh3d_load("models/Monkey.obj");
	mesh3d_make_centroid_origin(sphereMeshData);

	this->mesh = mesh_create(renderer, sphereMeshData);
	this->renderObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->renderObject, this->material);
	render_object_attach(this->renderObject, this->mesh);
	render_object_set_transform(this->renderObject, mat4_mul(float)(2, mat4_rotation(float)(0, -90 DEG, 0), mat4_scale(float)(0.3f, 0.3f, 0.3f)));

	AUTO cubeMeshData = mesh3d_cube(1);
	this->cubeMesh = mesh_create(renderer, cubeMeshData);
	this->cubeObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->cubeObject, this->unlitMaterial);
	render_object_attach(this->cubeObject, this->cubeMesh);
	render_object_set_transform(this->cubeObject, mat4_mul(float)(2, mat4_translation(float)(-0.5f, 0, 0.5f), mat4_scale(float)(0.1f, 0.1f, 0.1f)));
	mesh3d_transform_set(cubeMeshData, mat4_scale(float)(-1, -1, -1));
	this->wallMesh = mesh_create(renderer, cubeMeshData);
	this->wallObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->wallObject, this->material);
	render_object_attach(this->wallObject, this->wallMesh);
	render_object_set_transform(this->wallObject, mat4_scale(float)(3, 3, 3));

}

TEST_ON_TERMINATE(SPOT_LIGHT)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->wallMesh);
	mesh_release_resources(this->wallMesh);
	mesh_destroy(this->cubeMesh);
	mesh_release_resources(this->cubeMesh);
	light_destroy(this->light);
	light_release_resources(this->light);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


static float angle = 0;

TEST_ON_UPDATE(SPOT_LIGHT)
{
	angle += deltaTime;
	float _angle = (0.5f * sin(angle) + 0.5f) * 180.0f - 180.0f;
	vec3_t(float) pos = vec3(float)(sin(_angle DEG), 0, cos(_angle DEG));
	light_set_position(this->light, pos);
	light_set_rotation(this->light, vec3(float)(0, (90 + _angle) DEG, 0));
	AUTO color = vec3(float)(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f, pos.z * 0.5f + 0.5f);
	light_set_color(this->light, color);
	material_set_vec4(this->unlitMaterial, "parameters.color", vec4(float)(color.x, color.y, color.z, 1));
	render_object_set_transform(this->cubeObject, mat4_mul(float)(2, mat4_translation(float)(pos.x, pos.y, pos.z),
		mat4_scale(float)(0.05f, 0.05f, 0.05f)));
}

TEST_ON_RENDER(SPOT_LIGHT)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

