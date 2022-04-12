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

#include <renderer/memory_allocator.h>
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
	mat4_move(float)(&projection_matrix,  mat4_persp_projection(float)(0.04f, 100, 60 * DEG2RAD, (float)window->width/window->height));
	// mat4_move(float)(&projection_matrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
	mat4_move(float)(&screen_space_matrix, mat4_ortho_projection(float)(-0.04f, 100, window->height, (float)window->width / window->height));
}

static void u32_to_string(u32 value, char* string)
{
	sprintf(string, "%u", value);
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);

	renderer_t* renderer = renderer_init(RENDERER_GPU_TYPE_DISCRETE, 800, 800, "Vulkan 3D Renderer", false);
	recreate_matrix(renderer_get_window(renderer), NULL);
	render_window_subscribe_on_resize(renderer_get_window(renderer), recreate_matrix, NULL);
	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -1.8f, 0.6f, 0 }, (vec3_t(float)) { 0, 0, -22 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) clip_matrix = mat4_identity(float)(); clip_matrix.m11 = -1;


	/*------TEXT-----------------------------*/
	font_t* font = font_load_and_create("showcase/resource/fonts/Pushster-Regular.ttf");
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

	shader_t* text_shader = shader_load(renderer, "showcase/resource/shaders/text_shader.sb");
	u64 per_vertex_attributes[5] = { MATERIAL_ALIGN(MATERIAL_VEC3, 0) };
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
	shader_t* game_ui_shader = shader_load(renderer, "showcase/resource/shaders/game_ui_shader.sb");
	text_material_info.shader = game_ui_shader;
	material_t* game_ui_material = material_create(renderer, &text_material_info);

	text_mesh_t* game_ui = text_mesh_create(renderer, pool);
	text_mesh_string_handle_t score_text = text_mesh_string_create(game_ui);
	text_mesh_string_set_positionH(game_ui, score_text, vec3(float)(0, 7, 5));
	text_mesh_string_set_scaleH(game_ui, score_text, vec3(float)(0.8f, 0.8f, 0.8f));

	/*--------PLAYGROUND----------------------*/
	mesh3d_t* rock_mesh3d = mesh3d_load("showcase/resource/Rock/Rock.obj");
	mesh3d_make_centroid_origin(rock_mesh3d);
	mesh3d_calculate_tangents(rock_mesh3d);
	mesh_t* rock = mesh_create(renderer, rock_mesh3d);
	mesh3d_destroy(rock_mesh3d);
	shader_t* rock_shader = shader_load(renderer, "showcase/resource/shaders/bump_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); // position
	per_vertex_attributes[1] = MATERIAL_ALIGN(MATERIAL_VEC3, 1); // normal
	per_vertex_attributes[2] = MATERIAL_ALIGN(MATERIAL_VEC2, 2); // texture coordinates
	per_vertex_attributes[3] = MATERIAL_ALIGN(MATERIAL_VEC3, 3); // tangent
	material_create_info_t rock_material_info = 
	{
		.per_vertex_attribute_binding_count = 4,
		.per_vertex_attribute_bindings = &per_vertex_attributes[0],
		.is_vertex_attrib_from_file = true,
		.shader = rock_shader
	};
	material_t* rock_material = material_create(renderer, &rock_material_info);
	texture_t* rock_textures[] = 
	{ 
		texture_load(renderer, TEXTURE_TYPE_ALBEDO, "showcase/resource/Rock/albedo.bmp"),
		texture_load(renderer, TEXTURE_TYPE_NORMAL, "showcase/resource/Rock/normal.bmp")
	};
	material_set_texture(rock_material, "albedo", rock_textures[0]);
	material_set_texture(rock_material, "normal_map", rock_textures[1]);
	/*---------------------------------------*/

	/*----------- CUBEMAP - SKYBOX ----------------------*/
	texture_t* skybox_texture = texture_load(renderer, TEXTURE_TYPE_CUBE,
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	mesh3d_t* skybox_mesh3d = mesh3d_cube(5);
	mesh3d_flip_triangles(skybox_mesh3d);
	mesh_t* skybox = mesh_create(renderer, skybox_mesh3d);
	mesh3d_destroy(skybox_mesh3d);

	shader_t* skybox_shader = shader_load(renderer, "showcase/resource/shaders/skybox_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); 	// position
	material_create_info_t skybox_material_info = 
	{
		.per_vertex_attribute_binding_count = 1,
		.per_vertex_attribute_bindings = &per_vertex_attributes[0],
		.shader = skybox_shader
	};
	material_t* skybox_material = material_create(renderer, &skybox_material_info);
	material_set_texture(skybox_material, "skybox", skybox_texture);


	/*----------- QUAD ----------------------*/
	mesh3d_t* quad_mesh3d = mesh3d_plane(0.6f);
	mesh_t* quad = mesh_create(renderer, quad_mesh3d);
	mesh3d_destroy(quad_mesh3d);
	shader_t* quad_shader = shader_load(renderer, "showcase/resource/shaders/transparent_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); //position
	material_create_info_t quad_material_info =
	{
		.per_vertex_attribute_binding_count = 1,
		.per_vertex_attribute_bindings = &per_vertex_attributes[0],
		.shader = quad_shader,
		// .is_transparent = true
	};
	material_t* quad_material = material_create(renderer, &quad_material_info);

	time_handle_t frame_time_handle = time_get_handle();
	time_handle_t second_time_handle = time_get_handle();
	time_handle_t game_time_handle = time_get_handle();
	u32 frame_count = 0;
	float angle = 0;
	u32 texture_index = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		float delta_time = time_get_delta_time(&frame_time_handle);
		float game_time = time_get_seconds(game_time_handle);
		material_set_float(text_material, "ubo.time", game_time);
		material_set_float(game_ui_material, "ubo.time", game_time);

		material_set_vec3(rock_material, "light.color", vec3(float)(1, 1, 1));
		material_set_vec3(rock_material, "light.dir", vec3_normalize(float)(vec3(float)(1, -1, 3)));
		material_set_float(rock_material, "light.intensity", 4.0f);

		// vec4_t(float) eye_dir = mat4_mul_vec4(float)(camera_transform, 1, 0, 0, 0);
		// material_set_vec3(rock_material, "misc.eye_dir", vec3_normalize(float)(vec3(float)(eye_dir.x, eye_dir.y, eye_dir.z)));

		// material_set_vec3(rock_material, "properties.specular_color", vec3(float)(1, 1, 1));
		// material_set_float(rock_material, "properties.specularity", 2);

		renderer_begin_frame(renderer, 0.01f, 0.1f, 0.2f, 0.4f);

		material_bind(rock_material);
		mat4_t(float) model_matrix;
		mat4_move(float)(&model_matrix, mat4_mul(float)(2, mat4_rotation(float)(0, 0 * DEG2RAD, 0), mat4_scale(float)(2.2, 2.2, 2.2)));
		mat4_t(float) mvp = mat4_mul(float)(4, clip_matrix, projection_matrix, view_matrix, model_matrix);
		material_set_push_mat4(rock_material, "push.mvp_matrix", mat4_transpose(float)(mvp));
		material_set_push_mat4(rock_material, "push.model_matrix", mat4_transpose(float)(model_matrix));
		mesh_draw_indexed(rock);

		material_bind(skybox_material);

		mat4_t(float) skybox_matrix = view_matrix;
		// skybox should remain at origin, so set the last column of the view_matrix to zero
		float** elements = (float**)mat4_data(float)(&skybox_matrix);
		elements[0][3] = 0;
		elements[1][3] = 0;
		elements[2][3] = 0;
		elements[3][3] = 1;
		mat4_move(float)(&mvp, mat4_mul(float)(3, clip_matrix, projection_matrix, skybox_matrix));
		material_set_push_mat4(skybox_material, "push.mvp_matrix", mat4_transpose(float)(mvp));
		mesh_draw_indexed(skybox);

		material_bind(quad_material);
		mat4_move(float)(&model_matrix, mat4_mul(float)(2, mat4_translation(float)(-0.8f, 0, 0), mat4_rotation(float)(0, 0, 80 * DEG2RAD)));
		mat4_move(float)(&mvp, mat4_mul(float)(4, clip_matrix, projection_matrix, view_matrix, model_matrix));
		mat4_move(float)(&mvp, mat4_transpose(float)(mvp));
		material_set_push_mat4(quad_material, "push.mvp_matrix", mvp);
		mesh_draw_indexed(quad);

		material_bind(text_material);
		mat4_t(float) canvas_transform = mat4_mul(float)(2, clip_matrix, screen_space_matrix);
		mat4_t(float) _model_matrix = mat4_mul(float)(2, mat4_translation(float)(0, 0, 0), mat4_scale(float)(0, 50, 50));
		material_set_push_mat4(text_material, "push.mvp_matrix", mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _model_matrix)));
		text_mesh_draw(text_mesh);


		material_bind(game_ui_material);
		mat4_move(float)(&_model_matrix, mat4_mul(float)(2, mat4_scale(float)(50, 50, 50), mat4_identity(float)()));
		material_set_push_mat4(game_ui_material, "push.mvp_matrix", mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _model_matrix)));
		text_mesh_draw(game_ui);
		// mesh_draw_indexed(glyph_A);
		// mesh_draw_indexed(glyph_B);

		renderer_end_frame(renderer);

		renderer_update(renderer);
		angle += 180 * delta_time * 0.1f;
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
			second_time_handle = time_get_handle();
			frame_count = 0;
			texture_index++;
			texture_index = texture_index % 3;
		}
		++frame_count;
	}

	// destroy fonts
	font_destroy(font);

	// destroy meshes
	glyph_mesh_pool_destroy(pool);
	text_mesh_destroy(game_ui);
	text_mesh_destroy(text_mesh);
	mesh_destroy(rock);
	mesh_destroy(quad);
	mesh_destroy(skybox);

	// destroy shaders
	shader_destroy(text_shader);
	shader_destroy(game_ui_shader);
	shader_destroy(rock_shader);
	shader_destroy(quad_shader);
	shader_destroy(skybox_shader);

	// destroy materials
	material_destroy(game_ui_material);
	material_destroy(text_material);
	material_destroy(rock_material);
	material_destroy(quad_material);
	material_destroy(skybox_material);

	// destroy textures
	texture_destroy(skybox_texture);
	for(int i = 0; i < 2; i++)
		texture_destroy(rock_textures[i]);


	// release resources (extra heap allocated memory)
	
	font_release_resources(font);

	glyph_mesh_pool_release_resources(pool);
	text_mesh_release_resources(game_ui);
	text_mesh_release_resources(text_mesh);
	mesh_release_resources(rock);
	mesh_release_resources(quad);
	mesh_release_resources(skybox);
	
	shader_release_resources(text_shader);
	shader_release_resources(game_ui_shader);
	shader_release_resources(rock_shader);
	shader_release_resources(quad_shader);
	shader_release_resources(skybox_shader);
	
	material_release_resources(text_material);
	material_release_resources(game_ui_material);
	material_release_resources(rock_material);
	material_release_resources(quad_material);
	material_release_resources(skybox_material);

	for(int i = 0; i < 2; i++)
		texture_release_resources(rock_textures[i]);
	texture_release_resources(skybox_texture);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
