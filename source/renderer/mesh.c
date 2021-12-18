
#include <renderer/internal/vulkan/vulkan_mesh.h>

#include <renderer/mesh.h>
#include <renderer/mesh3d.h>
#include <renderer/assert.h>

vulkan_mesh_t* mesh_new()
{
	return vulkan_mesh_new();
}

vulkan_mesh_t* mesh_create(renderer_t* renderer, mesh3d_t* mesh_data)
{
	ASSERT(mesh_data != NULL, "Failed to create mesh_t, mesh3d_t* mesh_data == NULL\n");
	ASSERT(mesh3d_has_positions(mesh_data), "!mesh3d_has_positions(mesh_data)\n");
	ASSERT(mesh3d_has_normals(mesh_data), "!mesh3d_has_normals(mesh_data)\n");
	ASSERT(mesh3d_has_colors(mesh_data), "!mesh3d_has_colors(mesh_data)\n");
	ASSERT(mesh3d_has_uvs(mesh_data), "!mesh3d_has_uvs(mesh_data)\n");
	vulkan_mesh_create_info_t create_info =
	{
		.vertex_count = mesh3d_positions_count(mesh_data),

		.p_position_data = mesh_data->positions->bytes,
		.position_stride = mesh3d_sizeof_position(mesh_data),

		.p_color_data = mesh_data->colors->bytes,
		.color_stride = mesh3d_sizeof_color(mesh_data),

		.p_normal_data = mesh_data->normals->bytes,
		.normal_stride = mesh3d_sizeof_normal(mesh_data),

		.p_uv_data = mesh_data->uvs->bytes,
		.uv_stride = mesh3d_sizeof_uv(mesh_data),

		.p_index_data = mesh_data->triangles->bytes,
		.index_stride = mesh3d_sizeof_index(mesh_data),
		.index_count = mesh3d_triangles_count(mesh_data) * 3
	};
	ASSERT(create_info.color_stride == (sizeof(float) * 3), "create_info.color_stride != (sizeof(float) * 3)\n");
	ASSERT(create_info.normal_stride == (sizeof(float) * 3), "create_info.normal_stride != (sizeof(float) * 3)\n");
	ASSERT(create_info.position_stride == (sizeof(float) * 3), "create_info.position_stride != (sizeof(float) * 3)\n");
	ASSERT(create_info.uv_stride == (sizeof(float) * 2), "create_info.uv_stride != (sizeof(float) * 2)\n");
	vulkan_mesh_t* vulkan_mesh = vulkan_mesh_create(renderer, &create_info);
	return vulkan_mesh;
}

void mesh_create_no_alloc(renderer_t* renderer, mesh3d_t* mesh_data, mesh_t* mesh)
{
	ASSERT(mesh3d_has_positions(mesh_data), "!mesh3d_has_positions(mesh_data)\n");
	ASSERT(mesh3d_has_normals(mesh_data), "!mesh3d_has_normals(mesh_data)\n");
	ASSERT(mesh3d_has_colors(mesh_data), "!mesh3d_has_colors(mesh_data)\n");
	// ASSERT(mesh3d_has_uvs(mesh_data), "!mesh3d_has_uvs(mesh_data)\n");
	vulkan_mesh_create_info_t create_info =
	{
		.vertex_count = mesh3d_positions_count(mesh_data),

		.p_position_data = mesh_data->positions->bytes,
		.position_stride = mesh3d_sizeof_position(mesh_data),

		.p_color_data = mesh_data->colors->bytes,
		.color_stride = mesh3d_sizeof_color(mesh_data),

		.p_normal_data = mesh_data->normals->bytes,
		.normal_stride = mesh3d_sizeof_normal(mesh_data),

		.p_index_data = mesh_data->triangles->bytes,
		.index_stride = mesh3d_sizeof_index(mesh_data),
		.index_count = mesh3d_triangles_count(mesh_data) * 3
	};
	vulkan_mesh_create_no_alloc(renderer, &create_info, mesh);
}

void mesh_destroy(mesh_t* mesh, renderer_t* renderer)
{
	vulkan_mesh_destroy(mesh, renderer);
}

void mesh_release_resources(mesh_t* mesh)
{
	vulkan_mesh_release_resources(mesh);
}

void mesh_sync(mesh_t* mesh, renderer_t* renderer, mesh3d_t* mesh_data)
{
	assert(mesh != NULL);
	vulkan_mesh_create_info_t mesh_info =
	{
		.vertex_count = mesh3d_positions_count(mesh_data),

		.p_position_data = mesh_data->positions->bytes,
		.position_stride = mesh3d_sizeof_position(mesh_data),

		.p_color_data = mesh_data->colors->bytes,
		.color_stride = mesh3d_sizeof_color(mesh_data),

		.p_normal_data = mesh_data->normals->bytes,
		.normal_stride = mesh3d_sizeof_normal(mesh_data),

		.p_index_data = mesh_data->triangles->bytes,
		.index_stride = mesh3d_sizeof_index(mesh_data),
		.index_count = mesh3d_triangles_count(mesh_data) * 3
	};
	vulkan_mesh_sync(mesh, renderer, &mesh_info);
}

void mesh_draw(mesh_t* mesh, renderer_t* renderer)
{
	vulkan_mesh_draw(mesh, renderer);
}

void mesh_draw_indexed(mesh_t* mesh, renderer_t* renderer)
{
	vulkan_mesh_draw_indexed(mesh, renderer);
}

void mesh_draw_instanced(mesh_t* mesh, renderer_t* renderer, uint32_t instance_count)
{
	vulkan_mesh_draw_instanced(mesh, renderer, instance_count);
}
void mesh_draw_indexed_instanced(mesh_t* mesh, renderer_t* renderer, uint32_t instance_count)
{
	vulkan_mesh_draw_indexed_instanced(mesh, renderer, instance_count);
}
