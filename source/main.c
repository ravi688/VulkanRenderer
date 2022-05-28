
#include <renderer/renderer.h>

#include <renderer/defines.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>
#include <renderer/render_pass_pool.h>
#include <renderer/render_pass.h>


#include <renderer/camera.h>

/*
 Initializing Renderer:
	1. Render Window [done]
	2. Vulkan Instance [done]
	3. Vulkan Surface [done]
	4. Vulkan Device [done]
	5. Vulkan Queue [done]
	6. Vulkan Swapchain [done]
	7. Vulkan Command Buffers [done]

	8. Global and Object set layouts [done]

	9. Vulkan Shader Library
	10. Vulkan Material Library
	11. Vulkan Render Pass Pool

 Default Renderpass [ color attachment + depth attachment ] [ done ]

On window resize:
	1. recreate swapchain [done]
	2. update swapchain images [done]
	2. recreate swapchain image views [done]
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	AUTO renderer = renderer_init(RENDERER_GPU_TYPE_AUTO, 800, 800, "Vulkan Renderer", false);

	AUTO camera = camera_create(renderer, CAMERA_PROJECTION_TYPE_PERSPECTIVE, 65);

	AUTO pool = renderer_get_render_pass_pool(renderer);
	AUTO pass = render_pass_pool_getH(pool, render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN));


	while(renderer_is_running(renderer))
	{
		renderer_begin_frame(renderer);

		render_pass_begin(pass, NULL);
		
		render_pass_end(pass);
		
		renderer_end_frame(renderer);

		renderer_update(renderer);
	}

	camera_destroy(camera);
	camera_release_resources(camera);

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
