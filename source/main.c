
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
/*
	1. Default clear screen render pass after creating a camera [done]
	2. Test Shader & Material, Default Shaders [done]
	3. Rendering [done]
	4. Render Queue [done]
	5. Muliple render passes (shadow mapping) [ in - progress ]
	6. Multiple sub passes (color override)
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	
	// initialize renderer
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_AUTO, 800, 800, "Renderer", false, true);

	// create a camera
	AUTO camera = camera_create(renderer, CAMERA_PROJECTION_TYPE_PERSPECTIVE, 0.04f, 100, 65 DEG);

	AUTO pool = renderer_get_render_pass_pool(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	AUTO shaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR);
	AUTO shader = shader_library_getH(slib, shaderH);
	AUTO blueMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH, "BlueColorMaterial"));
	AUTO greenMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH, "GreenColorMaterial"));
	AUTO mesh = mesh_create(renderer, mesh3d_cube(1.0f));
	AUTO planeMesh = mesh_create(renderer, mesh3d_plane(1.0f));

	material_set_vec4(blueMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));
	material_set_vec4(greenMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));

	render_object_create_info_t create_info = 
	{
		.material = blueMaterial,
		.user_data = mesh,
		.draw_handler = CAST_TO(render_object_draw_handler_t, mesh_draw_indexed)
	};
	AUTO renderObject = render_object_create(renderer, &create_info);

	create_info.user_data = planeMesh;
	create_info.material = greenMaterial;
	AUTO renderObject2 = render_object_create(renderer, &create_info);
	render_object_set_transform(renderObject2, mat4_translation(float)(0, -1.0f, 0));

	AUTO queue = render_queue_create(renderer, "GeometryQueue");
	render_queue_add(queue, renderObject);
	render_queue_add(queue, renderObject2);

	while(renderer_is_running(renderer))
	{
		// begin command buffer recording
		renderer_begin_frame(renderer);

		render_queue_dispatch(queue);

		// end command buffer recording
		renderer_end_frame(renderer);

		// submit the work to the GPU and present the rendered image to the window
		renderer_update(renderer);
	}

	render_object_destroy(renderObject);
	render_object_release_resources(renderObject);

	render_queue_destroy(queue);
	render_queue_release_resources(queue);

	mesh_destroy(planeMesh);
	mesh_release_resources(planeMesh);
	mesh_destroy(mesh);
	mesh_release_resources(mesh);
	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
