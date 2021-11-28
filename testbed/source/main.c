//For handling rendering
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/render_window.h>

//For loading mesh files
#include <disk_manager/file_reader.h>
#include <meshlib/stl/readers/ascii.h>

//For creating objects and attaching components on them
#include <ecs/object.h>
#include <ecs/scene_manager.h>
#include <Cube.h>


#include <memory_allocator/memory_allocator.h>
#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>

instantiate_stack_array(VkFormat);
#ifdef USE_SAFE_MEMORY
typedef vulkan_shader_t* pvulkan_shader_t;
instantiate_declaration_safe_array(pvulkan_shader_t);
instantiate_implementation_safe_array(pvulkan_shader_t);
#endif

typedef struct
{
	union
	{
		vec2_t(float) position;
		struct
		{
			float x;
			float y;
		};
	};

	union
	{
		vec3_t(float) color;
		struct
		{
			float r;
			float g;
			float b;
		};
	};
} vertex2d_t;

static vulkan_material_create_info_t material_info;
static vulkan_material_t* material;
static renderer_t* renderer;

static void destroy_material(render_window_t* window)
{
	vulkan_material_destroy(material, renderer);
	material = vulkan_material_create(renderer, &material_info);
}

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);
	renderer = renderer_init(800, 800, "Vulkan 3D Renderer");
	render_window_subscribe_on_resize(renderer->window, destroy_material);

 	//Prepare shaders
	vulkan_shader_t** shaders = stack_array(pvulkan_shader_t, 2,
											vulkan_shader_create(renderer, "shaders/fragmentShader.spv", VULKAN_SHADER_TYPE_FRAGMENT),
											vulkan_shader_create(renderer, "shaders/vertexShader.spv", VULKAN_SHADER_TYPE_VERTEX));

	//Prepare Material
	VkFormat* attribute_formats = stack_array(VkFormat, 2, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT);
	u32* attribute_offsets = stack_array(u32, 2, offsetof(vertex2d_t, position), offsetof(vertex2d_t, color));
	material_info.shaders = shaders;
	material_info.shader_count = 2;
	material_info.vertex_info = (vulkan_vertex_info_t) { sizeof(vertex2d_t), 2,  attribute_formats, attribute_offsets };
	material = vulkan_material_create(renderer, &material_info);

	//Prepare Mesh
	vertex2d_t vertices[4] =
	{
		{ { 0.5f, -0.5f }, { 1, 1, 0 } },
		{ { 0.5f, 0.5f }, { 1, 0, 1 } },
		{ { -0.5f, 0.5f }, { 1, 1, 1 } },
		{ { -0.5f, -0.5f }, { 0, 1, 1 } },
	};

	u32 indices[6] =
	{
		0, 1, 3,
		3, 1, 2
	};

	vulkan_mesh_create_info_t mesh_info =
	{
		.p_vertex_data = vertices,
		.p_index_data = indices,
		.vertex_stride = sizeof(vertex2d_t),
		.index_stride = sizeof(u32),
		.vertex_count = 4,
		.index_count = 6
	};

	vertex2d_t vertices2[3] =
	{
		{ { 0, -0.5f }, { 0, 0.6f, 0 } },
		{ { 0.5f, 0.5f }, { 0.6f, 0, 0.3f } },
		{ { -0.5f, 0.5f }, { 0, 0.1f, 0.4f } }
	};

	vulkan_mesh_create_info_t mesh_info2 =
	{
		.p_vertex_data = vertices2,
		.vertex_stride = sizeof(vertex2d_t),
		.vertex_count = 3,
	};

	vulkan_mesh_t* mesh = vulkan_mesh_create(renderer, &mesh_info);
	vulkan_mesh_t* mesh2 = vulkan_mesh_create(renderer, &mesh_info2);

	bool binded = false;

	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		renderer_begin_frame(renderer, 0.1f, 0.3f, 0.1f, 1.0f);
		vulkan_material_bind(material, renderer);
		vulkan_mesh_draw_indexed(mesh, renderer);
		vulkan_mesh_draw(mesh2, renderer);
		renderer_end_frame(renderer);

		renderer_update(renderer);
	}

	for(u32 i = 0; i < 2; i++)
		vulkan_shader_destroy(shaders[i], renderer);
	vulkan_material_destroy(material, renderer);
	vulkan_mesh_destroy(mesh, renderer);
	vulkan_mesh_destroy(mesh2, renderer);

	stack_free(attribute_formats);
	stack_free(attribute_offsets);
	stack_free(shaders);
	
	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
