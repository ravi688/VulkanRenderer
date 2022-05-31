
#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_EVERYTHING_INTERNAL
#define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_TIMING
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
/*
	1. Default clear screen render pass after creating a camera [done]
	2. Test Shader & Material, Default Shaders [done]
	3. Rendering [done]
	4. Render Queue [in - progress]
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

	AUTO pass = render_pass_pool_getH(pool, render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN));
	AUTO shaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR);
	AUTO shader = shader_library_getH(slib, shaderH);
	AUTO blueMaterial = material_library_getH(mlib, material_library_create_materialH(mlib, shaderH, "BlueColorMaterial"));
	AUTO mesh = mesh_create(renderer, mesh3d_cube(1.0f));
	
	material_set_vec4(blueMaterial, "parameters.color", vec4(float)(1, 1, 1, 1));

	render_object_create_info_t create_info = 
	{
		.material = blueMaterial,
		.user_data = mesh,
		.draw_handler = CAST_TO(render_object_draw_handler_t, mesh_draw_indexed)
	};
	AUTO renderObject = render_object_create(renderer, &create_info);

	// AUTO queue = render_queue_create(renderer, "GeometryQueue");

	time_handle_t tHandle = time_get_handle();
	float angle = 0.0f;
	while(renderer_is_running(renderer))
	{
		float deltaTime = time_get_delta_time(&tHandle);

		// begin command buffer recording
		renderer_begin_frame(renderer);

		vulkan_pipeline_layout_t* pipeline_layout = vulkan_shader_get_pipeline_layout(shader, CAST_TO(vulkan_render_pass_t*, pass)->handle, 0);

		render_pass_begin(pass, NULL);

		vulkan_graphics_pipeline_bind(vulkan_shader_get_pipeline(CAST_TO(vulkan_shader_t*, shader), CAST_TO(vulkan_render_pass_t*,pass)->handle, 0));

		vulkan_descriptor_set_bind(&renderer->vulkan_handle->global_set, VULKAN_DESCRIPTOR_SET_GLOBAL, pipeline_layout);
		vulkan_descriptor_set_bind(&CAST_TO(vulkan_render_pass_t*, pass)->render_set, VULKAN_DESCRIPTOR_SET_RENDER, pipeline_layout);
		vulkan_descriptor_set_bind(&CAST_TO(vulkan_render_pass_t*, pass)->sub_render_sets[0], VULKAN_DESCRIPTOR_SET_SUB_RENDER, pipeline_layout);
		vulkan_descriptor_set_bind(&CAST_TO(vulkan_material_t*, blueMaterial)->material_set, VULKAN_DESCRIPTOR_SET_MATERIAL, pipeline_layout);
		vulkan_descriptor_set_bind(&CAST_TO(vulkan_render_object_t*, renderObject)->object_set, VULKAN_DESCRIPTOR_SET_OBJECT, pipeline_layout);
		
		angle += 90.0f * deltaTime;
		render_object_set_transform(renderObject, mat4_rotation(float)(0, angle DEG , 0));
		render_object_draw(renderObject);

		render_pass_end(pass);

		// end command buffer recording
		renderer_end_frame(renderer);

		// submit the work to the GPU and present the rendered image to the window
		renderer_update(renderer);
	}

	render_object_destroy(renderObject);
	render_object_release_resources(renderObject);

	// render_queue_destroy(queue);
	// render_queue_release_resources(queue);

	mesh_destroy(mesh);
	mesh_release_resources(mesh);
	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
