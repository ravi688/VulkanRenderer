
#include <renderer/render_object.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/memory_allocator.h>

/* constructors and destructors */
RENDERER_API render_object_t* render_object_new()
{
	return vulkan_render_object_new();
}

RENDERER_API render_object_t* render_object_create(renderer_t* renderer, render_object_create_info_t* create_info)
{
	vulkan_render_object_create_info_t vcreate_info =
	{
		.material = create_info->material,
		.user_data = create_info->user_data,
		.draw_handler = create_info->draw_handler
	};
	return vulkan_render_object_create(renderer->vulkan_handle, &vcreate_info);
}

RENDERER_API void render_object_create_no_alloc(renderer_t* renderer, render_object_create_info_t* create_info, render_object_t OUT object)
{
	vulkan_render_object_create_info_t vcreate_info = 
	{
		.material = create_info->material,
		.user_data = create_info->user_data,
		.draw_handler = create_info->draw_handler
	};
	vulkan_render_object_create_no_alloc(renderer->vulkan_handle, &vcreate_info, object);
}

RENDERER_API void render_object_destroy(render_object_t* object)
{
	vulkan_render_object_destroy(object);
}

RENDERER_API void render_object_release_resources(render_object_t* object)
{
	vulkan_render_object_release_resources(object);
}

RENDERER_API void render_object_draw(render_object_t* object)
{
	vulkan_render_object_draw(object);
}

RENDERER_API void render_object_set_transform(render_object_t* obj, mat4_t(float) transform)
{
	vulkan_render_object_set_transform(obj, transform);
}

RENDERER_API mat4_t(float) render_object_get_transform(render_object_t* obj)
{
	return vulkan_render_object_get_transform(obj);
}

RENDERER_API mat4_t(float) render_object_get_normal(render_object_t* obj)
{
	return vulkan_render_object_get_normal(obj);
}
