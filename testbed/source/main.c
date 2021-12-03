//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/render_window.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/mesh3d.h>
#include <renderer/defines.h>

//For loading mesh files
#include <disk_manager/file_reader.h>
#include <meshlib/stl/readers/ascii.h>
#include <meshlib/stl/parse_callbacks.h>

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

typedef shader_t* pshader_t;
instantiate_static_stack_array(pshader_t);
instantiate_static_stack_array(VkFormat);

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

vec3_t(float) vec4_vec3(vec4_t(float) v)
{
	if(v.w == 0)
		return (vec3_t(float)) { v.x, v.y, v.z };
	else
	{
		float t = 1 / v.w;
		return (vec3_t(float))  { v.x * t, v.y * t, v.z * t };
	}
}

static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }


static void convert_to_vulkan_clipspace(vec2_t(float) *  const vertices, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vertices[i].y = -vertices[i].y;
}

static vec2_t(float)* foreach_vertex3d(mesh3d_t* mesh, mat4_t(float) m)
{
	vec2_t(float)* vertices2d = heap_newv(vec2_t(float), mesh3d_positions_count(mesh));
	for(u32 i = 0; i < mesh3d_positions_count(mesh); i++)
	{
		vec3_t(float) position = mesh3d_position_get(mesh, i);
		vec3_t(float) v = vec4_vec3(mat4_mul_vec4(float)(m, position.x, position.y, position.z, 1));
		ref(vec2_t(float), vertices2d, i).x = v.z;
		ref(vec2_t(float), vertices2d, i).y = v.y;
	}
	return vertices2d;
}

static void vertex_position(vec3_t(float) position, void* mesh)
{
	mesh3d_position_add_vec3(mesh, position);
}

static void vertex_normal(vec3_t(float) normal, void* mesh)
{
	mesh3d_normal_add_vec3(mesh, normal);
}

vulkan_mesh_t* get_vulkan_mesh(renderer_t* renderer, mesh3d_t* mesh, vec3_t(float) position, vec3_t(float) rotation, mat4_t(float) projection_matrix, mat4_t(float) view_matrix)
{
	vec2_t(float)* vertices = foreach_vertex3d(mesh, mat4_mul(float)(3, projection_matrix, view_matrix, mat4_transform(position, rotation)));
	convert_to_vulkan_clipspace(vertices, mesh3d_positions_count(mesh));

	vulkan_mesh_create_info_t mesh_info =
	{
		.vertex_count = mesh3d_positions_count(mesh),

		.p_position_data = vertices,
		.position_stride = sizeof(vec2_t(float)),

		.p_color_data = mesh->colors->bytes,
		.color_stride = sizeof(vec3_t(float)),

		.p_normal_data = mesh->normals->bytes,
		.normal_stride = sizeof(vec3_t(float)),

		.p_index_data = mesh->triangles->bytes,
		.index_stride = sizeof(int),
		.index_count = mesh3d_triangles_count(mesh) * 3
	};
	vulkan_mesh_t* vulkan_mesh = vulkan_mesh_create(renderer, &mesh_info);
	heap_free(vertices);
	return vulkan_mesh;
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);
	renderer_t* renderer = renderer_init(800, 800, "Vulkan 3D Renderer");

 	//Prepare shaders
	shader_t** shaders = stack_array(pshader_t, 2,
											shader_create(renderer, "shaders/fragmentShader.spv", SHADER_TYPE_FRAGMENT),
											shader_create(renderer, "shaders/vertexShader.spv", SHADER_TYPE_VERTEX));

	//Prepare Material
	material_t* material = material_create(renderer, 2, shaders);

	mat4_t(float) camera_transform = mat4_transform((vec3_t(float)) { -3, 2, 0 }, (vec3_t(float)) { 0, 0, -30 * DEG2RAD } );
	mat4_t(float) view_matrix = mat4_inverse(float)(camera_transform);
	mat4_t(float) projection_matrix = mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)renderer->window->width/renderer->window->height);
	mesh3d_t* cube_mesh1 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh2 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh3 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh4 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh5 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh6 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh7 = mesh3d_cube(0.5f);
	mesh3d_t* cube_mesh8 = mesh3d_cube(0.5f);
	vulkan_mesh_t* cube1 = get_vulkan_mesh(renderer, cube_mesh1, vec3(float)(0, 0, 0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube2 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(0, 0, -0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube3 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(0, 0.6f, 0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube4 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(0, 0.6f, -0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube5 = get_vulkan_mesh(renderer, cube_mesh1, vec3(float)(-0.6f, 0, 0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube6 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(-0.6f, 0, -0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube7 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(-0.6f, 0.6f, 0.3f), vec3_zero(float)(), projection_matrix, view_matrix);
	vulkan_mesh_t* cube8 = get_vulkan_mesh(renderer, cube_mesh2, vec3(float)(-0.6f, 0.6f, -0.3f), vec3_zero(float)(), projection_matrix, view_matrix);


	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		renderer_begin_frame(renderer, 0.0f, 0.0f, 0.0f, 0.0f);
		material_bind(material, renderer);
		vulkan_mesh_draw_indexed(cube1, renderer);
		vulkan_mesh_draw_indexed(cube2, renderer);
		vulkan_mesh_draw_indexed(cube3, renderer);
		vulkan_mesh_draw_indexed(cube4, renderer);
		vulkan_mesh_draw_indexed(cube5, renderer);
		vulkan_mesh_draw_indexed(cube6, renderer);
		vulkan_mesh_draw_indexed(cube7, renderer);
		vulkan_mesh_draw_indexed(cube8, renderer);
		renderer_end_frame(renderer);

		renderer_update(renderer);
	}
	vulkan_mesh_destroy(cube1, renderer);
	vulkan_mesh_destroy(cube2, renderer);
	vulkan_mesh_destroy(cube3, renderer);
	vulkan_mesh_destroy(cube4, renderer);
	vulkan_mesh_destroy(cube5, renderer);
	vulkan_mesh_destroy(cube6, renderer);
	vulkan_mesh_destroy(cube7, renderer);
	vulkan_mesh_destroy(cube8, renderer);

	vulkan_mesh_release_resources(cube1);
	vulkan_mesh_release_resources(cube2);
	vulkan_mesh_release_resources(cube3);
	vulkan_mesh_release_resources(cube4);
	vulkan_mesh_release_resources(cube5);
	vulkan_mesh_release_resources(cube6);
	vulkan_mesh_release_resources(cube7);
	vulkan_mesh_release_resources(cube8);

	for(u32 i = 0; i < 2; i++)
		shader_destroy(shaders[i], renderer);
	for(u32 i = 0; i < 2; i++)
		shader_release_resources(shaders[i]);
	stack_free(shaders);

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
