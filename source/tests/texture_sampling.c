/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: texture_sampling.c is a part of VulkanRenderer

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


/*
	test id: TEXTURE_SAMPLING
	Tests render to depth texture and present that to screen.
	while depth texture's dimensions is rectangular rather than square
	and different from the swapchain's dimensions
 */


#include <sge/tests/TID-48.case5.h>

#define SGE_INCLUDE_EVERYTHING_INTERNAL
#define SGE_INCLUDE_MATH
#define SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#define SGE_INCLUDE_3D_LIGHT_SYSTEM
#define SGE_INCLUDE_CORE
#include <sge/sge.h>

TEST_DATA(TEXTURE_SAMPLING)
{
	render_scene_t* scene;

	camera_t* camera;

	light_t* light;
	material_t* material;
	mesh_t* mesh;
	render_object_t* render_object;
	float angle;
	float angular_speed;

	texture_t* texture;
};

SETUP_TEST(TEXTURE_SAMPLING);

TEST_ON_SGE_INITIALIZE(TEXTURE_SAMPLING)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TEXTURE_SAMPLING",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TEXTURE_SAMPLING)
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
	
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);
	render_scene_add_light(this->scene, this->light);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/builtins/texture_sample.sb"), "MyMaterial"));

	this->texture = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Smile.bmp");

	material_set_texture(this->material, "albedo", this->texture);

	AUTO planeMeshData = mesh3d_plane(renderer->allocator, 1);
	this->mesh = mesh_create(renderer, planeMeshData);
	mesh3d_destroy(planeMeshData);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);
	render_object_set_transform(this->render_object, mat4_mul(3, mat4_scale(1, 1, 1), mat4_rotation_z(0 DEG), mat4_translation(0.0f, 0.0f, 0)));


	render_scene_build_queues(this->scene);

}

TEST_ON_TERMINATE(TEXTURE_SAMPLING)
{
	texture_destroy(this->texture);
	texture_release_resources(this->texture);
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	light_destroy(this->light);
	light_release_resources(this->light);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TEXTURE_SAMPLING)
{
}

TEST_ON_RENDER(TEXTURE_SAMPLING)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

