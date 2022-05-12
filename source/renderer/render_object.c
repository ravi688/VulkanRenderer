
#include <renderer/render_object.h>
#include <renderer/memory_allocator.h>

#include <renderer/internal/vulkan/vulkan_pipeline_layout.h> 	// for vulkan_pipeline_layout_t

/* constructors and destructors */
RENDERER_API render_object_t* render_object_new()
{
	return vulkan_render_object_new();
}

RENDERER_API render_object_t* render_object_create(render_object_create_info_t* create_info)
{
	vulkan_render_object_create_info_t vcreate_info
	{
		.material = material_get_vulkan_handle(create_info),
		.user_data = create_info->user_data,
		.draw_handler = create_info->draw_handler,
		.set = create_info->api_resource
	};
	return vulkan_render_object_create(&vcreate_info);
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

RENDERER_API void render_object_bind_resource(render_object_t* object, void* api_resource)
{
	vulkan_render_object_bind_set(object, (vulkan_pipeline_layout_t*)api_resource);
}
