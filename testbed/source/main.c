//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
// #include <renderer/texture.h>
#include <renderer/render_window.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_texture.h>

#include <renderer/mesh3d.h>
#include <renderer/defines.h>

#include <renderer/png.h>
#include <renderer/bmp.h>

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
	recreate_matrix(renderer->window, NULL);
	render_window_subscribe_on_resize(renderer->window, recreate_matrix, NULL);
	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -3, 1, 0 }, (vec3_t(float)) { 0, 0, -20 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) clip_matrix = mat4_identity(float)(); clip_matrix.m11 = -1;

 	//Prepare shaders
	shader_t** shaders = stack_newv(shader_t*, 2);
	ref(shader_t*, shaders, 0) = shader_create(renderer, "shaders/fragmentShader.spv", SHADER_TYPE_FRAGMENT);
	ref(shader_t*, shaders, 1) = shader_create(renderer, "shaders/vertexShader.spv", SHADER_TYPE_VERTEX);

	//Prepare Material
	material_t* material = material_create(renderer, 2, shaders);

	//Prepare Textures
	bmp_t image1 = bmp_load("resource/textures/green.bmp");
	bmp_t image2 = bmp_load("resource/textures/linuxlogo.bmp");
	vulkan_texture_create_info_t create_info1 = { .data = image1.data, .width = image1.width, .height = image1.height, .channel_count = image1.channel_count };
	vulkan_texture_t* texture1 = vulkan_texture_create(renderer, &create_info1);
	vulkan_texture_create_info_t create_info2 = { .data = image2.data, .width = image2.width, .height = image2.height, .channel_count = image2.channel_count };
	vulkan_texture_t* texture2 = vulkan_texture_create(renderer, &create_info2);
	// vulkan_material_set_texture(material, renderer, texture1);
	vulkan_material_set_texture(material, renderer, texture2);

	//Prepare Mesh
	// mesh3d_t* cube_mesh = mesh3d_plane(1);
	mesh3d_t* cube_mesh = mesh3d_cube(1);
	// mesh3d_t* cube_mesh = mesh3d_load("resource/Crankshaft HD.stl");
	// mesh3d_t* cube_mesh = mesh3d_load("resource/Binary-box.stl");
	mesh3d_make_centroid_origin(cube_mesh);
	// mesh3d_transform_set(cube_mesh, mat4_mul(float)(2, mat4_rotation(float)(45 * DEG2RAD, 0, 0), mat4_scale(float)(1, 1, 1)));
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
	vulkan_texture_destroy(texture1, renderer);
	vulkan_texture_release_resources(texture1);
	vulkan_texture_destroy(texture2, renderer);
	vulkan_texture_release_resources(texture2);
	bmp_destroy(image1);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
