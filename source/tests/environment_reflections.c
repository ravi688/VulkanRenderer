
#include <renderer/tests/environment_reflections.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

TEST_DATA(ENVIRONMENT_REFLECTIONS)
{
	render_scene_t* scene;
	camera_t* camera;
	camera_t* offscreenCamera;
	material_t* material;
	material_t* material2;
	material_t* skyboxMaterial;
	mesh_t* mesh;
	mesh_t* mesh2;
	mesh_t* skyboxMesh;
	render_object_t* render_object;
	render_object_t* render_object2;
	render_object_t* skyboxObject;
	light_t* light;

	texture_t* skyboxTexture;

	float angle;


	vulkan_texture_t* environmentMap;
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
	
	this->offscreenCamera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->offscreenCamera, COLOR_RED, 1.0f);
	camera_set_position(this->offscreenCamera, vec3_zero(float)());
	camera_set_rotation(this->offscreenCamera, vec3(float)(0, 0, 0));
	camera_set_field_of_view(this->offscreenCamera, 90 DEG);
	camera_set_active(this->offscreenCamera, false);

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_BACKGROUND) | BIT64(RENDER_QUEUE_TYPE_QUEUE0));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->skyboxMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_SKYBOX), "SkyboxMaterial"));
	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_create_shader_from_preset(slib, 
								SHADER_LIBRARY_SHADER_PRESET_REFLECTION), "MyMaterial"));
	this->material2 = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_create_shader_from_preset(slib,
								SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR), "LitColorMaterial"));


	material_set_vec4(this->material, "parameters.color", vec4(float)(1, 1, 1, 1));
	material_set_vec4(this->material2, "parameters.color", vec4(float)(1, 1, 1, 1));

	vulkan_texture_create_info_t create_info = 
	{
		.width = renderer_get_window(renderer)->width,
		.height = renderer_get_window(renderer)->height,
		.depth = 1,
		.channel_count = 4,
		.type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET ,
		.initial_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.usage = VULKAN_TEXTURE_USAGE_RENDER_TARGET,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.technique = VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH 
	};	
	this->environmentMap = vulkan_texture_create(renderer->vulkan_handle, &create_info);
	camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_COLOR, this->environmentMap);

	this->skyboxTexture = texture_load(renderer, TEXTURE_TYPE_CUBE, 
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	material_set_texture(this->skyboxMaterial, "albedo", this->skyboxTexture);
	material_set_texture(this->material, "reflectionMap", this->environmentMap);
	material_set_float(this->material, "parameters.reflectance", 1);
	AUTO cubeMeshData = mesh3d_cube(1);
	AUTO torusMeshData = mesh3d_load("models/Sphere.obj");
	mesh3d_make_centroid_origin(torusMeshData);
	mesh3d_transform_set(torusMeshData, mat4_rotation(float)(0, 0, 30 DEG));

	this->mesh = mesh_create(renderer, torusMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);	

	this->mesh2 = mesh_create(renderer, cubeMeshData);
	this->render_object2 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object2, this->material2);
	render_object_attach(this->render_object2, this->mesh2);
	render_object_set_transform(this->render_object2, mat4_mul(float)(2, mat4_translation(float)(-1, 0.2f, 0.3f), mat4_scale(float)(0.3f, 0.3f, 0.3f)));

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
	mesh_destroy(this->mesh2);
	mesh_release_resources(this->mesh2);
	mesh_destroy(this->skyboxMesh);
	mesh_release_resources(this->skyboxMesh);
	light_destroy(this->light);
	light_release_resources(this->light);

	texture_destroy(this->skyboxTexture);
	texture_release_resources(this->skyboxTexture);

	vulkan_texture_destroy(this->environmentMap);
	vulkan_texture_release_resources(this->environmentMap);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


static bool swap = false;

TEST_ON_UPDATE(ENVIRONMENT_REFLECTIONS)
{
	this->angle += deltaTime * 30;

	if(kbhit())
	{
		getch();
		swap = !swap;
		if(swap)
			camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_SCREEN);
		else 
			camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_COLOR, this->environmentMap);
	}
	mat4_t(float) transform = mat4_mul(float)(3, mat4_translation(float)(-1, 0.2f, 0.3f), mat4_rotation(float)(0, this->angle DEG, 0), mat4_scale(float)(0.3f, 0.3f, 0.3f));
	render_object_set_transform(this->render_object2, transform);
}

TEST_ON_RENDER(ENVIRONMENT_REFLECTIONS)
{
	camera_set_active(this->offscreenCamera, true);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_BACKGROUND), RENDER_SCENE_CLEAR);

	camera_set_active(this->offscreenCamera, false);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_QUEUE0), RENDER_SCENE_DONT_CARE);
}

