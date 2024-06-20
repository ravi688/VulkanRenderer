/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: texture_sampling_alpha_channel.c is a part of VulkanRenderer

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
	test id: TEXTURE_SAMPLING_ALPHA_CHANNEL
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

TEST_DATA(TEXTURE_SAMPLING_ALPHA_CHANNEL)
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

SETUP_TEST(TEXTURE_SAMPLING_ALPHA_CHANNEL);

TEST_ON_RENDERER_INITIALIZE(TEXTURE_SAMPLING_ALPHA_CHANNEL)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TEXTURE_SAMPLING_ALPHA_CHANNEL",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TEXTURE_SAMPLING_ALPHA_CHANNEL)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	this->light = light_create(renderer, LIGHT_TYPE_DIRECTIONAL);

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/builtins/texture_sample.sb"), "MyMaterial"));

	s32 num_divisons = 8;
	u8 data[512 * 512];
	for(s32 i = 0; i < 512; i++)
	{
		for(s32 j = 0; j < 512; j++)
		{
			s32 size = 512 / num_divisons;
			s32 y = (i / size) * size;
			s32 x = (j / size) * size;

			if(((i >= x) && (i < (size + x))) && ((j >= y) < (size + y)))
				data[i * 512 + j] = 255;
			else
				data[i * 512 + j] = 0;
		}
	}

	/* create alpha channel texture */
	texture_create_info_t create_info = 
	{
		.width = 512,
		.height = 512,
		.depth = 1,
		.channel_count = 1,
		.type = TEXTURE_TYPE_ALBEDO,
		.initial_usage = TEXTURE_USAGE_TRANSFER_DST,
		.usage = TEXTURE_USAGE_SAMPLED,
		.final_usage = TEXTURE_USAGE_SAMPLED
	};
 	this->texture = texture_create(renderer, &create_info);
 	
 	/* upload alpha mask data to the texture */
 	texture_data_t texture_data =
 	{
 		.data = (void*)data,
 		.width = 512,
 		.height = 512,
 		.depth = 1,
 		.channel_count = 1
 	};
 	texture_upload_data(this->texture, 1, &texture_data);

 	/* make this texture ready to be used (sampling) */
 	texture_set_usage_stage(this->texture, TEXTURE_USAGE_STAGE_FINAL);

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

TEST_ON_TERMINATE(TEXTURE_SAMPLING_ALPHA_CHANNEL)
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


TEST_ON_UPDATE(TEXTURE_SAMPLING_ALPHA_CHANNEL)
{
}

TEST_ON_RENDER(TEXTURE_SAMPLING_ALPHA_CHANNEL)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

