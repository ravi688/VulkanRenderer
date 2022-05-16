
#include <renderer/render_object.h>
#include <renderer/memory_allocator.h>

/* constructors and destructors */
RENDERER_API render_object_t* render_object_new()
{
	return vulkan_render_object_new();
}

RENDERER_API render_object_t* render_object_create(renderer_t* context, render_object_create_info_t* create_info)
{
	vulkan_render_object_create_info_t vcreate_info
	{
		.material = material_get_vulkan_handle(create_info->material),
		.user_data = create_info->user_data,
		.draw_handler = create_info->draw_handler
	};
	return vulkan_render_object_create(context->handle, &vcreate_info);
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

