//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/render_window.h>
#include <renderer/time.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>

//For handling text/font rendering
#include <renderer/font.h>
#include <renderer/text_mesh.h>
// #include <renderer/text.h>

#include <renderer/mesh3d.h>
#include <renderer/defines.h>
#include <renderer/assert.h>

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
	vec3_t(float) center = *(vec3_t(float)*)user_data;
	*position = vec3_sub(float)(*position, center);
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

	// mesh3d_t* char_mesh = mesh3d_new();
	// mesh3d_positions_new(char_mesh, 0);
	// mesh3d_normals_new(char_mesh, 0);
	// mesh3d_uvs_new(char_mesh, 0);
	// mesh3d_colors_new(char_mesh, 0);
	// mesh3d_triangles_new(char_mesh, 0);

	// font_t* font = font_load_and_create("resource/fonts/arial.ttf");
	// text_mesh_t* text = text_mesh_create(font);
	// text_mesh_set_string(text, "Hello World");
	// text_mesh_set_size(text, 100);

	// int index = ttf_find_glyph(font->handle, L'&');
	// assert(index >= 0);

	// ttf_mesh_t* mesh;
	// int result = ttf_glyph2mesh(&font->handle->glyphs[index], &mesh, TTF_QUALITY_LOW, TTF_FEATURES_DFLT);
	// assert(result == TTF_DONE);

	// float max_y = 0, min_y = 0, max_x = 0, min_x = 0;
	// for(int i = 0; i < mesh->nvert; i++)
	// {
	// 	if(mesh->vert[i].y > max_y)
	// 		max_y = mesh->vert[i].y;
	// 	if(mesh->vert[i].y < min_y)
	// 		min_y = mesh->vert[i].y;

	// 	if(mesh->vert[i].x > max_x)
	// 		max_x = mesh->vert[i].x;
	// 	if(mesh->vert[i].x < min_x)
	// 		min_x = mesh->vert[i].x;

	// 	mesh3d_position_add(char_mesh, 0, mesh->vert[i].y * 100, mesh->vert[i].x * 100 + 200);
	// 	mesh3d_color_add(char_mesh, 1, 1, 1);
	// 	mesh3d_normal_add(char_mesh, 0, 1, 0);
	// 	mesh3d_uv_add(char_mesh, 0, 0);
	// }
	// for(int i = 0; i < mesh->nfaces; i++)
	// 	mesh3d_triangle_add(char_mesh, mesh->faces[i].v3, mesh->faces[i].v2, mesh->faces[i].v1);
	// mesh3d_optimize_buffer(char_mesh);
	// vec3_t(float) center = { .z = (max_x + min_x) * 800 * 0.5f, .y = (max_y + min_y) * 800 * 0.5f };
	// mesh3d_positions_foreach(char_mesh, offset_visitor, &center);
	// ttf_free_mesh(mesh);
	// mesh_t* char_render_mesh = mesh_create(renderer, char_mesh);

	shader_t* instanced_color_shader = shader_load(renderer, "resource/shaders/instanced_color_shader.sb");
	material_create_info_t instanced_color_material_info =
	{
		.per_vertex_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0) //position
								| MATERIAL_ALIGN(MATERIAL_VEC3, 1) //color
								| MATERIAL_ALIGN(MATERIAL_VEC2, 2), //texture coordinates
		.per_instance_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0) //offset
								 | MATERIAL_ALIGN(MATERIAL_MAT2, 1), //rotation matrix
		.shader = instanced_color_shader
	};
	material_t* instanced_color_material = material_create(renderer, &instanced_color_material_info);
	texture_t* texture = texture_load(renderer, "resource/textures/linuxlogo.bmp");
	material_set_texture(instanced_color_material, renderer, texture);

	mesh3d_t* mesh_bounds = mesh3d_new();
	mesh3d_positions_new(mesh_bounds, 4);
	mesh3d_colors_new(mesh_bounds, 4);
	mesh3d_triangles_new(mesh_bounds, 6);
	mesh3d_uvs_new(mesh_bounds, 4);

	mesh3d_position_add(mesh_bounds, 0,  100,  100);
	mesh3d_position_add(mesh_bounds, 0,  100, -100);
	mesh3d_position_add(mesh_bounds, 0, -100, -100);
	mesh3d_position_add(mesh_bounds, 0, -100,  100);

	mesh3d_triangle_add(mesh_bounds, 3, 1, 0);
	mesh3d_triangle_add(mesh_bounds, 3, 2, 1);

	mesh3d_uv_add(mesh_bounds, 1, 1);
	mesh3d_uv_add(mesh_bounds, 0, 1);
	mesh3d_uv_add(mesh_bounds, 0, 0);
	mesh3d_uv_add(mesh_bounds, 1, 0);

	mesh3d_color_add(mesh_bounds, 0, 0.1f, 0.5f);
	mesh3d_color_add(mesh_bounds, 0, 0.2f, 0.5f);
	mesh3d_color_add(mesh_bounds, 0, 0.3f, 0.5f);
	mesh3d_color_add(mesh_bounds, 0, 0.4f, 0.5f);

	mesh_t* bounds = mesh_create(renderer, mesh_bounds);

	vec3_t(float) offsets[2] =
	{
		{ 0, 0, -200 }, { 0, 0, 200 },
	};
	vulkan_vertex_buffer_create_info_t offset_create_info =
	{
		.data = offsets,
		.stride = sizeof(vec3_t(float)),
		.count = 2
	};
	vulkan_mesh_create_and_add_vertex_buffer(bounds, renderer, &offset_create_info);

	mat2_t(float) rotations[2] =
	{
		mat2_rotation(float)(-45 * DEG2RAD),
		mat2_rotation(float)(45 * DEG2RAD)
	};
	vulkan_vertex_buffer_create_info_t rotation_create_info =
	{
		.data = rotations,
		.stride = sizeof(mat2_t(float)),
		.count = 2
	};
	vulkan_mesh_create_and_add_vertex_buffer(bounds, renderer, &rotation_create_info);


	time_handle_t frame_time_handle = time_get_handle();
	time_handle_t second_time_handle = time_get_handle();
	u32 frame_count = 0;
	float angle = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		float delta_time = time_get_delta_time(&frame_time_handle);
		renderer_begin_frame(renderer, 0, 0, 0, 0);

		material_bind(instanced_color_material, renderer);
		mat4_t(float) canvas_transform = mat4_mul(float)(2, clip_matrix, screen_space_matrix);

		mat4_t(float) instance1;
		mat4_move(float)(&instance1, mat4_transpose(float)(canvas_transform));
		material_push_constants(instanced_color_material, renderer, &instance1);
		mesh_draw_indexed_instanced(bounds, renderer, 2);

		renderer_end_frame(renderer);
		renderer_update(renderer);
		angle += 180 * delta_time;
		if(angle >= 360.0f)
			angle = 0.0f;

		// float seconds = time_get_seconds(second_time_handle);
		// if(seconds >= 1)
		// {
		// 	printf("FPS: %u\n", (int)((int)frame_count / seconds));
		// 	second_time_handle = time_get_handle();
		// 	frame_count = 0;
		// }
		// ++frame_count;
	}

	// font_destroy(font);
	// font_release_resources(font);
	// text_mesh_destroy(text, renderer);
	// text_mesh_release_resources(text);

	mesh_destroy(bounds, renderer);
	mesh_release_resources(bounds);
	mesh3d_destroy(mesh_bounds);

	// mesh_destroy(cube, renderer);
	// mesh_release_resources(cube);
	// mesh3d_destroy(cube_mesh);

	// mesh_destroy(char_render_mesh, renderer);
	// mesh_release_resources(char_render_mesh);
	// mesh3d_destroy(char_mesh);

	shader_destroy(instanced_color_shader, renderer);
	shader_release_resources(instanced_color_shader);
	material_destroy(instanced_color_material, renderer);
	material_release_resources(instanced_color_material);
	texture_destroy(texture, renderer);
	texture_release_resources(texture);

	// shader_destroy(albedo_shader, renderer);
	// shader_release_resources(albedo_shader);
	// material_destroy(albedo_material, renderer);
	// material_release_resources(albedo_material);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
