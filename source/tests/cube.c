/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: cube.c is a part of VulkanRenderer

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


#include <renderer/tests/cube.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

TEST_DATA(CUBE)
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

SETUP_TEST(CUBE);

TEST_ON_RENDERER_INITIALIZE(CUBE)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "CUBE",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(CUBE)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create camera */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);

	/* create scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	/* add the camera to the scene */
	render_scene_add_camera(this->scene, this->camera);

	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/lit.color.sb"), "MyMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));

	AUTO cubeMeshData = mesh3d_cube(renderer->allocator, 1);

	this->mesh = mesh_create(renderer, cubeMeshData);
	mesh3d_destroy(cubeMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);

	render_scene_build_queues(this->scene);

	this->angle = 0;
	this->angular_speed = 60 DEG;
}

TEST_ON_TERMINATE(CUBE)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	light_destroy(this->light);
	light_release_resources(this->light);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(CUBE)
{
	this->angle += this->angular_speed * deltaTime;
	render_object_set_transform(this->render_object, mat4_rotation_y(this->angle));
}

TEST_ON_RENDER(CUBE)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

