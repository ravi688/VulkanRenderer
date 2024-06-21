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
	test id: TEXT_MESH
	Test to check if text mesh is working fine
 */

#include <renderer/tests/text_mesh.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/conio.h>

TEST_DATA(TEXT_MESH)
{
	render_scene_t* scene;

	camera_t* camera;

	glyph_mesh_pool_t* glyph_pool;
	font_t* font;

	text_mesh_t* text;
	text_mesh_string_handle_t text_string_handle;
	text_mesh_string_handle_t another_string_handle;

	render_object_t* text_object;
	material_t* text_material;
};


SETUP_TEST(TEXT_MESH);

TEST_ON_RENDERER_INITIALIZE(TEXT_MESH)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TEXT_MESH (press any key to switch from Screen space to World space and vice versa",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TEXT_MESH)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	/* create camera */
	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_BLACK, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_transform(this->camera, mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

	/* create render scene */
	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
	/* add the camera into the render scene */
	render_scene_add_camera(this->scene, this->camera);

	this->text_material = material_library_getH(mlib,
							material_library_create_materialH(mlib,
							shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH),
							"TextMeshShader"));

	this->font = font_load_and_create(renderer, "showcase/resource/fonts/arial.ttf");
	font_set_char_size(this->font, 24);

	/* glyph mesh pool */
	this->glyph_pool = glyph_mesh_pool_create(renderer, this->font);

	material_set_float(this->text_material, "parameters.color.r", 1.0f);
	material_set_float(this->text_material, "parameters.color.g", 1.0f);
	material_set_float(this->text_material, "parameters.color.b", 1.0f);
	this->text = text_mesh_create(renderer, this->glyph_pool);
	this->text_string_handle = text_mesh_string_create(this->text);
	this->another_string_handle = text_mesh_string_create(this->text);
	text_mesh_string_set_point_sizeH(this->text, this->another_string_handle, 24);
	text_mesh_string_set_point_sizeH(this->text, this->text_string_handle, 35);
	text_mesh_string_setH(this->text, this->another_string_handle, "Hardwork with dedication suffices to c");
	text_mesh_string_setH(this->text, this->text_string_handle, "Hardwork with dedication suffices to c: e141234324");

	this->text_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_TEXT_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->text_object, this->text_material);
	render_object_attach(this->text_object, this->text);
	text_mesh_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 400.0f, -400.0f));

	render_scene_build_queues(this->scene);
}

TEST_ON_TERMINATE(TEXT_MESH)
{
	text_mesh_destroy(this->text);
	text_mesh_release_resources(this->text);
	font_destroy(this->font);
	font_release_resources(this->font);
	glyph_mesh_pool_destroy(this->glyph_pool);
	glyph_mesh_pool_release_resources(this->glyph_pool);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TEXT_MESH)
{
	static bool isScreenSpace = true;
	if(kbhit())
	{
		char ch = getch();
		if((ch != 'p') && (ch != 'q'))
		{
			isScreenSpace = !isScreenSpace;
			if(isScreenSpace)
			{
				text_mesh_set_render_space_type(this->text, TEXT_MESH_RENDER_SPACE_TYPE_2D);
				debug_log_info("TEXT_MESH_RENDER_SPACE_TYPE_2D");
				render_object_set_transform(this->text_object, mat4_identity());
				text_mesh_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 400.0f, -400.0f));
			}
			else
			{
				text_mesh_set_render_space_type(this->text, TEXT_MESH_RENDER_SPACE_TYPE_3D);
				debug_log_info("TEXT_MESH_RENDER_SPACE_TYPE_3D");
				render_object_set_transform(this->text_object, mat4_mul(3, mat4_rotation_y(45 DEG), mat4_scale(0.005f, 0.005f, 0.005f), mat4_translation(0.0f, 0.0f, -0.5f)));
				text_mesh_string_set_transformH(this->text, this->text_string_handle, mat4_translation(0.0f, 30.0f, 0.0f));
			}
		}
		else if(ch == 'p')
		{
			text_mesh_string_set_point_sizeH(this->text, this->another_string_handle, text_mesh_string_get_point_sizeH(this->text, this->another_string_handle) * 2);
			text_mesh_string_set_point_sizeH(this->text, this->text_string_handle, text_mesh_string_get_point_sizeH(this->text, this->text_string_handle) * 2);
		}
		else if(ch == 'q')
		{
			text_mesh_string_set_point_sizeH(this->text, this->another_string_handle, text_mesh_string_get_point_sizeH(this->text, this->another_string_handle) / 2);
			text_mesh_string_set_point_sizeH(this->text, this->text_string_handle, text_mesh_string_get_point_sizeH(this->text, this->text_string_handle) / 2);
		}
	}

	static int counter = 0;
	counter++;
	if(counter == 66000)
	counter = 0;
	char buffer[128] =  { };
	sprintf(buffer, "%d", counter);
	text_mesh_string_setH(this->text, this->text_string_handle, buffer);
	sprintf(buffer, "%d", counter);
	text_mesh_string_setH(this->text, this->another_string_handle, buffer);
}

TEST_ON_RENDER(TEXT_MESH)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}
