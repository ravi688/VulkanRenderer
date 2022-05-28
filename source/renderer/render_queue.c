
#include <renderer/render_queue.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>

RENDERER_API render_queue_t* render_queue_new()
{
	return vulkan_render_queue_new();
}

RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, const char* name)
{
	return vulkan_render_queue_create(renderer->vulkan_handle, name);
}

RENDERER_API void render_queue_create_no_alloc(renderer_t* renderer, const char* name, render_queue_t OUT queue)
{
	vulkan_render_queue_create_no_alloc(renderer->vulkan_handle, name, queue);
}

RENDERER_API void render_queue_destroy(render_queue_t* queue)
{
	vulkan_render_queue_destroy(queue);
}

RENDERER_API void render_queue_release_resources(render_queue_t* queue)
{
	vulkan_render_queue_release_resources(queue);
}


/* logic functions */
RENDERER_API void render_queue_add(render_queue_t* queue, render_object_t* obj)
{
	vulkan_render_queue_add(queue, obj);
}

RENDERER_API void render_queue_remove(render_queue_t* queue, render_object_t* obj)
{
	vulkan_render_queue_remove(queue, obj);
}

RENDERER_API void render_queue_build(render_queue_t* queue)
{
	vulkan_render_queue_build(queue);
}

RENDERER_API void render_queue_dispatch(render_queue_t* queue)
{
	vulkan_render_queue_dispatch(queue);
}

