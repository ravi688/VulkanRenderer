
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

RENDERER_API renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen)
{
	renderer_t* renderer = heap_new(renderer_t);
	memset(renderer, 0, sizeof(renderer_t));
	renderer->handle = vulkan_renderer_init((vulkan_renderer_gpu_type_t)gpu_type, width, height, title, full_screen);
	return renderer;
}

RENDERER_API void renderer_terminate(renderer_t* renderer)
{
	vulkan_renderer_terminate(renderer->handle);
	heap_free(renderer);
}

RENDERER_API void renderer_update(renderer_t* renderer)
{
	vulkan_renderer_update(renderer->handle);
}

RENDERER_API bool renderer_is_running(renderer_t* renderer)
{
	vulkan_renderer_is_running(renderer->handle);
}


RENDERER_API void renderer_begin_frame(renderer_t* renderer, float r, float g, float b, float a)
{
	vulkan_renderer_begin_frame(renderer->handle, r, g, b, a);
}

RENDERER_API void renderer_end_frame(renderer_t* renderer)
{
	vulkan_renderer_end_frame(renderer->handle);
}


RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer)
{
	return vulkan_renderer_get_window(renderer->handle);
}

