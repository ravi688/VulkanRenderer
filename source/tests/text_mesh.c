
/*
	test id: TEXT_MESH
	Test to check if text mesh is working fine
 */

#include <renderer/tests/text_mesh.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

TEST_DATA(TEXT_MESH)
{
	render_scene_t* scene;

	camera_t* camera;

	glyph_mesh_pool_t* glyph_pool;
	font_t* font;

	text_mesh_t* text_mesh;
	text_mesh_string_handle_t string_handle;

	render_object_t* render_object;
	material_t* material;

	int isScreenSpace; /* bools are not support currently in V3DShader (see: TID64)*/
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

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_transform(this->camera, mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/builtins/text_shader.sb"), "MyMaterial"));
	this->isScreenSpace = 1;
	material_set_int(this->material, "parameters.isScreenSpace", 1);

	this->font = font_load_and_create(renderer->allocator, "showcase/resource/fonts/arial.ttf");
	this->glyph_pool = glyph_mesh_pool_create(renderer, this->font);
	this->text_mesh = text_mesh_create(renderer, this->glyph_pool);
	this->string_handle = text_mesh_string_create(this->text_mesh);
	text_mesh_string_setH(this->text_mesh, this->string_handle, "Screen Space");

	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_TEXT_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->text_mesh);
	render_object_set_transform(this->render_object, mat4_translation(0.0f, 0.0f, -3.0f));

	render_scene_build_queues(this->scene);

}

TEST_ON_TERMINATE(TEXT_MESH)
{
	text_mesh_destroy(this->text_mesh);
	text_mesh_release_resources(this->text_mesh);
	font_destroy(this->font);
	font_release_resources(this->font);
	glyph_mesh_pool_destroy(this->glyph_pool);
	glyph_mesh_pool_release_resources(this->glyph_pool);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(TEXT_MESH)
{
	if(kbhit())
	{
		getch();
		this->isScreenSpace = !this->isScreenSpace;
		material_set_int(this->material, "parameters.isScreenSpace", this->isScreenSpace);
		switch(this->isScreenSpace)
		{
			case 0:
				text_mesh_string_setH(this->text_mesh, this->string_handle, "World Space");
				break;
			case 1:
				text_mesh_string_setH(this->text_mesh, this->string_handle, "Screen Space");
				break;
		}
	}
}

TEST_ON_RENDER(TEXT_MESH)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

