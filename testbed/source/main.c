//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/render_window.h>
#include <renderer/time.h>

//For handling text/font rendering
#include <renderer/font.h>
#include <renderer/text_mesh.h>
// #include <renderer/text.h>

#include <renderer/mesh3d.h>
#include <renderer/defines.h>
#include <renderer/assert.h>
#include <renderer/Serialization.h>

#include <memory_allocator/memory_allocator.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>
#include <hpml/mat2/header_config.h>
#include <hpml/mat2/mat2.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

#include <ttf2mesh.h>

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }

static mat4_t(float) projection_matrix;
static mat4_t(float) screen_space_matrix;

static void recreate_matrix(render_window_t* window, void* user_data)
{
	mat4_move(float)(&projection_matrix,  mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)window->width/window->height));
	// mat4_move(float)(&projection_matrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
	mat4_move(float)(&screen_space_matrix, mat4_ortho_projection(float)(0, 100, window->height, (float)window->width / window->height));
}

static void offset_visitor(vec3_t(float)* position, void* user_data)
{
	*position = vec3_mul(float)(*position, vec3(float)(0, 100, 100));
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);

	renderer_t* renderer = renderer_init(800, 800, "Vulkan 3D Renderer", false);
	recreate_matrix(renderer_get_window(renderer), NULL);
	render_window_subscribe_on_resize(renderer_get_window(renderer), recreate_matrix, NULL);
	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -3, 1, 0 }, (vec3_t(float)) { 0, 0, -20 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) clip_matrix = mat4_identity(float)(); clip_matrix.m11 = -1;

	/*------TEXT-----------------------------*/
	shader_t* text_shader = shader_load(renderer, "resource/shaders/text_shader.sb");
	font_t* font = font_load_and_create("resource/fonts/arial.ttf");
	text_mesh_t* text = text_mesh_create(font);
	text_mesh_set_string(text, renderer, "Vulkan 3D Renderer");
	// text_mesh_set_size(text, 100);

	material_create_info_t text_material_info =
	{
		.per_vertex_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0), //position
		.per_instance_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0), //offset
		.shader = text_shader,
	};
	material_t* text_material = material_create(renderer, &text_material_info);
	/*---------------------------------------*/

	/*------CUBE-----------------------------*/
	shader_t* albedo_shader = shader_load(renderer, "resource/shaders/albedo_shader.sb");
	mesh3d_t* cube_mesh3d = mesh3d_cube(1);
	mesh_t* cube = mesh_create(renderer, cube_mesh3d);
	texture_t* linux_texture = texture_load(renderer, "resource/textures/linuxlogo.bmp");
	texture_t* windows_texture = texture_load(renderer, "resource/textures/windowslogo.bmp");
	texture_t* apple_texture = texture_load(renderer, "resource/textures/applelogo.bmp");
	material_create_info_t cube_material_info =
	{
		.per_vertex_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0) //position
								| MATERIAL_ALIGN(MATERIAL_VEC3, 1) //normal
								| MATERIAL_ALIGN(MATERIAL_VEC3, 2) //color
								| MATERIAL_ALIGN(MATERIAL_VEC2, 3), //texture coordinates
		.shader = albedo_shader,
	};
	material_t* cube_material = material_create(renderer, &cube_material_info);

	material_set_texture2d(cube_material, "texture", linux_texture);
	material_set_texture2d(cube_material, "texture2", windows_texture);
	/*---------------------------------------*/

	time_handle_t frame_time_handle = time_get_handle();
	time_handle_t second_time_handle = time_get_handle();
	time_handle_t game_time_handle = time_get_handle();
	u32 frame_count = 0;
	float angle = 0;
	u32 texture_index = 0;
	material_field_handle_t handle = material_get_field_handle(cube_material, "scene_data.time");
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		float delta_time = time_get_delta_time(&frame_time_handle);
		float game_time = time_get_seconds(game_time_handle);
		mat4_t(float) model_matrix = mat4_rotation(float)(0, angle * DEG2RAD, 0);
		material_set_floatH(cube_material, handle, game_time);
		material_set_uint(cube_material, "scene_data.value", 2);
		material_set_vec3(cube_material, "scene_data.green_color", vec3(float)(1, 1, 0));
		material_set_vec3(cube_material, "light.dir", vec3(float)(0, -1, 0));
		material_set_float(cube_material, "light.intensity", 1.0f);

		renderer_begin_frame(renderer, 0, 0, 0, 0);

		material_bind(cube_material, renderer);
		mat4_t(float) mvp = mat4_mul(float)(4, clip_matrix, projection_matrix, view_matrix, model_matrix);
		mat4_move(float)(&mvp, mat4_transpose(float)(mvp));
		material_push_constants(cube_material, renderer, &mvp);
		mesh_draw_indexed(cube, renderer);

		material_bind(text_material, renderer);
		mat4_t(float) canvas_transform = mat4_mul(float)(2, clip_matrix, screen_space_matrix);
		mat4_t(float) _model_matrix = mat4_mul(float)(2, mat4_translation(float)(0, -(int)renderer_get_window(renderer)->height * 0.5f + 40, -250), mat4_scale(float)(0, 50, 50));
		mat4_move(float)(&canvas_transform, mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _model_matrix)));
		material_push_constants(text_material, renderer, &canvas_transform);
		text_mesh_draw(text, renderer);

		renderer_end_frame(renderer);

		renderer_update(renderer);
		angle += 180 * delta_time;
		if(angle >= 360.0f)
			angle = 0.0f;

		float seconds = time_get_seconds(second_time_handle);
		if(seconds >= 1)
		{
			printf("FPS: %u, TIME: %.3f\n", (int)((int)frame_count / seconds), game_time);
			second_time_handle = time_get_handle();
			frame_count = 0;
			texture_index++;
			texture_index = texture_index % 3;
			switch(texture_index)
			{
				case 0: material_set_texture2d(cube_material, "texture", linux_texture); break;
				case 1: material_set_texture2d(cube_material, "texture", windows_texture); break;
				case 2: material_set_texture2d(cube_material, "texture", apple_texture); break;
				default:
					assert(false);
			}
		}
		++frame_count;
	}

	font_destroy(font);
	font_release_resources(font);
	text_mesh_destroy(text, renderer);
	text_mesh_release_resources(text);
	shader_destroy(text_shader, renderer);
	shader_release_resources(text_shader);
	material_destroy(text_material, renderer);
	material_release_resources(text_material);

	mesh_destroy(cube, renderer);
	mesh_release_resources(cube);
	mesh3d_destroy(cube_mesh3d);
	shader_destroy(albedo_shader, renderer);
	shader_release_resources(albedo_shader);
	material_destroy(cube_material, renderer);
	material_release_resources(cube_material);
	texture_destroy(linux_texture, renderer);
	texture_release_resources(linux_texture);
	texture_destroy(windows_texture, renderer);
	texture_release_resources(windows_texture);
	texture_destroy(apple_texture, renderer);
	texture_release_resources(apple_texture);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
