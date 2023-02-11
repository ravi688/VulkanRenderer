
#pragma once


#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>	 		// for vulkan_render_queue_type_t
#include <renderer/internal/vulkan/vulkan_render_object.h> 			// for vulkan_render_object_type_t
#include <renderer/internal/vulkan/vulkan_handles.h> 				// for vulkan_render_scene_handle_t
#include <renderer/dictionary.h>

typedef enum vulkan_render_scene_preset_type_t
{
	VULKAN_RENDER_SCENE_PRESET_TYPE_DEFAULT = 0,
	VULKAN_RENDER_SCENE_PRESET_TYPE_GEOMETRY_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_ALPHA_TESTED_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_OVERLAY_ONLY,
	VULKAN_RENDER_SCENE_PRESET_TYPE_TRANSPARENT_ONLY,
} vulkan_render_scene_preset_type_t;

typedef struct vulkan_render_scene_create_info_t
{
	vulkan_render_queue_type_t* required_queues;
	u32 required_queue_count;
} vulkan_render_scene_create_info_t;

typedef struct vulkan_render_scene_t
{
	vulkan_renderer_t* renderer;
	dictionary_t queues;
} vulkan_render_scene_t;

/* constructors & destructors */
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info);
RENDERER_API void vulkan_render_scene_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info, vulkan_render_scene_t OUT scene);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_preset(vulkan_renderer_t* renderer, vulkan_render_scene_preset_type_t type);
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_mask(vulkan_renderer_t* renderer, u64 mask);
RENDERER_API void vulkan_render_scene_destroy(vulkan_render_scene_t* scene);
RENDERER_API void vulkan_render_scene_release_resources(vulkan_render_scene_t* scene);

/* logic functions */

RENDERER_API void vulkan_render_scene_add_queue(vulkan_render_scene_t* scene, vulkan_render_queue_type_t queue_type);
#define VULKAN_RENDER_SCENE_ALL_QUEUES (~0ULL)
#define VULKAN_RENDER_SCENE_DONT_CARE 1UL
#define VULKAN_RENDER_SCENE_CLEAR 0UL
RENDERER_API void vulkan_render_scene_render(vulkan_render_scene_t* scene, u64 queue_mask, u32 render_flags);
RENDERER_API vulkan_render_object_t* vulkan_render_scene_getH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
RENDERER_API vulkan_render_scene_object_handle_t vulkan_render_scene_create_object(vulkan_render_scene_t* scene, vulkan_render_object_type_t object_type, vulkan_render_queue_type_t queue_type);
RENDERER_API void vulkan_render_scene_destroy_objectH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle);
