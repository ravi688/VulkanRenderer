//For handling rendering
#include <renderer/renderer.h>
#include <Renderer/Renderer.hpp>
#include <Renderer/TextMesh.hpp>
#include <Math/Mat4.hpp>
#include <hpml/vec2.h>
#include <hpml/vec3.h>

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

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

static Math::Mat4 mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return Math::Mat4::translation(position.x, position.y, position.z) * Math::Mat4::rotation(eulerRotation.x, eulerRotation.y, eulerRotation.z); }

static Math::Mat4 projection_matrix;
static Math::Mat4 screen_space_matrix;

static void recreate_matrix(render_window_t* window, void* user_data)
{
	projection_matrix = Math::Mat4::perspectiveProjection(0.04f, 100, 60 * DEG2RAD, (float)window->width/window->height);
	// mat4_move(float)(&projection_matrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
	screen_space_matrix = Math::Mat4::orthographicProjection(-0.04f, 100, window->height, (float)window->width / window->height);
}

static void u32_to_string(u32 value, char* string)
{
	sprintf(string, "%u", value);
}

int main(int argc, char** argv)
{

	V3D::Renderer myRenderer(V3D::RendererGPUType::INTEGRATED);
	myRenderer.init();

	renderer_t* renderer = myRenderer.getHandle();
	recreate_matrix(renderer_get_window(renderer), NULL);
	render_window_subscribe_on_resize(renderer_get_window(renderer), recreate_matrix, NULL);
	Math::Mat4 camera_transform = mat4_transform((vec3_t(float)) { -1.8f, 0.6f, 0 }, (vec3_t(float)) { 0, 0, -22 * DEG2RAD } );
	Math::Mat4 view_matrix = camera_transform.inverse();
	Math::Mat4 clip_matrix = Math::Mat4::identity(); clip_matrix.m11 = -1;

	/*------TEXT-----------------------------*/
	V3D::Font myFont("resource/fonts/Pushster-Regular.ttf");
	font_t* font = myFont.getHandle();
	glyph_mesh_pool_t* pool = myRenderer.createGlyphMeshPool(myFont).getHandle();
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
	u64 per_vertex_attributes[5] = { MATERIAL_ALIGN(MATERIAL_VEC3, 0) };
	u64 per_instance_attributes[1] = { MATERIAL_ALIGN(MATERIAL_VEC3, 0) | MATERIAL_ALIGN(MATERIAL_VEC3, 1) | MATERIAL_ALIGN(MATERIAL_VEC3, 2) };
	material_create_info_t text_material_info = { };
		text_material_info.per_vertex_attribute_binding_count = 1;
		text_material_info.per_vertex_attribute_bindings = &per_vertex_attributes[0];
		text_material_info.per_instance_attribute_binding_count = 1;
		text_material_info.per_instance_attribute_bindings = &per_instance_attributes[0];
		text_material_info.shader = text_shader;

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
	mesh3d_t* rock_mesh3d = mesh3d_load("resource/Rock/Rock.obj");
	mesh3d_make_centroid_origin(rock_mesh3d);
	mesh3d_calculate_tangents(rock_mesh3d);
	mesh_t* rock = mesh_create(renderer, rock_mesh3d);
	mesh3d_destroy(rock_mesh3d);
	shader_t* rock_shader = shader_load(renderer, "resource/shaders/bump_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); // position
	per_vertex_attributes[1] = MATERIAL_ALIGN(MATERIAL_VEC3, 1); // normal
	per_vertex_attributes[2] = MATERIAL_ALIGN(MATERIAL_VEC2, 2); // texture coordinates
	per_vertex_attributes[3] = MATERIAL_ALIGN(MATERIAL_VEC3, 3); // tangent
	material_create_info_t rock_material_info = { };
		rock_material_info.per_vertex_attribute_binding_count = 4;
		rock_material_info.per_vertex_attribute_bindings = &per_vertex_attributes[0];
		rock_material_info.shader = rock_shader;

	material_t* rock_material = material_create(renderer, &rock_material_info);
	texture_t* rock_textures[] = 
	{ 
		texture_load(renderer, TEXTURE_TYPE_ALBEDO, "resource/Rock/albedo.bmp"),
		texture_load(renderer, TEXTURE_TYPE_NORMAL, "resource/Rock/normal.bmp")
	};
	material_set_texture(rock_material, "albedo", rock_textures[0]);
	material_set_texture(rock_material, "normal_map", rock_textures[1]);
	/*---------------------------------------*/

	/*----------- CUBEMAP - SKYBOX ----------------------*/
	texture_t* skybox_texture = texture_load(renderer, TEXTURE_TYPE_CUBE,
											"resource/skybox_textures/skybox/right.bmp",
											"resource/skybox_textures/skybox/left.bmp",
											"resource/skybox_textures/skybox/bottom.bmp",
											"resource/skybox_textures/skybox/top.bmp",
											"resource/skybox_textures/skybox/front.bmp", 
											"resource/skybox_textures/skybox/back.bmp");
	mesh3d_t* skybox_mesh3d = mesh3d_cube(5);
	mesh3d_flip_triangles(skybox_mesh3d);
	mesh_t* skybox = mesh_create(renderer, skybox_mesh3d);
	mesh3d_destroy(skybox_mesh3d);

	shader_t* skybox_shader = shader_load(renderer, "resource/shaders/skybox_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); 	// position
	material_create_info_t skybox_material_info = { };
		skybox_material_info.per_vertex_attribute_binding_count = 1;
		skybox_material_info.per_vertex_attribute_bindings = &per_vertex_attributes[0];
		skybox_material_info.shader = skybox_shader;

	material_t* skybox_material = material_create(renderer, &skybox_material_info);
	material_set_texture(skybox_material, "skybox", skybox_texture);


	/*----------- QUAD ----------------------*/
	mesh3d_t* quad_mesh3d = mesh3d_plane(0.6f);
	mesh_t* quad = mesh_create(renderer, quad_mesh3d);
	mesh3d_destroy(quad_mesh3d);
	shader_t* quad_shader = shader_load(renderer, "resource/shaders/transparent_shader.sb");
	per_vertex_attributes[0] = MATERIAL_ALIGN(MATERIAL_VEC3, 0); //position
	material_create_info_t quad_material_info =  { };
		quad_material_info.per_vertex_attribute_binding_count = 1;
		quad_material_info.per_vertex_attribute_bindings = &per_vertex_attributes[0];
		quad_material_info.shader = quad_shader;
		quad_material_info.is_transparent = true;

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
		Math::Mat4 model_matrix = Math::Mat4::rotation(0, 0 * DEG2RAD, 0) * Math::Mat4::scale(2.2, 2.2, 2.2);
		Math::Mat4 mvp = clip_matrix * projection_matrix * view_matrix * model_matrix;
		material_set_push_mat4(rock_material, "push.mvp_matrix", mvp.transpose());
		material_set_push_mat4(rock_material, "push.model_matrix", model_matrix.transpose());
		mesh_draw_indexed(rock);

		material_bind(skybox_material);

		Math::Mat4 skybox_matrix = view_matrix;
		// skybox should remain at origin, so set the last column of the view_matrix to zero
		float** elements = (float**)skybox_matrix.getData();
		elements[0][3] = 0;
		elements[1][3] = 0;
		elements[2][3] = 0;
		elements[3][3] = 1;
		mvp = clip_matrix * projection_matrix * skybox_matrix;
		material_set_push_mat4(skybox_material, "push.mvp_matrix", mvp.transpose());
		mesh_draw_indexed(skybox);

		material_bind(quad_material);
		model_matrix = Math::Mat4::translation(-0.8f, 0, 0) * Math::Mat4::rotation(0, 0, 80 * DEG2RAD);
		mvp = clip_matrix * projection_matrix * view_matrix * model_matrix;
		material_set_push_mat4(quad_material, "push.mvp_matrix", mvp.transpose());
		mesh_draw_indexed(quad);

		material_bind(text_material);
		Math::Mat4 canvas_transform = clip_matrix * screen_space_matrix;
		Math::Mat4 _model_matrix = Math::Mat4::translation(0, 0, 0) * Math::Mat4::scale(0, 50, 50);
		material_set_push_mat4(text_material, "push.mvp_matrix", (canvas_transform * _model_matrix).transpose());
		text_mesh_draw(text_mesh);


		material_bind(game_ui_material);
		_model_matrix = Math::Mat4::scale(50, 50, 50) * Math::Mat4::identity();
		material_set_push_mat4(game_ui_material, "push.mvp_matrix", (canvas_transform * _model_matrix).transpose());
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

	myRenderer.terminate();
	return 0;
}
