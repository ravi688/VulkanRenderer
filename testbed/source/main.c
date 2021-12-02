//For handling rendering
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/render_window.h>
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

typedef vulkan_shader_t* pvulkan_shader_t;
instantiate_stack_array(pvulkan_shader_t);
instantiate_stack_array(VkFormat);

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

typedef struct
{
	vec2_t(float) position;
	vec3_t(float) color;
} vertex2d_t;

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

static mesh3d_t* mesh3d_cube(float size)
{
	mesh3d_t* mesh = mesh3d_new();
	mesh3d_positions_new(mesh, 0);
	mesh3d_triangles_new(mesh, 0);
	mesh3d_colors_new(mesh, 0);
	mesh3d_normals_new(mesh, 0);

	float half_size = 0.5f * size;

	//Bottom
	mesh3d_position_add(mesh, -half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size );
	mesh3d_position_add(mesh, half_size, 0, half_size  );

	//Top
	mesh3d_position_add(mesh, half_size, size, half_size  );
	mesh3d_position_add(mesh, half_size, size, -half_size );
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, size, half_size );

	//Front
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, size, half_size );
	mesh3d_position_add(mesh, -half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, -half_size);

	//Left
	mesh3d_position_add(mesh, half_size, size, -half_size );
	mesh3d_position_add(mesh, -half_size, size, -half_size);
	mesh3d_position_add(mesh, -half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size );

	//Right
	mesh3d_position_add(mesh, -half_size, size, half_size);
	mesh3d_position_add(mesh, half_size, size, half_size );
	mesh3d_position_add(mesh, half_size, 0, half_size );
	mesh3d_position_add(mesh, -half_size, 0, half_size);

	//Back
	mesh3d_position_add(mesh, half_size, size, half_size );
	mesh3d_position_add(mesh, half_size, size, -half_size);
	mesh3d_position_add(mesh, half_size, 0, -half_size);
	mesh3d_position_add(mesh, half_size, 0, half_size );


	//clockwise order
	mesh3d_triangle_add(mesh, 2, 1, 0);
	mesh3d_triangle_add(mesh, 3, 2, 0);
	mesh3d_triangle_add(mesh, 6, 5, 4),
	mesh3d_triangle_add(mesh, 7, 6, 4);
	mesh3d_triangle_add(mesh, 8, 9, 10);
	mesh3d_triangle_add(mesh, 8, 10, 11);
	mesh3d_triangle_add(mesh, 12, 13, 14);
	mesh3d_triangle_add(mesh, 12, 14, 15);
	mesh3d_triangle_add(mesh, 16, 17, 18);
	mesh3d_triangle_add(mesh, 16, 18, 19);
	mesh3d_triangle_add(mesh, 20, 21, 22);
	mesh3d_triangle_add(mesh, 20, 22, 23);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, -1.0f, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 1.0f, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, 1.0f);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, -1.0f, 0, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 1.0f, 0, 0);

	for(int i = 0; i < 4; i++)
		mesh3d_normal_add(mesh, 0, 0, -1.0f);

	for(int i = 0; i < mesh3d_positions_count(mesh); i++)
		mesh3d_color_add(mesh, 1, 1, 1);

	mesh3d_optimize_buffer(mesh);
	return mesh;
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);
	renderer_t* renderer = renderer_init(800, 800, "Vulkan 3D Renderer");

 	//Prepare shaders
	vulkan_shader_t** shaders = stack_array(pvulkan_shader_t, 2,
											vulkan_shader_create(renderer, "shaders/fragmentShader.spv", VULKAN_SHADER_TYPE_FRAGMENT),
											vulkan_shader_create(renderer, "shaders/vertexShader.spv", VULKAN_SHADER_TYPE_VERTEX));

	//Prepare Material
	VkFormat* attribute_formats1 = stack_array(VkFormat, 1, VK_FORMAT_R32G32_SFLOAT);
	VkFormat* attribute_formats2 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	VkFormat* attribute_formats3 = stack_array(VkFormat, 1, VK_FORMAT_R32G32B32_SFLOAT);
	u32* attribute_offsets1 = stack_array(u32, 1, 0);
	u32* attribute_offsets2 = stack_array(u32, 1, 0);
	u32* attribute_offsets3 = stack_array(u32, 1, 0);
	vulkan_vertex_info_t* vertex_infos = stack_newv(vulkan_vertex_info_t, 3);
	ref(vulkan_vertex_info_t, vertex_infos, 0) = (vulkan_vertex_info_t) { sizeof(vec2_t(float)), 1, attribute_formats1, attribute_offsets1 };
	ref(vulkan_vertex_info_t, vertex_infos, 1) = (vulkan_vertex_info_t) { sizeof(vec3_t(float)), 1, attribute_formats2, attribute_offsets2 };
	ref(vulkan_vertex_info_t, vertex_infos, 2) = (vulkan_vertex_info_t) { sizeof(vec3_t(float)), 1, attribute_formats3, attribute_offsets3 };

	vulkan_material_create_info_t material_info =
	{
		.shaders = shaders,
		.shader_count = 2,
		.vertex_info_count = 3,
		.vertex_infos = vertex_infos
	};
	vulkan_material_t* material = vulkan_material_create(renderer, &material_info);

	//Prepare Mesh
	mesh3d_t* mesh = mesh3d_cube(0.7f);

	//Prepare Transformation Matrices
	mat4_t(float) cameraTransform = mat4_transform((vec3_t(float)) { -3, 2, 0 }, (vec3_t(float)) { 0, 0, -30 * DEG2RAD } );
	mat4_t(float) viewMatrix = mat4_inverse(float)(cameraTransform);
	mat4_t(float) modelMatrix = mat4_transform((vec3_t(float)) { 0, 0, 0 }, (vec3_t(float)) { 0, -30 * DEG2RAD, 0 });
	mat4_t(float) projectionMatrix = mat4_persp_projection(float)(0, 10, 65 * DEG2RAD, (float)renderer->window->width/renderer->window->height);

	//Prepare Vulkan Render Mesh
	vec2_t(float)* vertices2 = foreach_vertex3d(mesh, mat4_mul(float)(3, projectionMatrix, viewMatrix, modelMatrix));
	convert_to_vulkan_clipspace(vertices2, mesh3d_positions_count(mesh));

	vulkan_mesh_create_info_t mesh_info =
	{
		.vertex_count = mesh3d_positions_count(mesh),

		.p_position_data = vertices2,
		.position_stride = sizeof(vec2_t(float)),

		.p_color_data = mesh->colors->bytes,
		.color_stride = sizeof(vec3_t(float)),

		.p_normal_data = mesh->normals->bytes,
		.normal_stride = sizeof(vec3_t(float)),

		.p_index_data = mesh->triangles->bytes,
		.index_stride = sizeof(int),
		.index_count = mesh3d_triangles_count(mesh) * 3
	};
	vulkan_mesh_t* render_mesh = vulkan_mesh_create(renderer, &mesh_info);

	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		renderer_begin_frame(renderer, 0.0f, 0.0f, 0.0f, 0.0f);
		vulkan_material_bind(material, renderer);
		vulkan_mesh_draw_indexed(render_mesh, renderer);
		renderer_end_frame(renderer);

		renderer_update(renderer);
	}

	for(u32 i = 0; i < 2; i++)
		vulkan_shader_destroy(shaders[i], renderer);
	vulkan_material_destroy(material, renderer);
	vulkan_mesh_destroy(render_mesh, renderer);

	vulkan_mesh_release_resources(render_mesh);
	vulkan_material_release_resources(material);
	for(u32 i = 0; i < 2; i++)
		vulkan_shader_release_resources(shaders[i]);
	stack_free(attribute_formats1);
	stack_free(attribute_offsets1);
	stack_free(attribute_formats2);
	stack_free(attribute_offsets2);
	stack_free(shaders);
	mesh3d_destroy(mesh);
	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
