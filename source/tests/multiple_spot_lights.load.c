/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: spot_light.load.c is a part of VulkanRenderer

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


#include <renderer/tests/multiple_spot_lights.load.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <renderer/conio.h>

TEST_DATA(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	render_scene_t* scene;

	/* cameras */
	camera_t* camera;

	/* lights */
	light_t* light;
	light_t* light2;

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
	render_object_t* cubeObject2;
};

SETUP_TEST(MULTIPLE_SPOT_LIGHTS_LOAD);

TEST_ON_RENDERER_INITIALIZE(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "MULTIPLE_SPOT_LIGHTS_LOAD",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false,
		.max_spot_lights = 1
	};
}

TEST_ON_INITIALIZE(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create camera */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);

	/* create render scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	/* add the camera into the render scene */
	render_scene_add_camera(this->scene, this->camera);

	this->light = light_create(renderer, LIGHT_TYPE_SPOT);
	light_set_rotation(this->light, vec3(0, 0, 0));
	light_set_position(this->light, vec3(-0.5, 0, 0));
	light_set_spot_angle(this->light, 45 DEG);
	light_set_cast_shadow(this->light, true);
	light_set_color(this->light, COLOR_YELLOW.rgb);
	this->light2 = light_create(renderer, LIGHT_TYPE_SPOT);
	light_set_rotation(this->light2, vec3(0, 0, 0));
	light_set_position(this->light2, vec3(-0.5, 0, 0));
	light_set_spot_angle(this->light2, 45 DEG);
	light_set_cast_shadow(this->light2, true);
	light_set_color(this->light2, COLOR_WHITE.rgb);
	/* add the light into the render scene */
	render_scene_add_light(this->scene, this->light);
	render_scene_add_light(this->scene, this->light2);

	render_scene_set_use_lights(this->scene, true);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/spot_light.multiple.sb"), "SpotLightMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));

	this->unlitMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/unlit.color.sb"), "UnlitMaterial"));

	material_set_vec4(this->unlitMaterial, "parameters.color", vec4(1, 1, 1, 1));

	AUTO sphereMeshData = mesh3d_load(renderer->allocator, "models/Monkey.obj");
	mesh3d_make_centroid_origin(sphereMeshData);

	this->mesh = mesh_create(renderer, sphereMeshData);
	mesh3d_destroy(sphereMeshData);
	this->renderObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->renderObject, this->material);
	render_object_attach(this->renderObject, this->mesh);
	render_object_set_transform(this->renderObject, mat4_mul(2, mat4_rotation(0, -90 DEG, 0), mat4_scale(0.3f, 0.3f, 0.3f)));

	AUTO cubeMeshData = mesh3d_cube(renderer->allocator, 1);
	this->cubeMesh = mesh_create(renderer, cubeMeshData);
	this->cubeObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->cubeObject, this->unlitMaterial);
	render_object_attach(this->cubeObject, this->cubeMesh);
	render_object_set_transform(this->cubeObject, mat4_mul(2, mat4_translation(-0.5f, 0, 0.5f), mat4_scale(0.1f, 0.1f, 0.1f)));
	this->cubeObject2 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->cubeObject2, this->unlitMaterial);
	render_object_attach(this->cubeObject2, this->cubeMesh);
	render_object_set_transform(this->cubeObject2, mat4_mul(2, mat4_translation(-0.5f, 0, 0.5f), mat4_scale(0.1f, 0.1f, 0.1f)));
	mesh3d_transform_set(cubeMeshData, mat4_scale(-1, -1, -1));
	this->wallMesh = mesh_create(renderer, cubeMeshData);
	mesh3d_destroy(cubeMeshData);
	this->wallObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->wallObject, this->material);
	render_object_attach(this->wallObject, this->wallMesh);
	render_object_set_transform(this->wallObject, mat4_scale(3, 3, 3));

	render_scene_build_queues(this->scene);
}

TEST_ON_TERMINATE(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->wallMesh);
	mesh_release_resources(this->wallMesh);
	mesh_destroy(this->cubeMesh);
	mesh_release_resources(this->cubeMesh);
	light_destroy(this->light);
	light_release_resources(this->light);
	light_destroy(this->light2);
	light_release_resources(this->light2);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


static float angle = 0;

TEST_ON_UPDATE(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	angle += deltaTime;
	float _angle = (0.5f * sin(angle) + 0.5f) * 180.0f - 180.0f;
	vec3_t pos = vec3(sin(_angle DEG), 0, cos(_angle DEG));
	light_set_position(this->light, pos);
	light_set_rotation(this->light, vec3(0, (90 + _angle) DEG, 0));
	render_object_set_transform(this->cubeObject, mat4_mul(2, mat4_translation(pos.x, pos.y, pos.z),
		mat4_scale(0.05f, 0.05f, 0.05f)));
	_angle = (0.5f * sin(-angle) + 0.5f) * 180.0f - 180.0f;
	pos = vec3(sin(_angle DEG), 0, cos(_angle DEG));
	light_set_position(this->light2, pos);
	light_set_rotation(this->light2, vec3(0, (90 + _angle) DEG, 0));
	render_object_set_transform(this->cubeObject2, mat4_mul(2, mat4_translation_v(pos),
		mat4_scale(0.05f, 0.05f, 0.05f)));

	AUTO color = vec3(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f, pos.z * 0.5f + 0.5f);
	light_set_color(this->light, color);
	material_set_vec4(this->unlitMaterial, "parameters.color", vec4(color.x, color.y, color.z, 1));
}

TEST_ON_RENDER(MULTIPLE_SPOT_LIGHTS_LOAD)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

