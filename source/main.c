
#include <renderer/renderer.h>

#include <renderer/defines.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>

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

On window resize:
	1. recreate swapchain [done]
	2. update swapchain images [done]
	2. recreate swapchain image views [done]
 */

int main(const char** argc, int argv)
{
	memory_allocator_init(&argv);
	renderer_t* renderer = renderer_init(RENDERER_GPU_TYPE_AUTO, 800, 800, "Multipass Rendering", false);

	while(renderer_is_running(renderer))
	{
		renderer_begin_frame(renderer);

		renderer_end_frame(renderer);

		renderer_update(renderer);
	}

	renderer_terminate(renderer);
	memory_allocator_terminate();
	return 0;
}
