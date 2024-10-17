/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: text_mesh.c is a part of VulkanRenderer

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
	test id: TRANSPARENT_QUEUE
	Test to render a rectangle with rounded corners using Signed Distance Function
 */

#include <sge/tests/transparent_queue.h>

#define SGE_INCLUDE_MATH
#define SGE_INCLUDE_CORE
#define SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#include <sge/sge.h>
#include <sge/conio.h>

TEST_DATA(TRANSPARENT_QUEUE)
{
	render_scene_t* scene;

	camera_t* camera;

	mesh_t* mesh;
	mesh_t* mesh2;
	render_object_t* sdf_object;
	render_object_t* sdf_object2;
	render_object_t* sdf_object3;
	render_object_t* sdf_object4;
	render_object_t* sdf_object5;
	material_t* sdf_material;
	material_t* sdf_material2;
	material_t* material3;
	material_t* material4;
	texture_t* texture;
	texture_t* texture2;
};


SETUP_TEST(TRANSPARENT_QUEUE);

TEST_ON_SGE_INITIALIZE(TRANSPARENT_QUEUE)
{
	return (renderer_initialization_data_t)
	{
		.gpu_type = SGE_GPU_TYPE_INTEGRATED,
		.window_name = "TRANSPARENT_QUEUE",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TRANSPARENT_QUEUE)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create camera */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_BLUE, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_transform(this->camera, mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

	/* create render scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY) | BIT64(RENDER_QUEUE_TYPE_TRANSPARENT));
	/* add the camera into the render scene */
	render_scene_add_camera(this->scene, this->camera);

	AUTO shader = shader_library_load_shader(slib, "shaders/presets/unlit_texture.ui.sb");

	this->sdf_material = material_library_getH(mlib, material_library_create_materialH(mlib, shader, "SDFRoundedRectMaterial1"));

	this->texture = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Thalita-Torres-Office-Archive-folders.512.png");
	material_set_vec4(this->sdf_material, "parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	material_set_texture(this->sdf_material, "albedo", this->texture);

	AUTO planeMeshData2 = mesh3d_plane(renderer->allocator, 1);
	mesh3d_transform_set(planeMeshData2, mat4_rotation_z(90 DEG));
	this->mesh = mesh_create(renderer, planeMeshData2);

	this->sdf_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_TRANSPARENT));
	render_object_set_material(this->sdf_object, this->sdf_material);
	render_object_attach(this->sdf_object, this->mesh);
	// NOTE: Since '45 DEG' is always less than the most precise PI / 2, the quad/plane is not vertical w.r.t x-y plane
	// Therefore, fragments in the bottom part aren't visible because of clipping (they lie past the near clip plane).
	// Thus, to avoid this artefact, we need to move the plane towards +x axis by 1.0f units.
	render_object_set_transform(this->sdf_object, mat4_mul(2, mat4_translation(1.0f, 200.0f, 0.0f), mat4_scale(200, 200, 200)));

	this->sdf_material2 = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_load_shader(slib, "shaders/presets/unlit_color.ui.sb"),
							"SDFCircleShader"));

	material_set_vec4(this->sdf_material2, "parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));

	this->mesh2 = mesh_create(renderer, planeMeshData2);
	mesh3d_destroy(planeMeshData2);

	this->sdf_object2 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->sdf_object2, this->sdf_material2);
	render_object_attach(this->sdf_object2, this->mesh2);
	// NOTE: Since '45 DEG' is always less than the most precise PI / 2, the quad/plane is not vertical w.r.t x-y plane
	// Therefore, fragments in the bottom part aren't visible because of clipping (they lie past the near clip plane).
	// Thus, to avoid this artefact, we need to move the plane towards +x axis by 1.0f units.
	render_object_set_transform(this->sdf_object2, mat4_mul(2, mat4_translation(2.0f, 100.0f, 0.0f), mat4_scale(200, 200, 300)));


	this->sdf_object3 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_TRANSPARENT));
	render_object_set_material(this->sdf_object3, this->sdf_material);
	render_object_attach(this->sdf_object3, this->mesh2);
	// NOTE: Since '45 DEG' is always less than the most precise PI / 2, the quad/plane is not vertical w.r.t x-y plane
	// Therefore, fragments in the bottom part aren't visible because of clipping (they lie past the near clip plane).
	// Thus, to avoid this artefact, we need to move the plane towards +x axis by 1.0f units.
	render_object_set_transform(this->sdf_object3, mat4_mul(2, mat4_translation(1.5f, 200.0f, 50.0f), mat4_scale(200, 200, 300)));


	this->texture2 = texture_load(renderer, TEXTURE_TYPE_ALBEDO, "textures/Folder.png");
	this->material3 = material_library_getH(mlib, material_library_create_materialH(mlib, shader, "SDFRoundedRectMaterial2"));
	material_set_vec4(this->material3, "parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	material_set_texture(this->material3, "albedo", this->texture2);
	this->sdf_object4 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_TRANSPARENT));
	render_object_set_material(this->sdf_object4, this->material3);
	render_object_attach(this->sdf_object4, this->mesh2);
	// NOTE: Since '45 DEG' is always less than the most precise PI / 2, the quad/plane is not vertical w.r.t x-y plane
	// Therefore, fragments in the bottom part aren't visible because of clipping (they lie past the near clip plane).
	// Thus, to avoid this artefact, we need to move the plane towards +x axis by 1.0f units.
	render_object_set_transform(this->sdf_object4, mat4_mul(2, mat4_translation(1.75f, 200.0f, -50.0f), mat4_scale(200, 200, 300)));

	AUTO shader2 = shader_library_load_shader(slib, "shaders/presets/unlit_texture.ui.sb");
	this->material4 = material_library_getH(mlib, material_library_create_materialH(mlib, shader2, "SDFRoundedRectMaterial2"));
	material_set_vec4(this->material4, "parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
	material_set_texture(this->material4, "albedo", this->texture2);
	this->sdf_object5 = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_MESH, RENDER_QUEUE_TYPE_TRANSPARENT));
	render_object_set_material(this->sdf_object5, this->material4);
	render_object_attach(this->sdf_object5, this->mesh2);
	// NOTE: Since '45 DEG' is always less than the most precise PI / 2, the quad/plane is not vertical w.r.t x-y plane
	// Therefore, fragments in the bottom part aren't visible because of clipping (they lie past the near clip plane).
	// Thus, to avoid this artefact, we need to move the plane towards +x axis by 1.0f units.
	render_object_set_transform(this->sdf_object5, mat4_mul(2, mat4_translation(1.85f, 150.0f, -50.0f), mat4_scale(200, 200, 300)));

	render_scene_build_queues(this->scene);
}

TEST_ON_TERMINATE(TRANSPARENT_QUEUE)
{
	texture_destroy(this->texture);
	texture_release_resources(this->texture);
	texture_destroy(this->texture2);
	texture_release_resources(this->texture2);
	mesh_destroy(this->mesh);
	mesh_release_resources(this->mesh);
	mesh_destroy(this->mesh2);
	mesh_release_resources(this->mesh2);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TRANSPARENT_QUEUE)
{
}

TEST_ON_RENDER(TRANSPARENT_QUEUE)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}
