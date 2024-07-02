/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-48.case4.c is a part of VulkanRenderer

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


#include <renderer/tests/TID-48.case4.h>

#define SGE_INCLUDE_EVERYTHING_INTERNAL
#define SGE_INCLUDE_MATH
#define SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#define SGE_INCLUDE_3D_LIGHT_SYSTEM
#define SGE_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/conio.h>

TEST_DATA(TID_48_CASE_4)
{
	render_scene_t* scene;

	/* cameras */
	camera_t* camera;
	camera_t* offscreenCamera;

	/* lights */
	light_t* pointLight;

	/* materials */
	material_t* material;
	material_t* shadowMapMaterial;
	material_t* depthReflectionMaterial;

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

SETUP_TEST(TID_48_CASE_4);

TEST_ON_SGE_INITIALIZE(TID_48_CASE_4)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_48_CASE_4",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_48_CASE_4)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create cameras */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_WHITE, 1.0f);
	camera_set_active(this->camera, true);
	
	this->offscreenCamera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->offscreenCamera, COLOR_RED, 1.0f);
	camera_set_position(this->offscreenCamera, vec3_zero());
	camera_set_rotation(this->offscreenCamera, vec3(0, 0, 0));
	camera_set_field_of_view(this->offscreenCamera, 90 DEG);
	camera_set_active(this->offscreenCamera, false);

	/* create a render scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_QUEUE0));
	/* add the cameras into the render scene */
	render_scene_add_camera(this->scene, this->camera);
	render_scene_add_camera(this->scene, this->offscreenCamera);
	
	this->pointLight = light_create(renderer, LIGHT_TYPE_POINT);
	light_set_position(this->pointLight, vec3_zero());
	light_set_intensity(this->pointLight, 0.5f);

	this->shadowMapMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/shadowmap.sb"), "ShadowMapMaterial"));
	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/point_light.sb"), "PointLightMaterial"));
	this->depthReflectionMaterial = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/presets/reflection.depth.point.sb"), "DepthRefectionMaterial"));

	material_set_vec4(this->material, "parameters.color", vec4(1, 1, 1, 1));
	material_set_vec4(this->depthReflectionMaterial, "parameters.color", vec4(1, 1, 1, 1));
	material_set_float(this->depthReflectionMaterial, "parameters.reflectance", 1.0f);

	vulkan_texture_create_info_t create_info = 
	{
		.width = 800,
		.height = 800,
		.depth = 1,
		.channel_count = 4,
		.type = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET ,
		.initial_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.usage = VULKAN_TEXTURE_USAGE_RENDER_TARGET,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED,
		.technique = VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH 
	};	
	this->shadowMap = vulkan_texture_create(renderer->vulkan_handle, &create_info);
	camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE, this->shadowMap);
	material_set_texture(this->depthReflectionMaterial, "reflectionMap", this->shadowMap);
	material_set_texture(this->material, "shadowMap", this->shadowMap);

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

TEST_ON_TERMINATE(TID_48_CASE_4)
{
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->wallMesh);
	mesh_release_resources(this->wallMesh);
	mesh_destroy(this->cubeMesh);
	mesh_release_resources(this->cubeMesh);
	light_destroy(this->pointLight);
	light_release_resources(this->pointLight);

	vulkan_texture_destroy(this->shadowMap);
	vulkan_texture_release_resources(this->shadowMap);

	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


UNUSED_VARIABLE static bool swap = false;
static float angle = -90.0f;

TEST_ON_UPDATE(TID_48_CASE_4)
{
	if(kbhit())
	{
		char ch = getch();
		// swap = !swap;
		// if(swap)
		// 	camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, CAMERA_RENDER_TARGET_SCREEN);
		// else 
		// 	camera_set_render_target(this->offscreenCamera, CAMERA_RENDER_TARGET_TYPE_DEPTH, this->shadowMap);
		switch(ch)
		{
		case 'g':
			event_publish(vulkan_renderer_get_window(renderer->vulkan_handle)->on_resize_event);
			break;
		case 'f':
			angle += 10.0f;
			debug_log_info("angle: %d", ((s32)angle) % 360);
		}
		// angle += 10.0f;
	}

	// angle += deltaTime * 90.0f;
	// if(angle > 360.0f)
	// 	angle = 0;
	// angle = -90.0f;
	float _angle = angle;// (0.5f * sin(angle) + 0.5f) * 180.0f - 180.0f;
	vec3_t pos = vec3(0.8f * sin(_angle DEG), 0, 0.8f * cos(_angle DEG));
	vulkan_camera_set_position_cube(this->offscreenCamera, pos);
	light_set_position(this->pointLight, pos);
	light_set_color(this->pointLight, vec3(pos.x * 0.5f + 0.5f, pos.y * 0.5f + 0.5f, pos.z * 0.5f + 0.5f));
	render_object_set_transform(this->cubeObject, mat4_mul(2, mat4_translation(pos.x, pos.y, pos.z),
		mat4_scale(0.1f, 0.1f, 0.1f)));
}

TEST_ON_RENDER(TID_48_CASE_4)
{
	render_object_set_material(this->renderObject, this->shadowMapMaterial);
	render_object_set_material(this->wallObject, this->shadowMapMaterial);
	render_object_set_material(this->cubeObject, this->shadowMapMaterial);
	camera_set_active(this->offscreenCamera, true);
	camera_set_active(this->camera, false);
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);

	render_object_set_material(this->renderObject, this->material);
	render_object_set_material(this->wallObject, this->material);
	render_object_set_material(this->cubeObject, this->material);
	camera_set_active(this->offscreenCamera, false);
	camera_set_active(this->camera, true);
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

