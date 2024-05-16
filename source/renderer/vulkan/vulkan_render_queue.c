/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_queue.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass_graph.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

RENDERER_API vulkan_render_queue_t* vulkan_render_queue_new(memory_allocator_t* allocator)
{
	vulkan_render_queue_t* queue = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_QUEUE, vulkan_render_queue_t);
	memzero(queue, vulkan_render_queue_t);
	VULKAN_OBJECT_INIT(queue, VULKAN_OBJECT_TYPE_RENDER_QUEUE, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return queue;
}

RENDERER_API vulkan_render_queue_t* vulkan_render_queue_create(vulkan_renderer_t* renderer, vulkan_render_queue_type_t type)
{
	vulkan_render_queue_t* queue = vulkan_render_queue_new(renderer->allocator);
	vulkan_render_queue_create_no_alloc(renderer, type, queue);
	return queue;
}

typedef BUFFER subpass_shader_list_t;
typedef BUFFER render_object_list_t;
typedef dictionary_t material_and_render_object_list_map_t;

RENDERER_API void vulkan_render_queue_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_queue_type_t type, vulkan_render_queue_t OUT queue)
{
	VULKAN_OBJECT_MEMZERO(queue, vulkan_render_queue_t);

	queue->handle = VULKAN_RENDER_QUEUE_HANDLE_INVALID;
	queue->type = type;
	queue->renderer = renderer;
	_debug_assert__(sizeof(vulkan_render_pass_handle_t) == sizeof(buf_ucount_t));
	queue->render_pass_handles = dictionary_create(vulkan_render_pass_handle_t, subpass_shader_list_t*, 1, dictionary_key_comparer_buf_ucount_t);
	_debug_assert__(sizeof(vulkan_shader_handle_t) == sizeof(buf_ucount_t));
	queue->shader_handles = dictionary_create(vulkan_shader_handle_t, material_and_render_object_list_map_t, 1, dictionary_key_comparer_buf_ucount_t);
	queue->is_ready = false;
	VULKAN_OBJECT_INIT(&queue->pass_graph, VULKAN_OBJECT_TYPE_RENDER_PASS_GRAPH, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_render_pass_graph_create_no_alloc(renderer, &queue->pass_graph);
}

RENDERER_API void vulkan_render_queue_destroy(vulkan_render_queue_t* queue)
{
	buf_ucount_t count = dictionary_get_count(&queue->render_pass_handles);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		subpass_shader_list_t* lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr_at(&queue->render_pass_handles, i));
		u32 subpass_count = vulkan_render_pass_pool_getH(queue->renderer->render_pass_pool, DEREF_TO(vulkan_render_pass_handle_t, dictionary_get_key_ptr_at(&queue->render_pass_handles, i)))->subpass_count;
		for(u32 j = 0; j < subpass_count; j++)
			buf_free(&lists[j]);
		memory_allocator_dealloc(queue->renderer->allocator, lists);
	}
	dictionary_clear(&queue->render_pass_handles);

	count = dictionary_get_count(&queue->shader_handles);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_and_render_object_list_map_t* map = dictionary_get_value_ptr_at(&queue->shader_handles, i);
		buf_ucount_t count1 = dictionary_get_count(map);
		for(buf_ucount_t j = 0; j < count1; j++)
		{
			render_object_list_t* list = dictionary_get_value_ptr_at(map, j);
			buf_ucount_t count2 = buf_get_count(list);
			for(buf_ucount_t k = 0; k < count2; k++)
			{
				vulkan_render_object_t* obj;
				buf_get_at(list, k, &obj);
				obj->queue = NULL;
				obj->handle = VULKAN_RENDER_OBJECT_HANDLE_INVALID;
			}
			buf_free(list);
		}
		dictionary_free(map);
	}
	dictionary_clear(&queue->shader_handles);

	// TODO
	queue->is_ready = false;

	vulkan_render_pass_graph_destroy(&queue->pass_graph);
}

RENDERER_API void vulkan_render_queue_release_resources(vulkan_render_queue_t* queue)
{
	dictionary_free(&queue->render_pass_handles);
	dictionary_free(&queue->shader_handles);

	vulkan_render_pass_graph_release_resources(&queue->pass_graph);

	if(VULKAN_OBJECT_IS_INTERNAL(queue))
		memory_allocator_dealloc(queue->renderer->allocator, queue);
}

RENDERER_API void vulkan_render_queue_destroy_all_objects(vulkan_render_queue_t* queue)
{
	buf_ucount_t count = dictionary_get_count(&queue->shader_handles);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_and_render_object_list_map_t* map = dictionary_get_value_ptr_at(&queue->shader_handles, i);
		buf_ucount_t count1 = dictionary_get_count(map);
		for(buf_ucount_t j = 0; j < count1; j++)
		{
			render_object_list_t* list = dictionary_get_value_ptr_at(map, j);
			buf_ucount_t count2 = buf_get_element_count(list);
			// WARNING: This won't work because destroying a render object automatically removes it from the queue resulting in the element count decrease
			// for(u32 k = 0; k < count2; k++)
			// 	vulkan_render_object_destroy(DEREF_TO(vulkan_render_object_t*, buf_get_ptr_at(list, k)));

			for(u32 k = 0; k < count2; k++)
			{
				AUTO obj = DEREF_TO(vulkan_render_object_t*, buf_peek_ptr(list));
				vulkan_render_object_destroy(obj);
				vulkan_render_object_release_resources(obj);
			}
			buf_clear(list, NULL);
		}
	}
}

RENDERER_API vulkan_render_object_handle_t vulkan_render_queue_add(vulkan_render_queue_t* queue, vulkan_render_object_t* obj)
{
	obj->queue = queue;
	obj->handle = VULKAN_RENDER_OBJECT_HANDLE_INVALID;

	/* if no material is attached then this object is not renderable (not visible at all, so no need to add it into the render queue */
	if(obj->material == NULL)
		return VULKAN_RENDER_OBJECT_HANDLE_INVALID;

	/* if this shader doesn't exists then add it */
	if(!dictionary_contains(&queue->shader_handles, &obj->material->shader->handle))
	{
		material_and_render_object_list_map_t map = dictionary_create(vulkan_material_handle_t, render_object_list_t, 1, dictionary_key_comparer_buf_ucount_t);
		dictionary_add(&queue->shader_handles, &obj->material->shader->handle, &map);
	}

	/* if this material doesn't exists then add it */
	material_and_render_object_list_map_t* map = dictionary_get_value_ptr(&queue->shader_handles, &obj->material->shader->handle);
	if(!dictionary_contains(map, &obj->material->handle))
	{
		render_object_list_t list = memory_allocator_buf_create(queue->renderer->allocator, sizeof(vulkan_render_object_t*), 1, 0);
		dictionary_add(map, &obj->material->handle, &list);
	}

	/* if this object doesn't exists then add it */
	render_object_list_t* list = dictionary_get_value_ptr(map, &obj->material->handle);
	if(buf_find_index_of(list, &obj, buf_ptr_comparer) == BUF_INVALID_INDEX)
		buf_push(list, &obj);

	vulkan_render_pass_graph_create_path(&queue->pass_graph);

	u32 pass_count = obj->material->shader->render_pass_count;
	vulkan_shader_render_pass_t* passes = obj->material->shader->render_passes;
	for(u32 i = 0; i < pass_count; i++)
	{
		subpass_shader_list_t* lists;
		u32 subpass_count = passes[i].subpass_count;

		/* if this render pass doesn't exist then add it */
		if(!dictionary_contains(&queue->render_pass_handles, &passes[i].handle))
		{
			lists = memory_allocator_alloc_obj_array(queue->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_SUBPASS_SHADER_LIST_ARRAY, subpass_shader_list_t, subpass_count);
			for(u32 j = 0; j < subpass_count; j++)
				lists[j] = memory_allocator_buf_create(queue->renderer->allocator, sizeof(vulkan_shader_handle_t), 1, 0);
			dictionary_add(&queue->render_pass_handles, &passes[i].handle, &lists);
			vulkan_render_pass_graph_add(&queue->pass_graph, passes[i].handle);
		}
		else
			lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr(&queue->render_pass_handles, &passes[i].handle));

		for(u32 j = 0; j < subpass_count; j++)
		{
			/* if this shader doesn't exist then add it */
			if(buf_find_index_of(&lists[j], &obj->material->shader->handle, buf_u64_comparer) == BUF_INVALID_INDEX)
				buf_push(&lists[j], &obj->material->shader->handle);
		}
	}
	obj->handle = obj;
	return obj;
}

// TODO
RENDERER_API void vulkan_render_queue_remove_materialH();
RENDERER_API void vulkan_render_queue_remove_render_passH();
RENDERER_API void vulkan_render_queue_remove_shaderH();

RENDERER_API void vulkan_render_queue_removeH(vulkan_render_queue_t* queue, vulkan_render_object_handle_t handle)
{
	_debug_assert__(handle != VULKAN_RENDER_OBJECT_HANDLE_INVALID);
	vulkan_render_object_t* object = handle;
	material_and_render_object_list_map_t* map = dictionary_get_value_ptr(&queue->shader_handles, &object->material->shader->handle);
	render_object_list_t* list = dictionary_get_value_ptr(map, &object->material->handle);
	buf_ucount_t index = buf_find_index_of(list, &object, buf_ptr_comparer);
	if(index == BUF_INVALID_INDEX)
		LOG_WRN("remove failed, render object isn't found in the queue\n");
	buf_remove_at(list, index, NULL);
	// object->queue = NULL;
	object->handle = VULKAN_RENDER_OBJECT_HANDLE_INVALID;
}

RENDERER_API void vulkan_render_queue_build(vulkan_render_queue_t* queue)
{
	debug_log_info("Building optimized render path ...");
	vulkan_render_pass_graph_dump(&queue->pass_graph);
	CAN_BE_UNUSED_VARIABLE vulkan_render_pass_graph_node_handle_list_t* pass_node_handles = vulkan_render_pass_graph_get_or_build_optimized_path(&queue->pass_graph);
DEBUG_BLOCK
(
	printf("Optimized Render Path: ");
	if(buf_get_element_count(pass_node_handles) != 0)
		buf_traverse_elements(pass_node_handles, 0, buf_get_element_count(pass_node_handles) - 1, buf_u32_print, NULL);
	else
		printf("<empty>");
	puts("");
)
	queue->is_ready = true;
}

RENDERER_API void vulkan_render_queue_dispatch(vulkan_render_queue_t* queue, vulkan_camera_t* camera)
{
	debug_assert_wrn__(queue->is_ready, "Render Queue isn't ready but you are still trying to dispatch it");

	vulkan_render_pass_pool_t* pass_pool = queue->renderer->render_pass_pool;

	/* if the camera is rendering only depth values then use the builtin depth shader and depth render pass */
	if(vulkan_camera_is_depth_render_only(camera))
	{
		_debug_assert__(camera->depth_material != NULL);

		vulkan_material_t* material = camera->depth_material;
		vulkan_shader_t* shader = material->shader;
		vulkan_shader_render_pass_counter_reset(shader);
		u32 pass_count = shader->render_pass_count;
		for(u32 i = 0; i < pass_count; i++)
		{
			vulkan_render_pass_handle_t pass_handle = shader->render_passes[i].handle;
			vulkan_render_pass_t* pass = vulkan_render_pass_pool_getH(pass_pool, pass_handle);
			_debug_assert__(shader->render_passes[i].subpass_count == pass->subpass_count);

			vulkan_render_pass_begin(pass, 0, camera);

			u32 subpass_count = pass->subpass_count;
			for(u32 j = 0; j < subpass_count;)
			{
				vulkan_render_pass_handle_t prev_pass_handle = vulkan_shader_get_prev_pass_handle(shader);

				if(!vulkan_shader_render_pass_is_next(shader, pass_handle))
				{
					debug_log_warning("builtins/depth_shader.v3dshader must be a compatible shader for depth rendering");
					continue;
				}

				vulkan_graphics_pipeline_t* pipeline = vulkan_shader_get_pipeline(shader, pass_handle, j);

				_debug_assert__((pipeline != NULL) && pipeline->render_pass == pass);
				_debug_assert__(pipeline->render_pass->handle == pass->handle);

				vulkan_pipeline_layout_t* layout = vulkan_shader_get_pipeline_layout(shader, pass_handle, j);

				vulkan_graphics_pipeline_bind(pipeline);

				/* bind CAMERA_SET */
				vulkan_descriptor_set_bind(&camera->sets[camera->current_shot_index], VULKAN_DESCRIPTOR_SET_CAMERA, layout);
				/* bind GLOBAL_SET */
				vulkan_descriptor_set_bind(&queue->renderer->global_set, VULKAN_DESCRIPTOR_SET_GLOBAL, layout);

				vulkan_render_pass_descriptor_sets_t* render_pass_descriptor_sets = vulkan_camera_get_descriptor_sets(camera, prev_pass_handle, pass_handle);
				/* bind RENDER_SET */
				vulkan_descriptor_set_bind(&render_pass_descriptor_sets->render_set, VULKAN_DESCRIPTOR_SET_RENDER, layout);
				/* bind SUB_RENDER_SET */
				_debug_assert__(j < render_pass_descriptor_sets->sub_render_set_count);
				vulkan_descriptor_set_bind(&render_pass_descriptor_sets->sub_render_sets[j], VULKAN_DESCRIPTOR_SET_SUB_RENDER, layout);

				/* bind MATERIAL_SET */
				vulkan_descriptor_set_bind(&material->material_set, VULKAN_DESCRIPTOR_SET_MATERIAL, layout);

				/* push constants from CPU memory to the GPU memory */
				vulkan_material_push_constants(material, layout);

				/* draw all the objects */

				u32 shader_count = dictionary_get_count(&queue->shader_handles);

				for(u32 k = 0; k < shader_count; k++)
				{
					material_and_render_object_list_map_t* map = dictionary_get_value_ptr_at(&queue->shader_handles, k);
					u32 material_count = dictionary_get_count(map);
					for(u32 l = 0; l < material_count; l++)
					{
						render_object_list_t* objects = dictionary_get_value_ptr_at(map, l);
						u32 object_count = buf_get_element_count(objects);
						for(u32 m = 0; m < object_count; m++)
						{
							vulkan_render_object_t* object = DEREF_TO(vulkan_render_object_t*, buf_get_ptr_at(objects, m));
							/* bind OBJECT_SET */
							vulkan_descriptor_set_bind(&object->object_set, VULKAN_DESCRIPTOR_SET_OBJECT, layout);
							/* draw the object */
							vulkan_render_object_draw(object);
						}
					}
				}

				j++;

				if(j < subpass_count)
					vulkan_render_pass_next(pass);
			}
			vulkan_render_pass_end(pass);
		}

		return;
	}


	// get the pointers to shader library and material library
	vulkan_shader_library_t* shader_library = queue->renderer->shader_library;
	vulkan_material_library_t* material_library = queue->renderer->material_library;
	vulkan_render_pass_graph_node_handle_list_t* pass_node_handles = vulkan_render_pass_graph_get_or_build_optimized_path(&queue->pass_graph);

	/* get the number of pass runs for this render queue */
	u32 pass_count = buf_get_element_count(pass_node_handles);

	/* if there is passes to be run then first reset the pass counters for each shader registered in this render queue */
	if(pass_count > 0)
	{
		u32 unique_pass_count = dictionary_get_count(&queue->render_pass_handles);
		for(u32 i = 0; i < unique_pass_count; i++)
		{
			DEBUG_BLOCK
			(
				vulkan_render_pass_handle_t pass_handle;
				dictionary_get_key_at(&queue->render_pass_handles, i, &pass_handle);
				_debug_assert__(vulkan_render_pass_pool_getH(pass_pool, pass_handle)->subpass_count > 0);
			)

			subpass_shader_list_t* lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr_at(&queue->render_pass_handles, i));

			BUFFER* shaders = &lists[0];
			u32 shader_count = buf_get_element_count(shaders);
			for(u32 k = 0; k < shader_count; k++)
				vulkan_shader_render_pass_counter_reset(vulkan_shader_library_getH(shader_library, DEREF_TO(vulkan_shader_handle_t, buf_get_ptr_at(shaders, k))));
		}
	}

	// iterate through each render pass
	for(u32 i = 0; i < pass_count; i++)
	{
		/* get render pass handle */
		vulkan_render_pass_graph_node_handle_t pass_node_handle;
		buf_get_at_s(pass_node_handles, i, &pass_node_handle);
		vulkan_render_pass_handle_t pass_handle;
		{
			vulkan_render_pass_graph_node_t pass_node;
			buf_get_at_s(&queue->pass_graph.nodes, pass_node_handle, &pass_node);
			pass_handle = pass_node.pass_handle;
		}

		// get the pointer to the render pass object
		vulkan_render_pass_t* pass = vulkan_render_pass_pool_getH(pass_pool, pass_handle);

		// begin the render pass
		vulkan_render_pass_begin(pass, queue->renderer->swapchain->current_image_index, camera);

		// get the number of sub passes that this render pass has
		u32 sub_pass_count = pass->subpass_count;

		// get the list of shader handles for each sub pass in this render pass
		subpass_shader_list_t* lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr(&queue->render_pass_handles, &pass_handle));

		// iterate through each subpass
		for(u32 j = 0; j < sub_pass_count;)
		{
			// get the number of shaders in this sub pass 'j'
			u32 shader_count = buf_get_element_count(&lists[j]);

			for(u32 k = 0; k < shader_count; k++)
			{
				vulkan_shader_handle_t shader_handle = DEREF_TO(vulkan_shader_handle_t, buf_get_ptr_at(&lists[j], k));
				vulkan_shader_t* shader = vulkan_shader_library_getH(shader_library, shader_handle);

				/* NOTE: this function must be called before vulkan_shader_render_pass_is_next() function to get correct values */
				vulkan_render_pass_handle_t prev_pass_handle = vulkan_shader_get_prev_pass_handle(shader);

				if(!vulkan_shader_render_pass_is_next(shader, pass_handle))
					/* for now, all shaders for a subpass must be same
					 * meaning if one shader has 3 subpasses then rest of the shaders would also have
					 * 3 subpasses for the same render pass.
					 * hence if one shader's checklist is complete then rest of the all shader's checklists
					 * must also be complete  */
					continue;

				// get the graphics pipeline for this render pass and sub pass 'j'
				vulkan_graphics_pipeline_t* pipeline = vulkan_shader_get_pipeline(shader, pass->handle, j);

				_debug_assert__((pipeline != NULL) && pipeline->render_pass == pass);
				_debug_assert__(pipeline->render_pass->handle == pass->handle);

				// bind pipeline
				vulkan_graphics_pipeline_bind(pipeline);

				// get the pipeline layout for this render pass and sub pass 'j'
				vulkan_pipeline_layout_t* pipeline_layout = vulkan_shader_get_pipeline_layout(shader, pass->handle, j);

				// bind CAMERA_SET
				vulkan_descriptor_set_bind(&camera->sets[camera->current_shot_index], VULKAN_DESCRIPTOR_SET_CAMERA, pipeline_layout);
				// bind GLOBAL_SET
				vulkan_descriptor_set_bind(&queue->renderer->global_set, VULKAN_DESCRIPTOR_SET_GLOBAL, pipeline_layout);

				vulkan_render_pass_descriptor_sets_t* render_pass_descriptor_sets = vulkan_camera_get_descriptor_sets(camera, prev_pass_handle, pass_handle);
				// bind RENDER_SET
				vulkan_descriptor_set_bind(&render_pass_descriptor_sets->render_set, VULKAN_DESCRIPTOR_SET_RENDER, pipeline_layout);
				// bind SUB_RENDER_SET
				_debug_assert__(j < render_pass_descriptor_sets->sub_render_set_count);
				vulkan_descriptor_set_bind(&render_pass_descriptor_sets->sub_render_sets[j], VULKAN_DESCRIPTOR_SET_SUB_RENDER, pipeline_layout);

				// get the material handles for this shader
				material_and_render_object_list_map_t* materials = dictionary_get_value_ptr(&queue->shader_handles, &shader_handle);
				u32 material_count = dictionary_get_count(materials);

				// iterate through each material
				for(u32 l = 0; l < material_count; l++)
				{
					// get the material handle
					vulkan_material_handle_t material_handle = DEREF_TO(vulkan_material_handle_t, dictionary_get_key_ptr_at(materials, l));

					// get the pointer to the material object
					vulkan_material_t* material = vulkan_material_library_getH(material_library, material_handle);

					// bind MATERIAL_SET
					vulkan_descriptor_set_bind(&material->material_set, VULKAN_DESCRIPTOR_SET_MATERIAL, pipeline_layout);

					// push constants from CPU memory to the GPU memory
					vulkan_material_push_constants(material, pipeline_layout);

					// get the render objects for this material
					render_object_list_t* objects = dictionary_get_value_ptr_at(materials, l);
					u32 object_count = buf_get_element_count(objects);

					// iterate through each object
					for(u32 m = 0; m < object_count; m++)
					{
						vulkan_render_object_t* object = DEREF_TO(vulkan_render_object_t*, buf_get_ptr_at(objects, m));

						// bind OBJECT_SET
						vulkan_descriptor_set_bind(&object->object_set, VULKAN_DESCRIPTOR_SET_OBJECT, pipeline_layout);

						// draw the object
						vulkan_render_object_draw(object);
					}
				}
			}

			j++;
			if(j < sub_pass_count)
				vulkan_render_pass_next(pass);
		}
		vulkan_render_pass_end(pass);
	}
}
