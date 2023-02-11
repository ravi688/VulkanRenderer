
#include <renderer/tests/depth_render_texture.load.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <renderer/conio.h>

TEST_DATA(DEPTH_RENDER_TEXTURE_LOAD)
{
	camera_system_t* camera_system;
	camera_t* camera;
	camera_t* camera3;
	light_t* light;
	render_scene_t* scene;

	shader_library_t* slib;
	material_library_t* mlib;

	texture_t* skyboxTexture;
	mesh3d_t* skyboxGeometry;
	mesh_t* skyboxMesh;
	shader_handle_t skyboxShaderH;
	material_t* skyboxMaterial;

	texture_t* texture;
	shader_handle_t shaderH;
	shader_t* shader;
	material_t* blueMaterial;
	material_t* greenMaterial;
	mesh_t* mesh;
	mesh_t* planeMesh;

	render_object_t* skyboxObj;
	render_object_t* obj1;
	render_object_t* obj2;
	// render_object_t* obj3;
	render_object_t* obj4;
	render_object_t* obj5;
	render_object_t* obj6;

	shader_handle_t uiShaderH;
	shader_handle_t uiShaderH2;

	material_t* uiMaterial;
	material_t* uiMaterial2;

	mesh_t* quadMesh;

	vulkan_texture_t* colorRenderTexture;
	vulkan_texture_t* depthRenderTexture;

	float angle;
	float speed;
	int camera_index;
};

SETUP_TEST(DEPTH_RENDER_TEXTURE_LOAD);

static void initialize(renderer_t* renderer, TEST_DATA(DEPTH_RENDER_TEXTURE_LOAD)* data)
{
	data->camera_system = renderer_get_camera_system(renderer);

	// TODO: every scene will have their own camera system ( Camera sets )
	// create a camera
	data->camera = camera_system_getH(data->camera_system,
							camera_system_create_camera(data->camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(data->camera, COLOR_WHITE, 1.0f);
	camera_set_active(data->camera, true);

	data->camera3 = camera_system_getH(data->camera_system,
							camera_system_create_camera(data->camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_active(data->camera3, false);
	camera_set_clear(data->camera3, COLOR_GREEN, 1.0f);
	camera_set_position(data->camera3, vec3(0, 0.6, -3.0f));
	camera_set_rotation(data->camera3, vec3(10 DEG, -90 DEG, 0));

	data->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);
	light_set_rotation(data->light, vec3(10 DEG, -120 DEG, 0));
	light_set_position(data->light, vec3(1, 0.6f, -3.0f));

	// create a render scene
	data->scene = render_scene_create_from_preset(renderer, RENDER_SCENE_PRESET_TYPE_DEFAULT);

	data->slib = renderer_get_shader_library(renderer);
	data->mlib = renderer_get_material_library(renderer);

	data->skyboxTexture = texture_load(renderer, TEXTURE_TYPE_CUBE, 
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	data->skyboxGeometry = mesh3d_cube(renderer->allocator, 5.0f);
	mesh3d_flip_triangles(data->skyboxGeometry);
	data->skyboxMesh = mesh_create(renderer, data->skyboxGeometry);
	data->skyboxShaderH = shader_library_load_shader(data->slib, "shaders/presets/skybox.sb");
	data->skyboxMaterial = material_library_getH(data->mlib, material_library_create_materialH(data->mlib, data->skyboxShaderH, "SkyboxMaterial"));
	material_set_texture(data->skyboxMaterial, "albedo", data->skyboxTexture);

	data->texture = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Smile.bmp");
	data->shaderH = shader_library_load_shader(data->slib, "shaders/presets/diffuse_test.sb");
	data->shader = shader_library_getH(data->slib, data->shaderH);
	data->blueMaterial = material_library_getH(data->mlib, material_library_create_materialH(data->mlib, data->shaderH, "BlueColorMaterial"));
	data->greenMaterial = material_library_getH(data->mlib, material_library_create_materialH(data->mlib, data->shaderH, "GreenColorMaterial"));
	data->mesh = mesh_create(renderer, mesh3d_cube(renderer->allocator, 1.0f));
	data->planeMesh = mesh_create(renderer, mesh3d_plane(renderer->allocator, 2.0f));

	material_set_texture(data->blueMaterial, "albedo", data->texture);
	material_set_texture(data->greenMaterial, "albedo", data->texture);
	material_set_vec4(data->blueMaterial, "parameters.color", vec4(1, 1, 1, 1));
	material_set_vec4(data->greenMaterial, "parameters.color", vec4(1, 1, 1, 1));

	data->skyboxObj = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_BACKGROUND));
	render_object_set_material(data->skyboxObj, data->skyboxMaterial);
	render_object_attach(data->skyboxObj, data->skyboxMesh);

	data->obj2 = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(data->obj2, data->greenMaterial);
	render_object_attach(data->obj2, data->planeMesh);
	render_object_set_transform(data->obj2, mat4_translation(0, -0.5f, 0));

	data->obj4 = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(data->obj4, data->blueMaterial);
	render_object_attach(data->obj4, data->mesh);
	render_object_set_transform(data->obj4, mat4_mul(2, mat4_scale(0.5f, 0.5f, 0.5f), mat4_translation(-0.8f, 0.1f, 1)));

	data->obj1 = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(data->obj1, data->blueMaterial);
	render_object_attach(data->obj1, data->mesh);
	render_object_set_transform(data->obj1, mat4_translation(0, 0, 0));

	data->uiShaderH = shader_library_load_shader(data->slib, "shaders/presets/unlit_ui.sb");
	data->uiShaderH2 = shader_library_load_shader(data->slib, "shaders/presets/unlit_ui2.sb");
	data->uiMaterial = material_library_getH(data->mlib, material_library_create_materialH(data->mlib, data->uiShaderH, "UIMaterial"));
	data->uiMaterial2 = material_library_getH(data->mlib, material_library_create_materialH(data->mlib, data->uiShaderH2, "UIMaterial2"));

	render_scene_add_queue(data->scene, RENDER_QUEUE_TYPE_QUEUE0);
	data->quadMesh = mesh_create(renderer, mesh3d_plane(renderer->allocator, 400));
	data->obj5 = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(data->obj5, data->uiMaterial);
	render_object_attach(data->obj5, data->quadMesh);
	render_object_set_transform(data->obj5, mat4_mul(2, mat4_translation(0, 0, -200), mat4_rotation(0, 0, 90 DEG)));
	material_set_vec4(data->uiMaterial, "parameters.color", vec4(1, 1, 1, 1));

	data->obj6 = render_scene_getH(data->scene, render_scene_create_object(data->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(data->obj6, data->uiMaterial2);
	render_object_attach(data->obj6, data->quadMesh);
	render_object_set_transform(data->obj6, mat4_mul(2, mat4_translation(0, 0, 200), mat4_rotation(0, 0, 90 DEG)));
	material_set_vec4(data->uiMaterial2, "parameters.color", vec4(1, 1, 1, 1));


	vulkan_texture_create_info_t create_info = 
	{
		.width = renderer_get_window(renderer)->width,
		.height = renderer_get_window(renderer)->height,
		.depth = 1,
		.channel_count = 4,
		.type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
		.initial_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.usage = VULKAN_TEXTURE_USAGE_RENDER_TARGET,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.technique = VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH
	};
	data->colorRenderTexture = vulkan_texture_create(renderer->vulkan_handle, &create_info);

	create_info.type = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
	data->depthRenderTexture = vulkan_texture_create(renderer->vulkan_handle, &create_info);

	material_set_texture(data->uiMaterial, "albedo", data->depthRenderTexture);
	material_set_texture(data->uiMaterial2, "albedo", data->colorRenderTexture);
	
	camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, data->colorRenderTexture);
	camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, data->depthRenderTexture);

	data->angle = 0;
	data->speed = 20;
	data->camera_index = 0;
}

static void terminate(renderer_t* renderer, TEST_DATA(DEPTH_RENDER_TEXTURE_LOAD)* data)
{	texture_destroy(data->colorRenderTexture);
	texture_release_resources(data->colorRenderTexture);

	texture_destroy(data->depthRenderTexture);
	texture_release_resources(data->depthRenderTexture);

	texture_destroy(data->texture);
	texture_release_resources(data->texture);

	texture_destroy(data->skyboxTexture);
	texture_release_resources(data->skyboxTexture);

	mesh_destroy(data->quadMesh);
	mesh_release_resources(data->quadMesh);
	mesh_destroy(data->planeMesh);
	mesh_release_resources(data->planeMesh);
	mesh_destroy(data->mesh);
	mesh_release_resources(data->mesh);

	mesh_destroy(data->skyboxMesh);
	mesh_release_resources(data->skyboxMesh);

	render_scene_destroy(data->scene);
	render_scene_release_resources(data->scene);

	light_destroy(data->light);
	light_release_resources(data->light);

	heap_free(data);
}

static void update(renderer_t* renderer, float deltaTime, TEST_DATA(DEPTH_RENDER_TEXTURE_LOAD)* data)
{	if(kbhit())
	{
		getch();
		switch(data->camera_index % 2)
		{
			case 1:
				camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, data->colorRenderTexture);
				camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, data->depthRenderTexture);
				camera_set_active(data->camera, true);
				break;
			case 0:
				camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_SCREEN);
				camera_set_render_target(data->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_SCREEN);
				camera_set_active(data->camera, false);
				break;
		}
		data->camera_index++;
	}
	data->angle += deltaTime * data->speed;
	render_object_set_transform(data->obj1, mat4_rotation(0 DEG, data->angle DEG, 0 DEG));
}

static void render(renderer_t* renderer, TEST_DATA(DEPTH_RENDER_TEXTURE_LOAD)* data)
{	// activate the offscreen camera and exclude the queue QUEUE0
	camera_set_active(data->camera3, true);
	render_scene_render(data->scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY)
							 | BIT64(RENDER_QUEUE_TYPE_BACKGROUND), RENDER_SCENE_CLEAR);
		
	// deactivate the offscreen camera and render only for QUEUE0
	camera_set_active(data->camera3, false);
	render_scene_render(data->scene, BIT64(RENDER_QUEUE_TYPE_QUEUE0), RENDER_SCENE_DONT_CARE);
}
