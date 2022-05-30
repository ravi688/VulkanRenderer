
#define RENDERER_INCLUDE_DEBUG
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

/*
	1. Default clear screen render pass after creating a camera [done]
	2. Test Shader & Material, Default Shaders [in-progress]
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	
	// initialize renderer
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_AUTO, 800, 800, "Renderer", false, true);

	// create a camera
	AUTO camera = camera_create(renderer, CAMERA_PROJECTION_TYPE_PERSPECTIVE, 0.04f, 100, 65);

	AUTO pool = renderer_get_render_pass_pool(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	AUTO pass = render_pass_pool_getH(pool, render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN));
	AUTO shadowPass = render_pass_pool_getH(pool, render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP));
	AUTO shadowPass2 = render_pass_pool_getH(pool, render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_SHADOW_MAP));
	AUTO shaderH = shader_library_create_shader_from_preset(slib, SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR);
	AUTO shader = shader_library_getH(slib, shaderH);
	// AUTO material = material_library_getH(mlib, material_library_create_material(mlib, shaderH));

	while(renderer_is_running(renderer))
	{
		// begin command buffer recording
		renderer_begin_frame(renderer);

		render_pass_begin(shadowPass, NULL);
		render_pass_end(shadowPass);

		render_pass_begin(pass, NULL);
		
		render_pass_end(pass);

		// end command buffer recording
		renderer_end_frame(renderer);

		// submit the work to the GPU and present the rendered image to the window
		renderer_update(renderer);
	}

	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
