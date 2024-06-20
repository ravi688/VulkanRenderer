/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-48.case5.c is a part of VulkanRenderer

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
	test id: TID_48_CASE_5 (Render Target Switching)
	Tests render to depth texture and present that to screen.
	while depth texture's dimensions is rectangular rather than square
	and different from the swapchain's dimensions
 */


#include <renderer/tests/TID-48.case5.h>

#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/conio.h>

TEST_DATA(TID_48_CASE_5)
{
	render_scene_t* scene;

	camera_t* offscreen_camera;
	camera_t* depth_present_camera;

	light_t* light;
	material_t* material;
	material_t* depthPresentMaterial;
	material_t* colorPresentMaterial;
	mesh_t* mesh;
	mesh_t* quadMesh;
	render_object_t* render_object;
	render_object_t* quadObject;
	float angle;
	float angular_speed;

	render_texture_t* depth_texture;
	render_texture_t* color_texture;
};

SETUP_TEST(TID_48_CASE_5);

TEST_ON_RENDERER_INITIALIZE(TID_48_CASE_5)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_48_CASE_5 (Press 't' to switch to screen depth, 'y' to external depth texture",
		.window_width = 1000,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_48_CASE_5)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->depth_present_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->depth_present_camera, COLOR_BLUE, 1.0f);
	camera_set_active(this->depth_present_camera, false);

	this->offscreen_camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->offscreen_camera, COLOR_BLACK, 1.0f);
	camera_set_active(this->offscreen_camera, false);

	vec3_t pos = camera_get_position(this->offscreen_camera);
	pos.z -= 0.7f;
	camera_set_position(this->offscreen_camera, pos);


	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_GEOMETRY_LAST));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/lit.color.sb"), "MyMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));

	this->depthPresentMaterial = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_load_shader(slib,
								"shaders/builtins/depth_present.sb"), "DepthPresentMaterial"));
	material_set_vec4(this->depthPresentMaterial, "parameters.color", vec4(1.0, 1.0, 1.0, 1.0));

	this->colorPresentMaterial = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_load_shader(slib,
								"shaders/builtins/color_present.sb"), "ColorPresentMaterial"));
	material_set_vec4(this->colorPresentMaterial, "parameters.color", vec4(1.0, 1.0, 1.0, 1.0));

	AUTO cube_mesh_3d = mesh3d_cube(renderer->allocator, 1);
	this->mesh = mesh_create(renderer, cube_mesh_3d);
	mesh3d_destroy(cube_mesh_3d);
	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->mesh);

	AUTO plane_mesh_3d = mesh3d_plane(renderer->allocator, 1);
	this->quadMesh = mesh_create(renderer, plane_mesh_3d);
	mesh3d_destroy(plane_mesh_3d);
	this->quadObject = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY_LAST));
	render_object_set_material(this->quadObject, this->depthPresentMaterial);
	render_object_attach(this->quadObject, this->quadMesh);
	render_object_set_transform(this->quadObject, mat4_mul(3, mat4_scale(0.6, 0.6, 0.6), mat4_rotation(0, -30 DEG, 72 DEG), mat4_translation(0.0f, 0.5f, 1.3f)));


	render_texture_create_info_t create_info = { .width = 100, .height = 100, .channel_count = 4, .depth = 1, .type = RENDER_TEXTURE_TYPE_DEPTH };
	this->depth_texture = render_texture_create(renderer, &create_info);
	create_info.type = RENDER_TEXTURE_TYPE_ALBEDO;
	this->color_texture = render_texture_create(renderer, &create_info);

	material_set_texture(this->depthPresentMaterial, "albedo", this->depth_texture);
	material_set_texture(this->colorPresentMaterial, "albedo", this->color_texture);

	this->angle = 0;
	this->angular_speed = 60 DEG;


	render_scene_build_queues(this->scene);

			render_object_set_material(this->quadObject, this->depthPresentMaterial);
			camera_set_render_target(this->offscreen_camera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, this->depth_texture);
}

TEST_ON_TERMINATE(TID_48_CASE_5)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->quadMesh);
	mesh_release_resources(this->quadMesh);
	render_texture_destroy(this->depth_texture);
	render_texture_release_resources(this->depth_texture);
	render_texture_destroy(this->color_texture);
	render_texture_release_resources(this->color_texture);
	light_destroy(this->light);
	light_release_resources(this->light);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TID_48_CASE_5)
{
	this->angle += this->angular_speed * deltaTime;
	render_object_set_transform(this->render_object, mat4_mul(2, mat4_translation(0, 0, -0.7f), mat4_rotation_y(this->angle)));

	if(kbhit())
	{
		char ch = getch();
		if(ch == 't')
			camera_set_render_target(this->offscreen_camera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED, CAMERA_RENDER_TARGET_SCREEN);
		else if(ch == 'y')
		{
			render_object_set_material(this->quadObject, this->depthPresentMaterial);
			camera_set_render_target(this->offscreen_camera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, this->depth_texture);
		}
		else if(ch == 'u')
		{
			camera_set_render_target(this->offscreen_camera, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED, CAMERA_RENDER_TARGET_SCREEN);
		}
		else if(ch == 'i')
		{
			render_object_set_material(this->quadObject, this->colorPresentMaterial);
			camera_set_render_target(this->offscreen_camera, CAMERA_RENDER_TARGET_TYPE_COLOR, CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED, this->color_texture);
		}
	}
}

TEST_ON_RENDER(TID_48_CASE_5)
{
	camera_set_active(this->offscreen_camera, true);
	camera_set_active(this->depth_present_camera, false);
	render_scene_render(this->scene, BIT64(RENDER_QUEUE_TYPE_GEOMETRY), RENDER_SCENE_CLEAR);
	camera_set_active(this->depth_present_camera, true);
	camera_set_active(this->offscreen_camera, false);
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

