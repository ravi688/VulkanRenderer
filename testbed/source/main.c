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

	mesh3d_t* char_mesh = mesh3d_new();
	mesh3d_positions_new(char_mesh, 0);
	mesh3d_colors_new(char_mesh, 0);
	mesh3d_triangles_new(char_mesh, 0);

	font_t* font = font_load_and_create("resource/fonts/arial.ttf");
	text_mesh_t* text = text_mesh_create(font);
	text_mesh_set_string(text, "Hello World");
	text_mesh_set_size(text, 100);

	int index = ttf_find_glyph(font->handle, L'&');
	assert(index >= 0);

	ttf_mesh_t* mesh;
	int result = ttf_glyph2mesh(&font->handle->glyphs[index], &mesh, TTF_QUALITY_LOW, TTF_FEATURES_DFLT);
	assert(result == TTF_DONE);

	float max_y = 0, min_y = 0, max_x = 0, min_x = 0;
	for(int i = 0; i < mesh->nvert; i++)
	{
		if(mesh->vert[i].y > max_y)
			max_y = mesh->vert[i].y;
		if(mesh->vert[i].y < min_y)
			min_y = mesh->vert[i].y;

		if(mesh->vert[i].x > max_x)
			max_x = mesh->vert[i].x;
		if(mesh->vert[i].x < min_x)
			min_x = mesh->vert[i].x;

		mesh3d_position_add(char_mesh, 0, mesh->vert[i].y * 100, mesh->vert[i].x * 100 + 200);
		mesh3d_color_add(char_mesh, 1, 1, 1);
	}
	for(int i = 0; i < mesh->nfaces; i++)
		mesh3d_triangle_add(char_mesh, mesh->faces[i].v3, mesh->faces[i].v2, mesh->faces[i].v1);
	mesh3d_optimize_buffer(char_mesh);
	vec3_t(float) center = { .z = (max_x + min_x) * 800 * 0.5f, .y = (max_y + min_y) * 800 * 0.5f };
	mesh3d_positions_foreach(char_mesh, offset_visitor, &center);
	ttf_free_mesh(mesh);
	mesh_t* char_render_mesh = mesh_create(renderer, char_mesh);

	shader_t* font_shader = shader_load(renderer, "resource/shaders/font_shader.sb");
	material_create_info_t font_material_info =
	{
		.per_vertex_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0) //position
								| MATERIAL_ALIGN(MATERIAL_VEC3, 1), //color
		.per_instance_attributes = MATERIAL_ALIGN(MATERIAL_VEC3, 0), //offset
		.shader = font_shader
	};
	material_t* font_material = material_create(renderer, &font_material_info);

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
	vulkan_mesh_create_and_add_vertex_buffer(char_render_mesh, renderer, &offset_create_info);

	time_handle_t frame_time_handle = time_get_handle();
	time_handle_t second_time_handle = time_get_handle();
	u32 frame_count = 0;
	float angle = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		float delta_time = time_get_delta_time(&frame_time_handle);
		renderer_begin_frame(renderer, 0, 0, 0, 0);

		material_bind(font_material, renderer);
		mat4_t(float) canvas_transform = mat4_mul(float)(2, clip_matrix, screen_space_matrix);

		mat4_move(float)(&canvas_transform, mat4_transpose(float)(canvas_transform));
		material_push_constants(font_material, renderer, &canvas_transform);
		mesh_draw_indexed_instanced(char_render_mesh, renderer, 2);

		renderer_end_frame(renderer);
		renderer_update(renderer);
		angle += 180 * delta_time;
		if(angle >= 360.0f)
			angle = 0.0f;

		float seconds = time_get_seconds(second_time_handle);
		if(seconds >= 1)
		{
			printf("FPS: %u\n", (int)((int)frame_count / seconds));
			second_time_handle = time_get_handle();
			frame_count = 0;
		}
		++frame_count;
	}

	font_destroy(font);
	font_release_resources(font);
	text_mesh_destroy(text, renderer);
	text_mesh_release_resources(text);

	mesh_destroy(char_render_mesh, renderer);
	mesh_release_resources(char_render_mesh);
	mesh3d_destroy(char_mesh);

	shader_destroy(font_shader, renderer);
	shader_release_resources(font_shader);
	material_destroy(font_material, renderer);
	material_release_resources(font_material);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
