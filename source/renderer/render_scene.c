
#include <renderer/render_scene.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>

/* constructors & destructors */
RENDERER_API render_scene_t* render_scene_new()
{
	return vulkan_render_scene_new();
}

RENDERER_API render_scene_t* render_scene_create(renderer_t* renderer, render_scene_create_info_t* create_info)
{
	vulkan_render_scene_create_info_t vcreate_info = 
	{
		.required_queues = CAST_TO(vulkan_render_queue_type_t*, create_info->required_queues),
		.required_queue_count = create_info->required_queue_count
	};
	return vulkan_render_scene_create(renderer->vulkan_handle, &vcreate_info);
}

RENDERER_API void render_scene_create_no_alloc(renderer_t* renderer, render_scene_create_info_t* create_info, render_scene_t OUT scene)
{
	vulkan_render_scene_create_info_t vcreate_info = 
	{
		.required_queues = CAST_TO(vulkan_render_queue_type_t*, create_info->required_queues),
		.required_queue_count = create_info->required_queue_count
	};
	vulkan_render_scene_create_no_alloc(renderer->vulkan_handle, &vcreate_info, scene);	
}

RENDERER_API render_scene_t* render_scene_create_from_preset(renderer_t* renderer, render_scene_preset_type_t type)
{
	return vulkan_render_scene_create_from_preset(renderer->vulkan_handle, CAST_TO(vulkan_render_scene_preset_type_t, type));
}

RENDERER_API void render_scene_destroy(render_scene_t* scene)
{
	vulkan_render_scene_destroy(scene);
}

RENDERER_API void render_scene_release_resources(render_scene_t* scene)
{
	vulkan_render_scene_release_resources(scene);
}

/* logic functions */
RENDERER_API void render_scene_render(render_scene_t* scene)
{
	vulkan_render_scene_render(scene);
}

RENDERER_API render_object_t* render_scene_getH(render_scene_t* scene, render_scene_object_handle_t handle)
{
	return vulkan_render_scene_getH(scene, handle);
}

RENDERER_API render_scene_object_handle_t render_scene_create_object(render_scene_t* scene, render_object_type_t object_type, render_queue_type_t queue_type)
{
	return vulkan_render_scene_create_object(scene, object_type, queue_type);
}

RENDERER_API void render_scene_destroy_objectH(render_scene_t* scene, render_scene_object_handle_t handle)
{
	vulkan_render_scene_destroy_objectH(scene, handle);
}

