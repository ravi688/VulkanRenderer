/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-28.case3.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <renderer/tests/TID-28.case3.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/conio.h>


TEST_DATA(TID_28_CASE_3)
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


	render_texture_t* environmentMap;
};

SETUP_TEST(TID_28_CASE_3);

TEST_ON_RENDERER_INITIALIZE(TID_28_CASE_3)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_28_CASE_3",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_28_CASE_3)
{
	this->angle = 0;

	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_WHITE, 1.0f);
	camera_set_active(this->camera, false);
	
	this->offscreenCamera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->offscreenCamera, COLOR_RED, 1.0f);
	camera_set_position(this->offscreenCamera, vec3_zero());
	camera_set_rotation(this->offscreenCamera, vec3(0, 0, 0));
	camera_set_field_of_view(this->offscreenCamera, 90 DEG);

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_BACKGROUND) | BIT64(RENDER_QUEUE_TYPE_QUEUE0));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->skyboxMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/skybox.sb"), "SkyboxMaterial"));
	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/reflection.depth.sb"), "MyMaterial"));
	this->material2 = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_load_shader(slib,
								"shaders/presets/lit.color.sb"), "LitColorMaterial"));


	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));
	material_set_vec4(this->material2, "parameters.color", vec4(1, 1, 1, 1));

	render_texture_create_info_t create_info = 
	{
		.width = 800,
		.height = 800,
		.depth = 1,
		.channel_count = 4,
		.type = RENDER_TEXTURE_TYPE_DEPTH_CUBE
	};	
	this->environmentMap = render_texture_create(renderer, &create_info);
	camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, this->environmentMap);

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
	AUTO cubeMeshData = mesh3d_cube(renderer->allocator, 1);
	AUTO torusMeshData = mesh3d_load(renderer->allocator, "models/Sphere.obj");
	mesh3d_make_centroid_origin(torusMeshData);
	mesh3d_transform_set(torusMeshData, mat4_rotation(0, 0, 30 DEG));

	this->mesh = mesh_create(renderer, torusMeshData);
	mesh3d_destroy(torusMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_QUEUE0));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);	

	this->mesh2 = mesh_create(renderer, cubeMeshData);
	this->render_object2 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object2, this->material2);
	render_object_attach(this->render_object2, this->mesh2);
	render_object_set_transform(this->render_object2, mat4_mul(2, mat4_translation(-1, 0.2f, 0.3f), mat4_scale(0.3f, 0.3f, 0.3f)));

	mesh3d_flip_triangles(cubeMeshData);
	this->skyboxMesh = mesh_create(renderer, cubeMeshData);
	mesh3d_destroy(cubeMeshData);
	this->skyboxObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_BACKGROUND));
	render_object_set_material(this->skyboxObject, this->skyboxMaterial);
	render_object_attach(this->skyboxObject, this->skyboxMesh);
	render_object_set_transform(this->skyboxObject, mat4_scale(10, 10, 10));

	render_scene_build_queues(this->scene);
}

TEST_ON_TERMINATE(TID_28_CASE_3)
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

	render_texture_destroy(this->environmentMap);
	render_texture_release_resources(this->environmentMap);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


static bool swap = false;

TEST_ON_UPDATE(TID_28_CASE_3)
{
	this->angle += deltaTime * 30;

	if(kbhit())
	{
		getch();
		swap = !swap;
		if(swap)
			camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED, CAMERA_RENDER_TARGET_SCREEN);
		else 
			camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, this->environmentMap);
	}
	mat4_t transform = mat4_mul(3, mat4_translation(-1, 0.2f, 0.3f), mat4_rotation(0, this->angle DEG, 0), mat4_scale(0.3f, 0.3f, 0.3f));
	render_object_set_transform(this->render_object2, transform);
}

TEST_ON_RENDER(TID_28_CASE_3)
{
	camera_set_active(this->camera, false);
	camera_set_active(this->offscreenCamera, true);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY), RENDER_SCENE_CLEAR);
	camera_set_active(this->camera, true);
	camera_set_active(this->offscreenCamera, false);
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

