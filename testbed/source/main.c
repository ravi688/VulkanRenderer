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

bool char_comparer(void* ptr1, void* ptr2)
{
	return (*(char*)ptr1) == (*(char*)ptr2);
}

static void u32_to_string(u32 value, char* string)
{
	sprintf(string, "%u", value);
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
	font_t* font = font_load_and_create("resource/fonts/Pushster-Regular.ttf");
	glyph_mesh_pool_t* pool = glyph_mesh_pool_create(renderer, font);
	mesh_t* glyph_A = glyph_mesh_pool_get_mesh(pool, 'A');
	mesh_t* glyph_B = glyph_mesh_pool_get_mesh(pool, 'B');
	text_mesh_t* text_mesh = text_mesh_create(renderer, pool);
	text_mesh_string_handle_t fps_string_handle = text_mesh_string_create(text_mesh);
	text_mesh_string_set_positionH(text_mesh, fps_string_handle, vec3(float)(0, 0, -3));
	text_mesh_string_handle_t module_name_handle = text_mesh_string_create(text_mesh);
	text_mesh_string_set_positionH(text_mesh, module_name_handle, vec3(float)(0, 2, -3));
	text_mesh_string_set_scaleH(text_mesh, module_name_handle, vec3(float)(1.5f, 1.5f, 1.5f));
	text_mesh_string_setH(text_mesh, module_name_handle, "Hello World");
	text_mesh_string_handle_t version_name_handle = text_mesh_string_create(text_mesh);
	text_mesh_string_set_positionH(text_mesh, version_name_handle, vec3(float)(0, -2, -3));
	text_mesh_string_set_scaleH(text_mesh, version_name_handle, vec3(float)(0.7f, 0.7f, 0.7f));
	text_mesh_string_setH(text_mesh, version_name_handle, "v1.0.0.0.0");
	text_mesh_string_handle_t cpu_usage = text_mesh_string_create(text_mesh);
	text_mesh_string_set_positionH(text_mesh, cpu_usage, vec3(float)(0, -4, -3));
	text_mesh_string_set_scaleH(text_mesh, cpu_usage, vec3(float)(0.7f, 0.7f, 0.7f));
	text_mesh_string_setH(text_mesh, cpu_usage, "10.99999%");

	shader_t* text_shader = shader_load(renderer, "resource/shaders/text_shader.sb");
	u64 per_vertex_attributes[4] = { MATERIAL_ALIGN(MATERIAL_VEC3, 0) };
	u64 per_instance_attributes[1] = { MATERIAL_ALIGN(MATERIAL_VEC3, 0) | MATERIAL_ALIGN(MATERIAL_VEC3, 1) | MATERIAL_ALIGN(MATERIAL_VEC3, 2) };
	material_create_info_t text_material_info =
	{
		.per_vertex_attribute_binding_count = 1,
		.per_vertex_attribute_bindings = &per_vertex_attributes[0],
		.per_instance_attribute_binding_count = 1,
		.per_instance_attribute_bindings = &per_instance_attributes[0],
		.shader = text_shader,
	};
	material_t* text_material = material_create(renderer, &text_material_info);
	/*---------------------------------------*/

	/*-----------GAME UI----------------------*/
	shader_t* game_ui_shader = shader_load(renderer, "resource/shaders/game_ui_shader.sb");
	text_material_info.shader = game_ui_shader;
	material_t* game_ui_material = material_create(renderer, &text_material_info);

	text_mesh_t* game_ui = text_mesh_create(renderer, pool);
	text_mesh_string_handle_t score_text = text_mesh_string_create(game_ui);
	text_mesh_string_set_positionH(game_ui, score_text, vec3(float)(0, 7, 5));
	text_mesh_string_set_scaleH(game_ui, score_text, vec3(float)(0.8f, 0.8f, 0.8f));

	/*--------PLAYGROUND----------------------*/
	/*---------------------------------------*/

	/*------CUBE-----------------------------*/
	mesh3d_t* cube_mesh3d = mesh3d_cube(1);
	mesh_t* cube = mesh_create(renderer, cube_mesh3d);
	texture_t* linux_texture = texture_load(renderer, "resource/textures/linuxlogo.bmp");
	texture_t* windows_texture = texture_load(renderer, "resource/textures/windowslogo.bmp");
	texture_t* apple_texture = texture_load(renderer, "resource/textures/applelogo.bmp");

	shader_t* albedo_shader = shader_load(renderer, "resource/shaders/albedo_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); //position
	per_vertex_attributes[1] = MATERIAL_ALIGN(MATERIAL_VEC3, 1); //normal
	per_vertex_attributes[2] = MATERIAL_ALIGN(MATERIAL_VEC3, 2); //color
	per_vertex_attributes[3] = MATERIAL_ALIGN(MATERIAL_VEC2, 3); //texture coordinates
	material_create_info_t cube_material_info =
	{
		.per_vertex_attribute_binding_count = 4,
		.per_vertex_attribute_bindings = &per_vertex_attributes[0],
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
		mat4_t(float) model_matrix = mat4_rotation(float)(0, sin(angle * DEG2RAD) * 1.57f, 0);
		material_set_floatH(cube_material, handle, game_time);
		material_set_uint(cube_material, "scene_data.value", 2);
		material_set_vec3(cube_material, "scene_data.green_color", vec3(float)(1, 1, 1));
		material_set_vec3(cube_material, "light.dir", vec3(float)(0, -1, 0));
		material_set_float(cube_material, "light.intensity", 1.0f);
		material_set_float(text_material, "ubo.time", game_time);
		material_set_float(game_ui_material, "ubo.time", game_time);

		renderer_begin_frame(renderer, 0.1f, 0.1f, 0.3f, 0);

		material_bind(cube_material, renderer);
		mat4_t(float) mvp = mat4_mul(float)(4, clip_matrix, projection_matrix, view_matrix, model_matrix);
		mat4_move(float)(&mvp, mat4_transpose(float)(mvp));
		material_push_constants(cube_material, renderer, &mvp);
		mesh_draw_indexed(cube, renderer);

		material_bind(text_material, renderer);
		mat4_t(float) canvas_transform = mat4_mul(float)(2, clip_matrix, screen_space_matrix);
		mat4_t(float) _model_matrix = mat4_mul(float)(2, mat4_translation(float)(0, 0, 0), mat4_scale(float)(0, 50, 50));
		mat4_move(float)(&canvas_transform, mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _model_matrix)));
		material_push_constants(text_material, renderer, &canvas_transform);
		text_mesh_draw(text_mesh);


		material_bind(game_ui_material, renderer);
		material_push_constants(game_ui_material, renderer, &canvas_transform);
		text_mesh_draw(game_ui);
		// mesh_draw_indexed(glyph_A, renderer);
		// mesh_draw_indexed(glyph_B, renderer);

		renderer_end_frame(renderer);

		renderer_update(renderer);
		angle += 180 * delta_time;
		if(angle >= 360.0f)
			angle = 0.0f;

		float seconds = time_get_seconds(second_time_handle);
		if(seconds >= 0.05f)
		{
			u32 fps = (u32)((int)frame_count / seconds);
			char fps_string[32];
			strcpy(fps_string, "FPS: ");
			u32_to_string(fps, fps_string + strlen("FPS: "));
			text_mesh_string_setH(text_mesh, fps_string_handle, fps_string);
			strcpy(fps_string, "Score: ");
			u32_to_string(frame_count, fps_string + strlen("Score: "));
			text_mesh_string_setH(game_ui, score_text, fps_string);
			// printf("FPS: %u, TIME: %.3f\n", fps, game_time);
			second_time_handle = time_get_handle();
			frame_count = 0;
			texture_index++;
			texture_index = texture_index % 3;
			// switch(texture_index)
			// {
			// 	case 0: material_set_texture2d(cube_material, "texture", linux_texture); break;
			// 	case 1: material_set_texture2d(cube_material, "texture", windows_texture); break;
			// 	case 2: material_set_texture2d(cube_material, "texture", apple_texture); break;
			// 	default:
			// 		assert(false);
			// }
		}
		++frame_count;
	}


	glyph_mesh_pool_destroy(pool);
	glyph_mesh_pool_release_resources(pool);

	text_mesh_destroy(game_ui);
	text_mesh_release_resources(game_ui);

	shader_destroy(game_ui_shader, renderer);
	shader_release_resources(game_ui_shader);
	material_destroy(game_ui_material, renderer);
	material_release_resources(game_ui_material);

	font_destroy(font);
	font_release_resources(font);
	text_mesh_destroy(text_mesh);
	text_mesh_release_resources(text_mesh);
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
