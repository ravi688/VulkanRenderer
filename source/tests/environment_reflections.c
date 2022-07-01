
#include <renderer/tests/environment_reflections.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

TEST_DATA(ENVIRONMENT_REFLECTIONS)
{
	render_scene_t* scene;
	camera_t* camera;
	material_t* material;
	material_t* skyboxMaterial;
	mesh_t* mesh;
	mesh_t* skyboxMesh;
	render_object_t* render_object;
	render_object_t* skyboxObject;
	light_t* light;

	texture_t* skyboxTexture;

	float angle;
};

SETUP_TEST(ENVIRONMENT_REFLECTIONS);

TEST_ON_INITIALIZE(ENVIRONMENT_REFLECTIONS)
{
	this->angle = 0;

	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_WHITE, 1.0f);
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_BACKGROUND));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->skyboxMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_SKYBOX), "SkyboxMaterial"));
	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_REFLECTION), "MyMaterial"));
	material_set_vec4(this->material, "parameters.color", vec4(float)(1, 1, 1, 1));

	this->skyboxTexture = texture_load(renderer, TEXTURE_TYPE_CUBE, 
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	material_set_texture(this->skyboxMaterial, "albedo", this->skyboxTexture);
	material_set_texture(this->material, "reflectionMap", this->skyboxTexture);
	material_set_float(this->material, "parameters.reflectance", 1);
	AUTO cubeMeshData = mesh3d_cube(1);
	AUTO torusMeshData = mesh3d_load("models/Torus.obj");
	mesh3d_make_centroid_origin(torusMeshData);
	mesh3d_transform_set(torusMeshData, mat4_rotation(float)(0, 0, 30 DEG));

	this->mesh = mesh_create(renderer, torusMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);	


	mesh3d_flip_triangles(cubeMeshData);
	this->skyboxMesh = mesh_create(renderer, cubeMeshData);
	this->skyboxObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_BACKGROUND));
	render_object_set_material(this->skyboxObject, this->skyboxMaterial);
	render_object_attach(this->skyboxObject, this->skyboxMesh);
	render_object_set_transform(this->skyboxObject, mat4_scale(float)(10, 10, 10));
}

TEST_ON_TERMINATE(ENVIRONMENT_REFLECTIONS)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->skyboxMesh);
	mesh_release_resources(this->skyboxMesh);
	light_destroy(this->light);
	light_release_resources(this->light);

	texture_destroy(this->skyboxTexture);
	texture_release_resources(this->skyboxTexture);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(ENVIRONMENT_REFLECTIONS)
{
	this->angle += deltaTime * 30;
	render_object_set_transform(this->render_object, mat4_rotation(float)(0, this->angle DEG, 0));
}

TEST_ON_RENDER(ENVIRONMENT_REFLECTIONS)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

