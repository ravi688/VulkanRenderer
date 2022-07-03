
#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

#include <renderer/renderer.h>
#include <renderer/memory_allocator.h>

#include <renderer/assert.h>

/* constructors & destructors */
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_new()
{
	vulkan_render_scene_t* scene = heap_new(vulkan_render_scene_t);
	memzero(scene, vulkan_render_scene_t);
	return scene;
}

RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info)
{
	vulkan_render_scene_t* scene = vulkan_render_scene_new();
	vulkan_render_scene_create_no_alloc(renderer, create_info, scene);
	return scene;
}

RENDERER_API void vulkan_render_scene_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info, vulkan_render_scene_t OUT scene)
{
	assert(sizeof(vulkan_render_queue_type_t) == sizeof(s32));
	scene->renderer = renderer;
	scene->queues = dictionary_create(vulkan_render_queue_type_t, vulkan_render_queue_t, 1, dictionary_key_comparer_s32);
	if((create_info == NULL) || (create_info->required_queue_count == 0))
		return;
	for(u32 i = 0; i < create_info->required_queue_count; i++)
	{
		vulkan_render_queue_t queue;
		vulkan_render_queue_create_no_alloc(renderer, create_info->required_queues[i], &queue);
		dictionary_add(&scene->queues, &create_info->required_queues[i], &queue);
	}
}

RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_preset(vulkan_renderer_t* renderer, vulkan_render_scene_preset_type_t type)
{
	BUFFER queue_types = buf_new(vulkan_render_queue_type_t);
	switch(type)
	{
		case VULKAN_RENDER_SCENE_PRESET_TYPE_DEFAULT:
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_BACKGROUND);
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_GEOMETRY);
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_ALPHA_TESTED);
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_GEOMETRY_LAST);
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_TRANSPARENT);
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_OVERLAY);
			break;
		case VULKAN_RENDER_SCENE_PRESET_TYPE_GEOMETRY_ONLY:
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_GEOMETRY);
			break;
		case VULKAN_RENDER_SCENE_PRESET_TYPE_ALPHA_TESTED_ONLY:
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_ALPHA_TESTED);
			break;
		case VULKAN_RENDER_SCENE_PRESET_TYPE_TRANSPARENT_ONLY:
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_TRANSPARENT);
			break;
		case VULKAN_RENDER_SCENE_PRESET_TYPE_OVERLAY_ONLY:
			buf_push_auto(&queue_types, VULKAN_RENDER_QUEUE_TYPE_OVERLAY);
			break;
		default:
			LOG_FETAL_ERR("Unsupport render scene preset type %u\n", type);
	};

	vulkan_render_scene_create_info_t create_info = 
	{
		.required_queues = queue_types.bytes,
		.required_queue_count = queue_types.element_count
	};
	vulkan_render_scene_t* scene = vulkan_render_scene_create(renderer, &create_info);
	buf_free(&queue_types);
	return scene;
}

RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_mask(vulkan_renderer_t* renderer, u64 mask)
{
	BUFFER queue_types = buf_new(vulkan_render_queue_type_t);
	for(int i = VULKAN_RENDER_QUEUE_TYPE_MIN + 1; i < VULKAN_RENDER_QUEUE_TYPE_MAX; i++)
		if(mask & BIT64(i))
			buf_push_auto(&queue_types, i);
	vulkan_render_scene_create_info_t create_info = 
	{
		.required_queues = queue_types.bytes,
		.required_queue_count = queue_types.element_count
	};
	vulkan_render_scene_t* scene = vulkan_render_scene_create(renderer, &create_info);
	buf_free(&queue_types);
	return scene;
}

RENDERER_API void vulkan_render_scene_destroy(vulkan_render_scene_t* scene)
{
	u32 count = dictionary_get_count(&scene->queues);
	for(u32 i = 0; i < count; i++)
	{
		vulkan_render_queue_t* queue = dictionary_get_value_ptr_at(&scene->queues, i);
		vulkan_render_queue_destroy_all_objects(queue);
		vulkan_render_queue_destroy(queue);
	}
}

RENDERER_API void vulkan_render_scene_release_resources(vulkan_render_scene_t* scene)
{
	// TODO
	// u32 count = buf_get_count(&scene->queues);
	// for(u32 i = 0; i < count; i++)
	// 	vulkan_render_queue_release_resources(buf_get_ptr_at(&scene->queues, i));
	dictionary_free(&scene->queues);
	// TODO
	// heap_free(scene);
}


/* logic functions */

RENDERER_API void vulkan_render_scene_add_queue(vulkan_render_scene_t* scene, vulkan_render_queue_type_t queue_type)
{
	if(dictionary_contains(&scene->queues, &queue_type))
	{
		log_wrn("vulkan render scene already has render queue of type %u, but you are still trying to add it\n", queue_type);
		return;
	}
	vulkan_render_queue_t queue;
	vulkan_render_queue_create_no_alloc(scene->renderer, queue_type, &queue);
	dictionary_add(&scene->queues, &queue_type, &queue);
}

RENDERER_API void vulkan_render_scene_render(vulkan_render_scene_t* scene, u64 queue_mask, u32 flags)
{
	vulkan_camera_system_t* camera_system = scene->renderer->camera_system;

	buf_ucount_t camera_count = vulkan_camera_system_get_count(camera_system);

	for(buf_ucount_t h = 0; h < camera_count; h++)
	{
		vulkan_camera_t* camera = vulkan_camera_system_get_at(camera_system, h);
		if(!vulkan_camera_is_active(camera))
			continue;

		u32 clear_flags = 0;
		switch(flags)
		{
			case VULKAN_RENDER_SCENE_DONT_CARE:
				clear_flags = VULKAN_CAMERA_CLEAR_FLAG_DONT_CARE;
			break;
			case VULKAN_RENDER_SCENE_CLEAR:
				clear_flags = VULKAN_CAMERA_CLEAR_FLAG_CLEAR;
			break;
		}
		
		vulkan_camera_begin(camera);
		
		while(vulkan_camera_capture(camera, clear_flags))
		{
			buf_ucount_t count = dictionary_get_count(&scene->queues);
			for(buf_ucount_t i = 0; i < count; i++)
			{
				AUTO queue_type = DEREF_TO(vulkan_render_queue_type_t, dictionary_get_key_ptr_at(&scene->queues, i));
				if((BIT64(queue_type) & queue_mask) == BIT64(queue_type))
					vulkan_render_queue_dispatch(dictionary_get_value_ptr_at(&scene->queues, i), camera);
			}
		}
		
		vulkan_camera_end(camera);
	}
}

RENDERER_API vulkan_render_object_t* vulkan_render_scene_getH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle)
{
	return handle.object_handle;
}

RENDERER_API vulkan_render_scene_object_handle_t vulkan_render_scene_create_object(vulkan_render_scene_t* scene, vulkan_render_object_type_t object_type, vulkan_render_queue_type_t queue_type)
{
	vulkan_render_object_create_info_t create_info = { .type = CAST_TO(vulkan_render_object_type_t, object_type) };
	vulkan_render_object_t* object = vulkan_render_object_create(scene->renderer, &create_info);
	buf_ucount_t queue_index = dictionary_find_index_of(&scene->queues, &REINTERPRET_TO(vulkan_render_queue_type_t, queue_type));
	if(queue_index == DICTIONARY_INVALID_INDEX)
		LOG_FETAL_ERR("render scene doesn't contain the requested queue type %u\n", queue_type);
	vulkan_render_queue_t* queue = dictionary_get_value_ptr_at(&scene->queues, queue_index);
	assert(sizeof(buf_ucount_t) == sizeof(vulkan_render_queue_handle_t));
	vulkan_render_object_handle_t handle = vulkan_render_queue_add(queue, object);
	return (vulkan_render_scene_object_handle_t) 
	{ 
		(handle == VULKAN_RENDER_OBJECT_HANDLE_INVALID) ? object : handle, 
		queue_index 
	};
}

RENDERER_API void vulkan_render_scene_destroy_objectH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle)
{
	vulkan_render_queue_t* queue = dictionary_get_value_ptr_at(&scene->queues, handle.queue_handle);
	vulkan_render_queue_removeH(queue, handle.object_handle);
}

