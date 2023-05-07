
#include <renderer/tests/TID-28.case1.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <renderer/conio.h>

TEST_DATA(TID_28_CASE_1)
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

	render_texture_t* colorRenderTexture;
	render_texture_t* depthRenderTexture;

	float angle;
	float speed;
	int camera_index;
};

SETUP_TEST(TID_28_CASE_1);

TEST_ON_INITIALIZE(TID_28_CASE_1)
{
	this->camera_system = renderer_get_camera_system(renderer);

	// TODO: every scene will have their own camera system ( Camera sets )
	// create a camera
	this->camera = camera_system_getH(this->camera_system,
							camera_system_create_camera(this->camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_WHITE, 1.0f);
	camera_set_active(this->camera, true);

	this->camera3 = camera_system_getH(this->camera_system,
							camera_system_create_camera(this->camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_active(this->camera3, false);
	camera_set_clear(this->camera3, COLOR_GREEN, 1.0f);
	camera_set_position(this->camera3, vec3(0, 0.6, -3.0f));
	camera_set_rotation(this->camera3, vec3(10 DEG, -90 DEG, 0));

	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);
	light_set_rotation(this->light, vec3(10 DEG, -120 DEG, 0));
	light_set_position(this->light, vec3(1, 0.6f, -3.0f));

	// create a render scene
	this->scene = render_scene_create_from_preset(renderer, RENDER_SCENE_PRESET_TYPE_DEFAULT);

	this->slib = renderer_get_shader_library(renderer);
	this->mlib = renderer_get_material_library(renderer);

	this->skyboxTexture = texture_load(renderer, TEXTURE_TYPE_CUBE, 
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	this->skyboxGeometry = mesh3d_cube(renderer->allocator, 5.0f);
	mesh3d_flip_triangles(this->skyboxGeometry);
	this->skyboxMesh = mesh_create(renderer, this->skyboxGeometry);
	this->skyboxShaderH = shader_library_load_shader(this->slib, "shaders/presets/skybox.sb");
	this->skyboxMaterial = material_library_getH(this->mlib, material_library_create_materialH(this->mlib, this->skyboxShaderH, "SkyboxMaterial"));
	material_set_texture(this->skyboxMaterial, "albedo", this->skyboxTexture);

	this->texture = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Smile.bmp");
	this->shaderH = shader_library_load_shader(this->slib, "shaders/presets/TID-28.case1.sb");
	this->shader = shader_library_getH(this->slib, this->shaderH);
	this->blueMaterial = material_library_getH(this->mlib, material_library_create_materialH(this->mlib, this->shaderH, "BlueColorMaterial"));
	this->greenMaterial = material_library_getH(this->mlib, material_library_create_materialH(this->mlib, this->shaderH, "GreenColorMaterial"));
	this->mesh = mesh_create(renderer, mesh3d_cube(renderer->allocator, 1.0f));
	this->planeMesh = mesh_create(renderer, mesh3d_plane(renderer->allocator, 2.0f));

	material_set_texture(this->blueMaterial, "albedo", this->texture);
	material_set_texture(this->greenMaterial, "albedo", this->texture);
	material_set_vec4(this->blueMaterial, "parameters.color", vec4(1, 1, 1, 1));
	material_set_vec4(this->greenMaterial, "parameters.color", vec4(1, 1, 1, 1));

	this->skyboxObj = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_BACKGROUND));
	render_object_set_material(this->skyboxObj, this->skyboxMaterial);
	render_object_attach(this->skyboxObj, this->skyboxMesh);

	this->obj2 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->obj2, this->greenMaterial);
	render_object_attach(this->obj2, this->planeMesh);
	render_object_set_transform(this->obj2, mat4_translation(0, -0.5f, 0));

	this->obj4 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->obj4, this->blueMaterial);
	render_object_attach(this->obj4, this->mesh);
	render_object_set_transform(this->obj4, mat4_mul(2, mat4_scale(0.5f, 0.5f, 0.5f), mat4_translation(-0.8f, 0.1f, 1)));

	this->obj1 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->obj1, this->blueMaterial);
	render_object_attach(this->obj1, this->mesh);
	render_object_set_transform(this->obj1, mat4_translation(0, 0, 0));

	this->uiShaderH = shader_library_load_shader(this->slib, "shaders/presets/unlit_ui.sb");
	this->uiShaderH2 = shader_library_load_shader(this->slib, "shaders/presets/unlit_ui2.sb");
	this->uiMaterial = material_library_getH(this->mlib, material_library_create_materialH(this->mlib, this->uiShaderH, "UIMaterial"));
	this->uiMaterial2 = material_library_getH(this->mlib, material_library_create_materialH(this->mlib, this->uiShaderH2, "UIMaterial2"));

	render_scene_add_queue(this->scene, RENDER_QUEUE_TYPE_QUEUE0);
	this->quadMesh = mesh_create(renderer, mesh3d_plane(renderer->allocator, 400));
	this->obj5 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(this->obj5, this->uiMaterial);
	render_object_attach(this->obj5, this->quadMesh);
	render_object_set_transform(this->obj5, mat4_mul(2, mat4_translation(0, 0, -200), mat4_rotation(0, 0, 90 DEG)));
	material_set_vec4(this->uiMaterial, "parameters.color", vec4(1, 1, 1, 1));

	this->obj6 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(this->obj6, this->uiMaterial2);
	render_object_attach(this->obj6, this->quadMesh);
	render_object_set_transform(this->obj6, mat4_mul(2, mat4_translation(0, 0, 200), mat4_rotation(0, 0, 90 DEG)));
	material_set_vec4(this->uiMaterial2, "parameters.color", vec4(1, 1, 1, 1));


	render_texture_create_info_t create_info = 
	{
		.width = renderer_get_window(renderer)->width,
		.height = renderer_get_window(renderer)->height,
		.depth = 1,
		.channel_count = 4,
		.type = RENDER_TEXTURE_TYPE_ALBEDO
	};
	this->colorRenderTexture = render_texture_create(renderer, &create_info);

	create_info.type = RENDER_TEXTURE_TYPE_DEPTH;
	this->depthRenderTexture = render_texture_create(renderer, &create_info);

	material_set_texture(this->uiMaterial, "albedo", this->depthRenderTexture);
	material_set_texture(this->uiMaterial2, "albedo", this->colorRenderTexture);
	
	camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, this->colorRenderTexture);
	camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, this->depthRenderTexture);

	this->angle = 0;
	this->speed = 20;
	this->camera_index = 0;
}

TEST_ON_TERMINATE(TID_28_CASE_1)
{	
	render_texture_destroy(this->colorRenderTexture);
	render_texture_release_resources(this->colorRenderTexture);

	render_texture_destroy(this->depthRenderTexture);
	render_texture_release_resources(this->depthRenderTexture);

	texture_destroy(this->texture);
	texture_release_resources(this->texture);

	texture_destroy(this->skyboxTexture);
	texture_release_resources(this->skyboxTexture);

	mesh_destroy(this->quadMesh);
	mesh_release_resources(this->quadMesh);
	mesh_destroy(this->planeMesh);
	mesh_release_resources(this->planeMesh);
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);

	mesh_destroy(this->skyboxMesh);
	mesh_release_resources(this->skyboxMesh);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);

	light_destroy(this->light);
	light_release_resources(this->light);
}

TEST_ON_UPDATE(TID_28_CASE_1)
{	
	if(kbhit())
	{
		getch();
		switch(this->camera_index % 2)
		{
			case 1:
				camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, this->colorRenderTexture);
				camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, this->depthRenderTexture);
				camera_set_active(this->camera, true);
				break;
			case 0:
				camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_SCREEN);
				camera_set_render_target(this->camera3, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_SCREEN);
				camera_set_active(this->camera, false);
				break;
		}
		this->camera_index++;
	}
	this->angle += deltaTime * this->speed;
	render_object_set_transform(this->obj1, mat4_rotation(0 DEG, this->angle DEG, 0 DEG));
}

TEST_ON_RENDER(TID_28_CASE_1)
{	
	// activate the offscreen camera and exclude the queue QUEUE0
	camera_set_active(this->camera3, true);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY)
							 | BIT64(RENDER_QUEUE_TYPE_BACKGROUND), RENDER_SCENE_CLEAR);
		
	// deactivate the offscreen camera and render only for QUEUE0
	camera_set_active(this->camera3, false);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_QUEUE0), RENDER_SCENE_DONT_CARE);
}