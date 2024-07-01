/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_scene.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <renderer/internal/vulkan/vulkan_render_scene.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_light.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>

#include <renderer/renderer.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

#include <renderer/assert.h>

/* constructors & destructors */
RENDERER_API vulkan_render_scene_t* vulkan_render_scene_new(memory_allocator_t* allocator)
{
	vulkan_render_scene_t* scene = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_SCENE, vulkan_render_scene_t);
	memzero(scene, vulkan_render_scene_t);
	VULKAN_OBJECT_INIT(scene, VULKAN_OBJECT_TYPE_RENDER_SCENE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return scene;
}

RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info)
{
	vulkan_render_scene_t* scene = vulkan_render_scene_new(renderer->allocator);
	vulkan_render_scene_create_no_alloc(renderer, create_info, scene);
	return scene;
}


static bool dictionary_light_type_comparer(void* key_we_passed, void* key_in_dict)
{
	return DREF_TO(vulkan_light_type_t, key_we_passed) == DREF_TO(vulkan_light_type_t, key_in_dict);
}


RENDERER_API void vulkan_render_scene_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_scene_create_info_t* create_info, vulkan_render_scene_t OUT scene)
{
	VULKAN_OBJECT_MEMZERO(scene, vulkan_render_scene_t);
	_debug_assert__(sizeof(vulkan_render_queue_type_t) == sizeof(s32));
	scene->renderer = renderer;
	scene->queues = dictionary_create(vulkan_render_queue_type_t, vulkan_render_queue_t*, 1, dictionary_key_comparer_s32);
	scene->cameras = memory_allocator_buf_new(renderer->allocator, vulkan_camera_t*);
	scene->light_buffer_map =  dictionary_create(vulkan_light_type_t, vulkan_light_buffer_stage_t, 1, dictionary_light_type_comparer);

	/* create descriptor set for the scope of a render scene */
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &renderer->scene_set_layout
	};
	VULKAN_OBJECT_INIT(&scene->scene_set, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, &scene->scene_set);
	
	if((create_info == NULL) || (create_info->required_queue_count == 0))
		return;
	for(u32 i = 0; i < create_info->required_queue_count; i++)
	{
		AUTO queue = vulkan_render_queue_create(renderer, create_info->required_queues[i]);
		dictionary_add(&scene->queues, &create_info->required_queues[i], &queue);
	}
}

RENDERER_API vulkan_render_scene_t* vulkan_render_scene_create_from_preset(vulkan_renderer_t* renderer, vulkan_render_scene_preset_type_t type)
{
	BUFFER queue_types = memory_allocator_buf_new(renderer->allocator, vulkan_render_queue_type_t);
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
	BUFFER queue_types = memory_allocator_buf_new(renderer->allocator, vulkan_render_queue_type_t);
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

static vulkan_render_queue_t* get_queue_at(vulkan_render_scene_t* scene, u32 index)
{
	vulkan_render_queue_t* queue;
	dictionary_get_value_at(&scene->queues, index, &queue);
	return VULKAN_RENDER_QUEUE(queue);
}

RENDERER_API void vulkan_render_scene_destroy(vulkan_render_scene_t* scene)
{
	u32 count = dictionary_get_count(&scene->queues);
	for(u32 i = 0; i < count; i++)
	{
		vulkan_render_queue_t* queue = get_queue_at(scene, i);
		vulkan_render_queue_destroy_all_objects(queue);
		vulkan_render_queue_destroy(queue);
	}
	buf_clear(&scene->cameras, NULL);

	/* destroy light buffers and lists */
	count = dictionary_get_count(&scene->light_buffer_map);
	for(u32 i = 0; i < count; i++)
	{
		AUTO stage = CAST_TO(vulkan_light_buffer_stage_t*, dictionary_get_value_ptr_at(&scene->light_buffer_map, i));
		buf_clear(&stage->lights, NULL);
		vulkan_formatted_buffer_destroy(&stage->light_buffer);
		vulkan_formatted_buffer_release_resources(&stage->light_buffer);
	}
	dictionary_clear(&scene->light_buffer_map);

	vulkan_descriptor_set_destroy(&scene->scene_set);
}

RENDERER_API void vulkan_render_scene_release_resources(vulkan_render_scene_t* scene)
{
	u32 count = dictionary_get_count(&scene->queues);
	for(u32 i = 0; i < count; i++)
		vulkan_render_queue_release_resources(get_queue_at(scene, i));
	dictionary_free(&scene->queues);
	buf_free(&scene->cameras);
	dictionary_free(&scene->light_buffer_map);
	vulkan_descriptor_set_release_resources(&scene->scene_set);	

	if(VULKAN_OBJECT_IS_INTERNAL(scene))
		memory_allocator_dealloc(scene->renderer->allocator, scene);
}


/* logic functions */

RENDERER_API void vulkan_render_scene_create_queue(vulkan_render_scene_t* scene, vulkan_render_queue_type_t queue_type)
{
	if(dictionary_contains(&scene->queues, &queue_type))
	{
		log_wrn("vulkan render scene already has render queue of type %u, but you are still trying to add it\n", queue_type);
		return;
	}
	AUTO queue = vulkan_render_queue_create(scene->renderer, queue_type);
	vulkan_render_scene_add_queue(scene, queue);
}

RENDERER_API void vulkan_render_scene_add_queue(vulkan_render_scene_t* scene, vulkan_render_queue_t* queue)
{
	dictionary_add(&scene->queues, &queue->type, &queue);
	queue->scene = scene;
}

static u32 get_scene_set_light_UBO_binding(vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_POINT: return VULKAN_DESCRIPTOR_BINDING_POINT_LIGHT;
		case VULKAN_LIGHT_TYPE_SPOT: return VULKAN_DESCRIPTOR_BINDING_SPOT_LIGHT;
		case VULKAN_LIGHT_TYPE_FAR: return VULKAN_DESCRIPTOR_BINDING_FAR_LIGHT;
		default:
			DEBUG_LOG_FETAL_ERROR("No UBO binding is defined for vulkan_light_type_t(%u)", type);
	}
	return 0;
}

static u32 get_scene_set_light_shadowmap_binding(vulkan_light_type_t type)
{
	switch(type)
	{
		case VULKAN_LIGHT_TYPE_POINT: return VULKAN_DESCRIPTOR_BINDING_POINT_LIGHT_SHADOWMAP;
		case VULKAN_LIGHT_TYPE_SPOT: return VULKAN_DESCRIPTOR_BINDING_SPOT_LIGHT_SHADOWMAP;
		case VULKAN_LIGHT_TYPE_FAR: return VULKAN_DESCRIPTOR_BINDING_FAR_LIGHT_SHADOWMAP;
		default:
			DEBUG_LOG_FETAL_ERROR("No shadow map binding is defined for vulkan_light_type_t(%u)", type);
	}
	return 0;
}

RENDERER_API void vulkan_render_scene_render(vulkan_render_scene_t* scene, u64 queue_mask, u32 flags)
{
	u32 light_buffer_count = dictionary_get_count(&scene->light_buffer_map);
	for(u32 i = 0; i < light_buffer_count; i++)
	{
		AUTO stage = CAST_TO(vulkan_light_buffer_stage_t*, dictionary_get_value_ptr_at(&scene->light_buffer_map, i));
		AUTO light_type = DREF_TO(vulkan_light_type_t, dictionary_get_key_ptr_at(&scene->light_buffer_map, i));

		/* if lights have been removed or added for this light type */
		if(stage->is_light_list_dirty)
		{
			/* then rebuild buffers and re-write descriptors */


			/* rebuild light buffer */

			/* get reference to the raw buffer of the light buffer */
			buffer_t* buffer = NULL;

			u32 light_count = buf_get_element_count(&stage->lights);

			/* allocate vulkan_texture_t* array for the shadow map descriptor rewrite */
			vulkan_texture_t* shadow_maps[light_count];

			/* iterate over each light of this type and fill the information into light buffer */
			for(u32 j = 0; j < light_count; j++)
			{
				/* get reference to the light */
				vulkan_light_t* light;
				buf_get_at_s(&stage->lights, j, &light);

				/* calling vulkan_formatted_buffer_get_array_buffer makes the vulkan_formatted_buffer_t dirty, 
				 * so only call it when we are confirmed of dirty data */
				if(buffer == NULL)
				{
					buffer = vulkan_formatted_buffer_get_array_buffer(&stage->light_buffer, "lights");
					buf_clear_fast(buffer);
				}

				/* add the light data into this light buffer */
				u32 data_size = vulkan_light_get_dispatchable_data_size(light);
				_debug_assert__(data_size <= buf_get_element_size(buffer));
				u8 bytes[data_size];
				vulkan_light_get_dispatchable_data(light, j, CAST_TO(void*, bytes));
				buf_push_n(buffer, CAST_TO(void*, bytes), data_size);	

				/* we are doing this here to avoid another for-loop when rewriting the shadow map descriptors */
				shadow_maps[j] = vulkan_light_get_shadow_map(light);
			}

DEBUG_BLOCK(
			if(buffer == NULL)
				_debug_assert__(light_count == 0);
)
			u32 array_len = buf_get_element_count(buffer);
			_debug_assert__(array_len == light_count);
			vulkan_formatted_buffer_set_uint(&stage->light_buffer, "count", array_len);

			/* rewrite shadow map descriptors */
			vulkan_descriptor_set_write_texturev(&scene->scene_set, get_scene_set_light_shadowmap_binding(light_type), 0, shadow_maps, light_count);

			stage->is_light_list_dirty = false;
		}
		/* otherwise check for each light if they have dirty data to be updated into corresponding light buffer */
		else
		{
			/* get reference to the raw buffer of the light buffer only if there is something to update */
			buffer_t* buffer = NULL;

			u32 light_count = buf_get_element_count(&stage->lights);
			for(u32 j = 0; j < light_count; j++)
			{
				/* get reference to the light */
				vulkan_light_t* light;
				buf_get_at_s(&stage->lights, j, &light);

				/* if this light has outdated dispatchable data, then updated it into the light buffer */
				if(vulkan_light_is_dirty(light))
				{
					/* calling vulkan_formatted_buffer_get_array_buffer makes the vulkan_formatted_buffer_t dirty, 
					 * so only call it when we are confirmed of dirty data */
					if(buffer == NULL)
						buffer = vulkan_formatted_buffer_get_array_buffer(&stage->light_buffer, "lights");
					u32 data_size = vulkan_light_get_dispatchable_data_size(light);
					u8 bytes[data_size];
					vulkan_light_get_dispatchable_data(light, j, CAST_TO(void*, bytes));
					buf_set_at_n(buffer, j, CAST_TO(void*, bytes), data_size);
				}

				/* if this light has resized shadow map, i.e. recreated VkImage(s) and VkImageView(s) objects */
				if(vulkan_light_is_shadow_map_dirty(light))
				{
					AUTO shadow_map = vulkan_light_get_shadow_map(light);
					vulkan_descriptor_set_write_texturev(&scene->scene_set, get_scene_set_light_shadowmap_binding(light_type), j, &shadow_map, 1);
				}
			}
		}

		/* if the light buffer contains outdated data or more/less data then update the GPU-side buffer */
		if(vulkan_formatted_buffer_is_dirty(&stage->light_buffer))
		{
			bool is_resized = false;
			bool is_updated = vulkan_formatted_buffer_commit(&stage->light_buffer, &is_resized);
			_debug_assert__(is_updated);
			/* if the VkBuffer object has been recreated then we need to re-write the descriptor slot */
			if(is_resized)
				vulkan_descriptor_set_write_uniform_buffer(&scene->scene_set, 
									get_scene_set_light_UBO_binding(light_type), 
									vulkan_formatted_buffer_get_device_buffer(&stage->light_buffer));
		}
	}

	/* if this render scene uses lights then render the shadow maps */
	if(vulkan_render_scene_is_use_lights(scene))
	{
		u32 light_buffer_count = dictionary_get_count(&scene->light_buffer_map);
		for(u32 i = 0; i < light_buffer_count; i++)
		{
			AUTO stage = CAST_TO(vulkan_light_buffer_stage_t*, dictionary_get_value_ptr_at(&scene->light_buffer_map, i));
			/* render to shadow maps first */
			u32 light_count = buf_get_element_count(&stage->lights);
			for(u32 j = 0; j < light_count; j++)
			{
				/* get the light */
				vulkan_light_t* light;
				buf_get_at_s(&stage->lights, j, &light);
	
				/* if this light is not active then do not generate any shadow maps (i.e. do not render to the shadow depth buffer) */
				if((!vulkan_light_is_active(light)) || (!vulkan_light_is_cast_shadow(light)))
					continue;
	
				vulkan_light_begin(light);
				while(vulkan_light_irradiate(light))
				{
					u32 count = dictionary_get_count(&scene->queues);
					for(u32 k = 0; k < count; k++)
					{
						AUTO queue_type = DEREF_TO(vulkan_render_queue_type_t, dictionary_get_key_ptr_at(&scene->queues, k));
						if((BIT64(queue_type) & queue_mask) == BIT64(queue_type))
						vulkan_render_queue_dispatch_single_material(get_queue_at(scene, k), 
										vulkan_light_get_shadow_material(light),
										vulkan_light_get_shadow_camera(light),
										scene);
					}
				}
				vulkan_light_end(light);
			}
		}
	}

	/* then sample the above rendered shadow maps and render a lit scene */
	buf_ucount_t camera_count = buf_get_element_count(&scene->cameras);

	for(buf_ucount_t h = 0; h < camera_count; h++)
	{
		vulkan_camera_t* camera;
		buf_get_at_s(&scene->cameras, h, &camera);
		
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
					vulkan_render_queue_dispatch(get_queue_at(scene, i), camera, scene);
			}
		}
		
		vulkan_camera_end(camera);
	}
}

RENDERER_API vulkan_render_object_t* vulkan_render_scene_getH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle)
{
	return handle.object;
}

RENDERER_API vulkan_render_scene_object_handle_t vulkan_render_scene_create_object(vulkan_render_scene_t* scene, vulkan_render_object_type_t object_type, vulkan_render_queue_type_t queue_type)
{
	vulkan_render_object_create_info_t create_info = { .type = CAST_TO(vulkan_render_object_type_t, object_type) };
	vulkan_render_object_t* object = vulkan_render_object_create(scene->renderer, &create_info);
	buf_ucount_t queue_index = dictionary_find_index_of(&scene->queues, &REINTERPRET_TO(vulkan_render_queue_type_t, queue_type));
	if(queue_index == DICTIONARY_INVALID_INDEX)
		LOG_FETAL_ERR("render scene doesn't contain the requested queue type %u\n", queue_type);
	vulkan_render_queue_t* queue = get_queue_at(scene, queue_index);
	vulkan_render_queue_add(queue, object);
	_debug_assert__(object->material == NULL);
	return (vulkan_render_scene_object_handle_t) 
	{ 
		object, 
		queue 
	};
}

RENDERER_API void vulkan_render_scene_destroy_objectH(vulkan_render_scene_t* scene, vulkan_render_scene_object_handle_t handle)
{
	vulkan_render_queue_removeH(handle.queue, handle.object);
}

RENDERER_API void vulkan_render_scene_build_queues(vulkan_render_scene_t* scene)
{
	debug_log_info("Render Queue Graphs");
	u32 count = dictionary_get_count(&scene->queues);
	for(u32 i = 0; i < count; i++)
	{
		AUTO queue = get_queue_at(scene, i);
		debug_log_info("Queue type: %s", vulkan_render_queue_type_str(queue->type));
		vulkan_render_queue_build(queue);
	}
}

static struct_descriptor_t* get_UBO_format(memory_allocator_t* allocator, struct_descriptor_t* light_properties)
{
	light_properties = struct_descriptor_clone_p(light_properties);
	struct_descriptor_t* desc = struct_descriptor_create(allocator);
	struct_descriptor_begin(allocator, desc, "lights_UBO", GLSL_TYPE_UNIFORM_BUFFER, GLSL_UNIFORM_BUFFER_MEMORY_LAYOUT);
		struct_descriptor_add_field(desc, "count", GLSL_TYPE_UINT);
		struct_descriptor_add_field_array2(desc, "lights", light_properties, U32_MAX);
	struct_descriptor_end(allocator, desc);
	return desc;
}

static vulkan_light_buffer_stage_t* get_light_buffer_stage(vulkan_render_scene_t* scene, vulkan_light_type_t type)
{
	vulkan_light_buffer_stage_t* stage;
	if(dictionary_try_get_value_ptr(&scene->light_buffer_map, &type, CAST_TO(void**, &stage)))
		return stage;
	return NULL;
}

RENDERER_API void vulkan_render_scene_add_light(vulkan_render_scene_t* scene, vulkan_light_t* light)
{ 
	/* if no formatted buffer exists for this light type, i.e. light->type, then create a new one.
	 * otherwise, use the existing one. */
	vulkan_light_buffer_stage_t* stage;
	AUTO type = vulkan_light_get_type(light);
	if(!dictionary_try_get_value_ptr(&scene->light_buffer_map, &type, CAST_TO(void**, &stage)))
	{
		vulkan_formatted_buffer_create_info_t create_info = 
		{
			.format = get_UBO_format(scene->renderer->allocator, vulkan_light_get_struct_def(light)),
			.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			/* internally creates vulkan_host_buffered_buffer_t object */
			.host_buffered_buffer = NULL
		};

		{
			vulkan_light_buffer_stage_t stage = 
			{
				.lights = memory_allocator_buf_new(scene->renderer->allocator, vulkan_light_t*),
			};

		 	/* create formatted buffer */		
		 	VULKAN_OBJECT_INIT(&stage.light_buffer, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
		 	vulkan_formatted_buffer_create_no_alloc(scene->renderer, &create_info, &stage.light_buffer);
			dictionary_add(&scene->light_buffer_map, &type, &stage);
		 }

		stage = CAST_TO(vulkan_light_buffer_stage_t*, dictionary_get_value_ptr(&scene->light_buffer_map, &type));
	}
	buf_push(&stage->lights, &light);
	stage->is_light_list_dirty = true;
}

RENDERER_API void vulkan_render_scene_remove_light(vulkan_render_scene_t* scene, vulkan_light_t* light)
{ 
	vulkan_light_buffer_stage_t* stage = get_light_buffer_stage(scene, vulkan_light_get_type(light));
	_debug_assert__(stage != NULL);
	bool result = buf_remove(&stage->lights, &light, buf_ptr_comparer);
	if(!result)
		DEBUG_LOG_WARNING("You're trying to remove vulkan_light_t from vulkan_render_scene_t which doesn't exists in it");
	else stage->is_light_list_dirty = true; 
}
