/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-48.case3.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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


#include <renderer/tests/TID-48.case3.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

TEST_DATA(TID_48_CASE_3)
{
	render_scene_t* scene;
	camera_t* front_camera;
	camera_t* top_camera;
	camera_t* bottom_camera;
	camera_t* back_camera;
	light_t* light;
	material_t* material;
	mesh_t* mesh;
	render_object_t* render_object;
	float angle;
	float angular_speed;
};

SETUP_TEST(TID_48_CASE_3);

TEST_ON_RENDERER_INITIALIZE(TID_48_CASE_3)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_48_CASE_3",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_48_CASE_3)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->front_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->front_camera, COLOR_GREEN, 1.0f);
	camera_set_render_area_relative(this->front_camera, 0, 0, 50, 50);

	this->top_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->top_camera, COLOR_RED, 1.0f);
	camera_set_render_area_relative(this->top_camera, 50, 0, 50, 50);
	camera_set_position(this->top_camera, vec3(-1.1f, 1.2f, 0));
	camera_set_rotation(this->top_camera, vec3(0, 0, -50 DEG));

	this->bottom_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->bottom_camera, COLOR_BLUE, 1.0f);
	camera_set_render_area_relative(this->bottom_camera, 0, 50, 50, 50);
	camera_set_position(this->bottom_camera, vec3(-1.1f, 1.2f, 0));
	camera_set_rotation(this->bottom_camera, vec3(0, 0, -50 DEG));

	this->back_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->back_camera, COLOR_BLACK, 1.0f);
	camera_set_render_area_relative(this->back_camera, 50, 50, 50, 50);
	camera_set_position(this->back_camera, vec3(-1.1f, 1.2f, 0));
	camera_set_rotation(this->back_camera, vec3(0, 0, -50 DEG));

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

	render_scene_build_queues(this->scene);
}

TEST_ON_TERMINATE(TID_48_CASE_3)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	light_destroy(this->light);
	light_release_resources(this->light);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TID_48_CASE_3)
{
	this->angle += this->angular_speed * deltaTime;
	render_object_set_transform(this->render_object, mat4_rotation_y(this->angle));
}

TEST_ON_RENDER(TID_48_CASE_3)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

