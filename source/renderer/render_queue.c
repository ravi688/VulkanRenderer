
#include <renderer/render_queue.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>

RENDERER_API render_queue_t* render_queue_new(memory_allocator_t* allocator)
{
	return vulkan_render_queue_new(allocator);
}

RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, render_queue_type_t type)
{
	return vulkan_render_queue_create(renderer->vulkan_handle, CAST_TO(vulkan_render_queue_type_t, type));
}

RENDERER_API void render_queue_create_no_alloc(renderer_t* renderer, render_queue_type_t type, render_queue_t OUT queue)
{
	vulkan_render_queue_create_no_alloc(renderer->vulkan_handle, CAST_TO(vulkan_render_queue_type_t, type), queue);
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
RENDERER_API render_object_handle_t render_queue_add(render_queue_t* queue, render_object_t* obj)
{
	return vulkan_render_queue_add(queue, obj);
}

RENDERER_API void render_queue_removeH(render_queue_t* queue, render_object_handle_t handle)
{
	vulkan_render_queue_removeH(queue, handle);
}

RENDERER_API void render_queue_build(render_queue_t* queue)
{
	vulkan_render_queue_build(queue);
}

RENDERER_API void render_queue_dispatch(render_queue_t* queue, camera_t* camera)
{
	vulkan_render_queue_dispatch(queue, CAST_TO(vulkan_camera_t*, camera));
}

