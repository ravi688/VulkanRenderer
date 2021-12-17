//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/render_window.h>

//For handling text/font rendering
#include <renderer/font.h>

#include <renderer/mesh3d.h>
#include <renderer/defines.h>

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
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }

static mat4_t(float) projection_matrix;

static void recreate_matrix(render_window_t* window, void* user_data)
{
	mat4_move(float)(&projection_matrix,  mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)window->width/window->height));
	// mat4_move(float)(&projection_matrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
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

 	shader_t* shader = shader_load(renderer, "resource/shaders/diffuse.sb");
	material_t* material = material_create(renderer, shader->stage_count, shader->stage_shaders);

	font_t* font = font_load_and_create("resource/fonts/arial.ttf");



	font_destroy(font);
	font_release_resources(font);


	texture_t* texture = texture_load(renderer, "resource/textures/linuxlogo.bmp");
	material_set_texture(material, renderer, texture);
	mesh3d_t* cube_mesh = mesh3d_plane(1);
	mesh3d_make_centroid_origin(cube_mesh);
	mesh_t* cube = mesh_create(renderer, cube_mesh);

	float angle = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		mat4_t(float) vp = mat4_mul(float)(3, clip_matrix, projection_matrix, view_matrix);
		vec3_t(float) eulerRotation = vec3_scale(float)(vec3(float)(0, 1, 0), angle * DEG2RAD);
		renderer_begin_frame(renderer, 0.1f, 0.3f, 0.1f, 0.0f);

		material_bind(material, renderer);

		mat4_t(float) mvp1 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(0, 0, 0), eulerRotation));
		mat4_move(float)(&mvp1, mat4_transpose(float)(mvp1));
		material_push_constants(material, renderer, &mvp1);
		mesh_draw_indexed(cube, renderer);

		renderer_end_frame(renderer);
		renderer_update(renderer);
		angle += 0.05f;
		if(angle >= 360.0f)
			angle = 0.0f;
	}

	mesh_destroy(cube, renderer);
	mesh_release_resources(cube);
	mesh3d_destroy(cube_mesh);

	shader_destroy(shader, renderer);
	shader_release_resources(shader);

	material_destroy(material, renderer);
	material_release_resources(material);
	texture_destroy(texture, renderer);
	texture_release_resources(texture);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
