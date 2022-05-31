
#include <renderer/internal/vulkan/vulkan_render_queue.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_render_object.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_render_queue_t* vulkan_render_queue_new()
{
	vulkan_render_queue_t* queue = heap_new(vulkan_render_queue_t);
	memset(queue, 0, sizeof(vulkan_render_queue_t));
	return queue;
}

RENDERER_API vulkan_render_queue_t* vulkan_render_queue_create(vulkan_renderer_t* renderer, const char* name)
{
	vulkan_render_queue_t* queue = vulkan_render_queue_new();
	vulkan_render_queue_create_no_alloc(renderer, name, queue);
	return queue;
}

typedef BUFFER subpass_shader_list_t;
typedef BUFFER render_object_list_t;
typedef dictionary_t material_and_render_object_list_map_t;

RENDERER_API void vulkan_render_queue_create_no_alloc(vulkan_renderer_t* renderer, const char* name, vulkan_render_queue_t OUT queue)
{
	memzero(queue, vulkan_render_queue_t);

	queue->renderer = renderer;
	queue->name = string_create(name);
	assert(sizeof(vulkan_render_pass_handle_t) == sizeof(buf_ucount_t));
	queue->render_pass_handles = dictionary_create(vulkan_render_pass_handle_t, subpass_shader_list_t*, 1, dictionary_key_comparer_buf_ucount_t);
	assert(sizeof(vulkan_shader_handle_t) == sizeof(buf_ucount_t));
	queue->shader_handles = dictionary_create(vulkan_shader_handle_t, material_and_render_object_list_map_t, 1, dictionary_key_comparer_buf_ucount_t);
	queue->is_ready = false;
}

RENDERER_API void vulkan_render_queue_destroy(vulkan_render_queue_t* queue)
{
	// TODO
	queue->is_ready = false;
}

RENDERER_API void vulkan_render_queue_release_resources(vulkan_render_queue_t* queue)
{
	string_destroy(&queue->name);
	buf_ucount_t count = dictionary_get_count(&queue->render_pass_handles);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		subpass_shader_list_t* list = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr_at(&queue->render_pass_handles, i));
		u32 subpass_count = vulkan_render_pass_pool_getH(queue->renderer->render_pass_pool, DEREF_TO(vulkan_render_pass_handle_t, dictionary_get_key_ptr_at(&queue->render_pass_handles, i)))->subpass_count;
		for(u32 j = 0; j < subpass_count; j++)
			buf_free(&list[i]);
		heap_free(list);
	}
	dictionary_free(&queue->render_pass_handles);

	count = dictionary_get_count(&queue->shader_handles);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_and_render_object_list_map_t* map = dictionary_get_value_ptr_at(&queue->shader_handles, i);
		buf_ucount_t count1 = dictionary_get_count(map);
		for(buf_ucount_t j = 0; j < count1; j++)
			buf_free(dictionary_get_value_ptr_at(map, i));
		dictionary_free(map);
	}
	dictionary_free(&queue->shader_handles);
	heap_free(queue);
}

RENDERER_API void vulkan_render_queue_add(vulkan_render_queue_t* queue, vulkan_render_object_t* obj)
{
	if(!dictionary_contains(&queue->shader_handles, &obj->material->shader->handle))
	{
		material_and_render_object_list_map_t map = dictionary_create(vulkan_material_handle_t, render_object_list_t, 1, dictionary_key_comparer_buf_ucount_t);
		dictionary_add(&queue->shader_handles, &obj->material->shader->handle, &map);
	}

	material_and_render_object_list_map_t* map = dictionary_get_value_ptr_at(&queue->shader_handles, obj->material->shader->handle);
	if(!dictionary_contains(map, &obj->material->handle))
	{
		render_object_list_t list = buf_create(sizeof(vulkan_render_object_t*), 1, 0);
		dictionary_add(map, &obj->material->handle, &list);
	}
	
	render_object_list_t* list = dictionary_get_value_ptr(map, &obj->material->handle);
	if(buf_find_index_of(list, &obj, buf_ptr_comparer) == BUF_INVALID_INDEX)
		buf_push(list, &obj);

	u32 pass_count = obj->material->shader->render_pass_count;
	vulkan_shader_render_pass_t* passes = obj->material->shader->render_passes;
	for(u32 i = 0; i < pass_count; i++)
	{
		subpass_shader_list_t* lists;
		u32 subpass_count = passes[i].subpass_count;
		if(!dictionary_contains(&queue->render_pass_handles, &passes[i].handle))
		{
			lists = heap_newv(subpass_shader_list_t, subpass_count);
			for(u32 j = 0; j < subpass_count; j++)
				lists[j] = buf_create(sizeof(vulkan_shader_handle_t), 1, 0);
			dictionary_add(&queue->render_pass_handles, &passes[i].handle, &lists);
		}
		else
			lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr(&queue->render_pass_handles, &passes[i].handle));
		for(u32 j = 0; j < subpass_count; j++)
		{
			if(buf_find_index_of(&lists[j], &obj->material->shader->handle, buf_u64_comparer) == BUF_INVALID_INDEX)
				buf_push(&lists[j], &obj->material->shader->handle);
		}
	}
}

RENDERER_API void vulkan_render_queue_remove(vulkan_render_queue_t* queue, vulkan_render_object_t* obj)
{
	LOG_FETAL_ERR("For now removing an object from the render queue is not allowed\n");
}

RENDERER_API void vulkan_render_queue_build(vulkan_render_queue_t* queue)
{
	// TODO
	// queue->is_ready = true;
}

RENDERER_API void vulkan_render_queue_dispatch(vulkan_render_queue_t* queue)
{
	// ASSERT_WRN(queue->is_ready, "Render Queue isn't ready but you are still trying to dispatch it\n");
	// get the pointers to render pass pool, shader library and material library
	vulkan_render_pass_pool_t* pass_pool = queue->renderer->render_pass_pool;
	vulkan_shader_library_t* shader_library = queue->renderer->shader_library;
	vulkan_material_library_t* material_library = queue->renderer->material_library;

	// get the number of unique render passes in this render queue
	buf_ucount_t pass_count = dictionary_get_count(&queue->render_pass_handles);
	
	// iterate through each render pass
	for(int i = 0; i < pass_count; i++)
	{
		// get the handle
		vulkan_render_pass_handle_t pass_handle = DEREF_TO(vulkan_render_pass_handle_t, dictionary_get_key_ptr_at(&queue->render_pass_handles, i));

		// get the pointer to the render pass object
		vulkan_render_pass_t* pass = vulkan_render_pass_pool_getH(pass_pool, pass_handle);

		// begin the render pass
		vulkan_render_pass_begin(pass, queue->renderer->swapchain->current_image_index);
		
		// get the number of sub passes that this render pass has
		u32 sub_pass_count = pass->subpass_count;
		
		// get the list of shader handles for each sub pass in this render pass		
		subpass_shader_list_t* lists = DEREF_TO(subpass_shader_list_t*, dictionary_get_value_ptr_at(&queue->render_pass_handles, i));
	
		// iterate through each subpass
		for(u32 j = 0; j < sub_pass_count;)
		{
			// get the number of shaders in this sub pass 'j'
			u32 shader_count = buf_get_element_count(&lists[j]);

			for(u32 k = 0; k < shader_count; k++)
			{
				vulkan_shader_handle_t shader_handle = DEREF_TO(vulkan_shader_handle_t, buf_get_ptr_at(&lists[j], k));
				vulkan_shader_t* shader = vulkan_shader_library_getH(shader_library, shader_handle);

				// get the graphics pipeline for this render pass and sub pass 'j'
				vulkan_graphics_pipeline_t* pipeline = vulkan_shader_get_pipeline(shader, pass->handle, j);
				// bind pipeline
				vulkan_graphics_pipeline_bind(pipeline);
				
				// get the pipeline layout for this render pass and sub pass 'j'
				vulkan_pipeline_layout_t* pipeline_layout = vulkan_shader_get_pipeline_layout(shader, pass->handle, j);

				// bind GLOBAL_SET
				vulkan_descriptor_set_bind(&queue->renderer->global_set, VULKAN_DESCRIPTOR_SET_GLOBAL, pipeline_layout); 
				// bind RENDER_SET
				vulkan_descriptor_set_bind(&pass->render_set, VULKAN_DESCRIPTOR_SET_RENDER, pipeline_layout);
				// bind SUB_RENDER_SET
				vulkan_descriptor_set_bind(&pass->sub_render_sets[j], VULKAN_DESCRIPTOR_SET_SUB_RENDER, pipeline_layout);

				// get the material handles for this shader
				material_and_render_object_list_map_t* materials = dictionary_get_value_ptr(&queue->shader_handles, &shader_handle);
				u32 material_count = dictionary_get_count(materials);

				// iterate through each material
				for(int l = 0; l < material_count; l++)
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
					for(int m = 0; m < object_count; m++)
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
