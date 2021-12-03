//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/render_window.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

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
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);
	renderer_t* renderer = renderer_init(800, 800, "Vulkan 3D Renderer");

 	//Prepare shaders
	shader_t** shaders1 = stack_newv(shader_t*, 2);
	ref(shader_t*, shaders1, 0) = shader_create(renderer, "shaders/fragmentShader2.spv", SHADER_TYPE_FRAGMENT);
	ref(shader_t*, shaders1, 1) = shader_create(renderer, "shaders/vertexShader2.spv", SHADER_TYPE_VERTEX);

	//Prepare Material
	material_t* material = material_create(renderer, 2, shaders1);

	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -6, 2, 0 }, (vec3_t(float)) { 0, 0, -15 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) clip_matrix = mat4_identity(float)();
	clip_matrix.m11 = -1;

	recreate_matrix(renderer->window, NULL);
	render_window_subscribe_on_resize(renderer->window, recreate_matrix, NULL);

	mesh3d_t* cube_mesh1 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh2 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh3 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh4 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh5 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh6 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh7 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh8 = mesh3d_cube(0.5f);
	mesh_t* cube1 = mesh_create(renderer, cube_mesh1);
	mesh_t* cube2 = mesh_create(renderer, cube_mesh2);
	mesh_t* cube3 = mesh_create(renderer, cube_mesh2);
	mesh_t* cube4 = mesh_create(renderer, cube_mesh2);
	mesh_t* cube5 = mesh_create(renderer, cube_mesh1);
	mesh_t* cube6 = mesh_create(renderer, cube_mesh2);
	mesh_t* cube7 = mesh_create(renderer, cube_mesh2);
	mesh_t* cube8 = mesh_create(renderer, cube_mesh2);


	float angle = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		mat4_t(float) vp = mat4_mul(float)(3, clip_matrix, projection_matrix, view_matrix);
		renderer_begin_frame(renderer, 0.0f, 0.0f, 0.0f, 0.0f);

		material_bind(material, renderer);

		mat4_t(float) mvp1 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(0, 0, 0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp1, mat4_transpose(float)(mvp1));
		material_push_constants(material, renderer, &mvp1);
		mesh_draw_indexed(cube1, renderer);

		mat4_t(float) mvp2 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(0, 0, -0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp2, mat4_transpose(float)(mvp2));
		material_push_constants(material, renderer, &mvp2);
		mesh_draw_indexed(cube2, renderer);

		mat4_t(float) mvp3 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(0, 0.6f, 0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp3, mat4_transpose(float)(mvp3));
		material_push_constants(material, renderer, &mvp3);
		mesh_draw_indexed(cube3, renderer);

		mat4_t(float) mvp4 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(0, 0.6f, -0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp4, mat4_transpose(float)(mvp4));
		material_push_constants(material, renderer, &mvp4);
		mesh_draw_indexed(cube4, renderer);

		mat4_t(float) mvp5 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(-0.6f, 0, 0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp5, mat4_transpose(float)(mvp5));
		material_push_constants(material, renderer, &mvp5);
		mesh_draw_indexed(cube5, renderer);

		mat4_t(float) mvp6 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(-0.6f, 0, -0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp6, mat4_transpose(float)(mvp6));
		material_push_constants(material, renderer, &mvp6);
		mesh_draw_indexed(cube6, renderer);

		mat4_t(float) mvp7 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(-0.6f, 0.6f, -0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp7, mat4_transpose(float)(mvp7));
		material_push_constants(material, renderer, &mvp7);
		mesh_draw_indexed(cube7, renderer);

		mat4_t(float) mvp8 = mat4_mul(float)(2, vp, mat4_transform(vec3(float)(-0.6f, 0.6f, 0.3f), vec3(float)(0, angle * DEG2RAD, 0)));
		mat4_move(float)(&mvp8, mat4_transpose(float)(mvp8));
		material_push_constants(material, renderer, &mvp8);
		mesh_draw_indexed(cube8, renderer);

		renderer_end_frame(renderer);

		renderer_update(renderer);
		angle += 0.1f;
		if(angle >= 360.0f)
			angle = 0.0f;
	}
	mesh_destroy(cube1, renderer);
	mesh_destroy(cube2, renderer);
	mesh_destroy(cube3, renderer);
	mesh_destroy(cube4, renderer);
	mesh_destroy(cube5, renderer);
	mesh_destroy(cube6, renderer);
	mesh_destroy(cube7, renderer);
	mesh_destroy(cube8, renderer);

	mesh_release_resources(cube1);
	mesh_release_resources(cube2);
	mesh_release_resources(cube3);
	mesh_release_resources(cube4);
	mesh_release_resources(cube5);
	mesh_release_resources(cube6);
	mesh_release_resources(cube7);
	mesh_release_resources(cube8);

	for(u32 i = 0; i < 2; i++)
	{
		shader_destroy(shaders1[i], renderer);
	}
	for(u32 i = 0; i < 2; i++)
	{
		shader_release_resources(shaders1[i]);
	}
	stack_free(shaders1);

	material_destroy(material, renderer);
	material_release_resources(material);

	mesh3d_destroy(cube_mesh1);
	mesh3d_destroy(cube_mesh2);
	mesh3d_destroy(cube_mesh3);
	mesh3d_destroy(cube_mesh4);
	mesh3d_destroy(cube_mesh5);
	mesh3d_destroy(cube_mesh6);
	mesh3d_destroy(cube_mesh7);
	mesh3d_destroy(cube_mesh8);
	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
