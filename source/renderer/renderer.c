
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>

#include <renderer/shader_library.h>
#include <renderer/material_library.h>
#include <renderer/render_pass_pool.h>

RENDERER_API renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen)
{
	renderer_t* renderer = heap_new(renderer_t);
	memset(renderer, 0, sizeof(renderer_t));
	
	// create the renderer
	renderer->handle = vulkan_renderer_init((vulkan_renderer_gpu_type_t)gpu_type, width, height, title, full_screen);
	
	// create the shader and material library
	renderer->shader_library = shader_library_create(renderer);
	renderer->material_library = material_library_create(renderer->shader_library);
	
	// NOTE: for now let's focus on vulkan
	// create render pass pool
	renderer->render_pass_pool = render_pass_pool_create(vulkan_render_pass_create_info_get_hash);	

	return renderer;
}

RENDERER_API void renderer_terminate(renderer_t* renderer)
{
	// terminate the renderer
	vulkan_renderer_terminate(renderer->handle);
	
	// destroy the shader library
	shader_library_destroy(renderer->shader_library);
	material_library_release_resources(renderer->material_library);

	// destroy the material library
	material_library_destory(renderer->material_library);
	shader_library_release_resources(renderer->shader_library);

	// destroy the render pass pool
	render_pass_pool_destroy(renderer->render_pass_pool);
	render_pass_pool_release_resources(renderer->render_pass_pool);

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

/* getters */
RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer)
{
	return vulkan_renderer_get_window(renderer->handle);
}

RENDERER_API shader_library_t* renderer_get_shader_library(renderer_t* renderer)
{
	return renderer->shader_library;
}

RENDERER_API material_library_t* renderer_get_material_library(renderer_t* renderer)
{
	return renderer->material_library;
}

RENDERER_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer)
{
	return renderer->render_pass_pool;
}

