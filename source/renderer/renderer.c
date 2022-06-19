
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

#include <renderer/shader_library.h>
#include <renderer/material_library.h>
#include <renderer/render_pass_pool.h>

RENDERER_API renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	renderer_t* renderer = heap_new(renderer_t);
	memset(renderer, 0, sizeof(renderer_t));
	
	// create the renderer
	renderer->vulkan_handle = vulkan_renderer_init(CAST_TO(vulkan_renderer_gpu_type_t, gpu_type), width, height, title, full_screen, resizable);

	return renderer;
}

RENDERER_API void renderer_terminate(renderer_t* renderer)
{
	// terminate the renderer
	vulkan_renderer_terminate(renderer->vulkan_handle);
	
	heap_free(renderer);
}

RENDERER_API void renderer_update(renderer_t* renderer)
{
	vulkan_renderer_update(renderer->vulkan_handle);
}

RENDERER_API bool renderer_is_running(renderer_t* renderer)
{
	vulkan_renderer_is_running(renderer->vulkan_handle);
}


RENDERER_API void renderer_begin_frame(renderer_t* renderer)
{
	vulkan_renderer_begin_frame(renderer->vulkan_handle);
}

RENDERER_API void renderer_end_frame(renderer_t* renderer)
{
	vulkan_renderer_end_frame(renderer->vulkan_handle);
}

/* getters */
RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer)
{
	return vulkan_renderer_get_window(renderer->vulkan_handle);
}

RENDERER_API shader_library_t* renderer_get_shader_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->shader_library;
}

RENDERER_API material_library_t* renderer_get_material_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->material_library;
}

RENDERER_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer)
{
	return renderer->vulkan_handle->render_pass_pool;
}

RENDERER_API camera_system_t* renderer_get_camera_system(renderer_t* renderer)
{
	return renderer->vulkan_handle->camera_system;
}
