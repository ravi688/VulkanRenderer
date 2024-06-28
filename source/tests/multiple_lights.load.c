/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: multiple_lights.load.c is a part of VulkanRenderer

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


#include <renderer/tests/multiple_lights.load.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/conio.h>


TEST_DATA(MULTIPLE_LIGHTS_LOAD)
{
	render_scene_t* scene;

	/* cameras */
	camera_t* camera;

	/* lights */
	light_t* pointLight;

	/* materials */
	material_t* material;

	/* meshes */
	mesh_t* mesh;
	mesh_t* wallMesh;
	mesh_t* cubeMesh;

	/* render objects*/
	render_object_t* renderObject;
	render_object_t* wallObject;
	render_object_t* cubeObject;

	/* textures */
	vulkan_texture_t* shadowMap;
};

SETUP_TEST(MULTIPLE_LIGHTS_LOAD);

TEST_ON_RENDERER_INITIALIZE(MULTIPLE_LIGHTS_LOAD)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "MULTIPLE_LIGHTS_LOAD",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(MULTIPLE_LIGHTS_LOAD)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create cameras */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_WHITE, 1.0f);
	camera_set_active(this->camera, true);

	/* create render scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_QUEUE0));
	render_scene_set_use_lights(this->scene, true);
	/* add the cameras which just created to the scene */
	render_scene_add_camera(this->scene, this->camera);
	
	this->pointLight = light_create(renderer, LIGHT_TYPE_POINT);
	light_set_position(this->pointLight, vec3_zero());
	light_set_intensity(this->pointLight, 0.5f);
	light_set_cast_shadow(this->pointLight, true);
	/* add the light which j ust created to the scene */
	render_scene_add_light(this->scene, this->pointLight);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/point_light.multiple.sb"), "PointLightMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));

	AUTO sphereMeshData = mesh3d_load(renderer->allocator, "models/Monkey.obj");
	mesh3d_make_centroid_origin(sphereMeshData);

	this->mesh = mesh_create(renderer, sphereMeshData);
	mesh3d_destroy(sphereMeshData);
	this->renderObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->renderObject, this->material);
	render_object_attach(this->renderObject, this->mesh);
	render_object_set_transform(this->renderObject, mat4_mul(2, mat4_rotation(0, -90 DEG, 0), mat4_scale(0.5f, 0.5f, 0.5f)));

	AUTO cubeMeshData = mesh3d_cube(renderer->allocator, 1);
	this->cubeMesh = mesh_create(renderer, cubeMeshData);
	this->cubeObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->cubeObject, this->material);
	render_object_attach(this->cubeObject, this->cubeMesh);
	render_object_set_transform(this->cubeObject, mat4_mul(2, mat4_translation(-0.5f, 0, 0.5f), mat4_scale(0.1f, 0.1f, 0.1f)));

	mesh3d_transform_set(cubeMeshData, mat4_scale(-1, -1, -1));
	this->wallMesh = mesh_create(renderer, cubeMeshData);
	mesh3d_destroy(cubeMeshData);
	this->wallObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->wallObject, this->material);
	render_object_attach(this->wallObject, this->wallMesh);
	render_object_set_transform(this->wallObject, mat4_scale(3, 3, 3));

	render_scene_build_queues(this->scene);

}

TEST_ON_TERMINATE(MULTIPLE_LIGHTS_LOAD)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->wallMesh);
	mesh_release_resources(this->wallMesh);
	mesh_destroy(this->cubeMesh);
	mesh_release_resources(this->cubeMesh);
	light_destroy(this->pointLight);
	light_release_resources(this->pointLight);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


static bool swap = false;
static float angle = 0;

TEST_ON_UPDATE(MULTIPLE_LIGHTS_LOAD)
{
	angle += deltaTime;
	float _angle = (0.5f * sin(angle) + 0.5f) * 180.0f - 180.0f;
	vec3_t pos = vec3(0.8f * sin(_angle DEG), 0, 0.8f * cos(_angle DEG));
	light_set_position(this->pointLight, pos);
	light_set_color(this->pointLight, vec3(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f, pos.z * 0.5f + 0.5f));
	render_object_set_transform(this->cubeObject, mat4_mul(2, mat4_translation(pos.x, pos.y, pos.z),
		mat4_scale(0.1f, 0.1f, 0.1f)));
}

TEST_ON_RENDER(MULTIPLE_LIGHTS_LOAD)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

