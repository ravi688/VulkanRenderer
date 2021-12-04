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
	// mat4_move(float)(&projection_matrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);
	renderer_t* renderer = renderer_init(600, 600, "Vulkan 3D Renderer");

 	//Prepare shaders
	shader_t** shaders = stack_newv(shader_t*, 2);
	ref(shader_t*, shaders, 0) = shader_create(renderer, "shaders/fragmentShader.spv", SHADER_TYPE_FRAGMENT);
	ref(shader_t*, shaders, 1) = shader_create(renderer, "shaders/vertexShader.spv", SHADER_TYPE_VERTEX);

	//Prepare Material
	material_t* material = material_create(renderer, 2, shaders);

	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -3, 1.2f, 0 }, (vec3_t(float)) { 0, 0, -20 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) clip_matrix = mat4_identity(float)(); clip_matrix.m11 = -1;

	recreate_matrix(renderer->window, NULL);
	render_window_subscribe_on_resize(renderer->window, recreate_matrix, NULL);

	mesh3d_t* cube_mesh = mesh3d_cube(1);
	mesh_t* cube = mesh_create(renderer, cube_mesh);


	float angle = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		mat4_t(float) vp = mat4_mul(float)(3, clip_matrix, projection_matrix, view_matrix);
		vec3_t(float) eulerRotation = vec3_scale(float)(vec3(float)(0, 1, 0), angle * DEG2RAD);
		renderer_begin_frame(renderer, 0.0f, 0.0f, 0.0f, 0.0f);

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

	for(u32 i = 0; i < 2; i++)
	{
		shader_destroy(shaders[i], renderer);
		shader_release_resources(shaders[i]);
	}
	stack_free(shaders);

	material_destroy(material, renderer);
	material_release_resources(material);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}