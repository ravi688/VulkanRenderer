
#include <renderer/render_queue.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

#include <renderer/string.h>
#include <bufferlib/buffer.h>
#include <renderer/dictionary.h>

#include <renderer/internal/vulkan/vulkan_material.h> // for vulkan_material_bind_info_t
#include <renderer/internal/vulkan/vulkan_render_pass.h> 	// for vulkan specific functions

typedef struct render_queue_t
{
	renderer_t* renderer;					// reference to the context
	string_t name; 							// identification name of this render queue
	dictionary_t render_pass_handles; 		// list of render_pass_handle_t
	bool is_ready; 							// true if this render queue is ready to dispatch
} render_queue_t;

RENDERER_API render_queue_t* render_queue_new()
{
	render_queue_t* queue = heap_new(render_queue_t);
	memset(queue, 0, sizeof(render_queue_t));
	return queue;
}

RENDERER_API render_queue_t* render_queue_create(renderer_t* renderer, const char* name)
{
	render_queue_t* queue = render_queue_new();
	queue->renderer = renderer;

	queue->name = string_create(name);
	queue->render_pass_handles = dictionary_create(render_pass_handle_t, dictionary_t*, 1, dictionary_key_comparer_buf_ucount_t);

	// initial the render queue won't be ready to be dispatched
	queue->is_ready = false;
	return queue;
}

RENDERER_API void render_queue_destroy(render_queue_t* queue)
{
	// TODO
	queue->is_ready = false;
}

RENDERER_API void render_queue_release_resources(render_queue_t* queue)
{
	string_destroy(queue->name);
	// TODO
	heap_free(queue);
}

RENDERER_API void render_queue_add(render_queue_t* queue, render_object_t* obj)
{
	// get the render pass handles required for this render object
	shader_handle_t* shaders = buf_get_ptr(&obj->material->shaders);
	buf_ucount_t shader_count = buf_get_element_count(&obj->material->shader_count);

	// iterate through each shader handle that this material uses
	for(int i = 0; i < shader_count; i++)
	{
		// get pointer to the actual shader object
		shader_t* shader = shader_library(obj->renderer->shader_library, shaders[i]);

		// render pass that this shader (graphics pipelines for this render pass) uses
		render_pass_handle_t pass = shader->render_pass;

		// if this render pass doesn't exist in the render queue then add it
		dictionary_t* shader_handles;
		if(!dictionary_contains(&queue->render_pass_handles, &pass))
		{
			// each render pass handle contains list of shader handles that it operates upon
			shader_handles = dictionary_create(shader_handle_t, dictionary_t*, 1, dictionary_key_comparer_buf_ucount_t);
			dictionary_add(&queue->render_pass_handles, &pass, &shader_handles);
		}
		else
			dictionary_get_value(&queue->render_pass_handles, &pass, &shader_handles);
		
		// if this shader doesn't exist in the list of shader handles of this render pass then add it
		dictionary_t* material_handles;
		if(!dictionary_contains(shader_handles, &shaders[i]))
		{
			// each shader handle contains list of material handles that it operates upon
			material_handles = dictionary_create(material_handle_t, BUFFER*, 1, dictionary_key_comparer_buf_ucount_t);
			dictionary_add(shader_handles, &shaders[i], &material_handles);
		}
		else
			dictionary_get_value(shader_handles, &shaders[i], &material_handles);

		// if the object's material doesn't exist in the list of material handles of this shader then add it
		BUFFER* objects;
		if(!dictionary_contains(material_handles, &obj->material_handle))
		{
			// each material handle contains list of render object pointers that it operates upon
			objects = BUFcreate(NULL, sizeof(render_object_t*), 1, 0);
			dictionary_add(material_handles, &obj->material_handle, &objects);
		}
		else
			dictionary_get_value(material_handles, &obj->material_handle, &objects);

		// if this object doesn't exist in the list of the render objects of this material then add it
		if(buf_find_index_of(objects, &obj, buf_ptr_comparer) == BUF_INVALID_INDEX)
			buf_push(objects, &obj);
	}
}

RENDERER_API void render_queue_remove(render_queue_t* queue, render_object_t* obj)
{
	LOG_FETAL_ERR("For now removing an object from the render queue is not allowed\n");
}

RENDERER_API void render_queue_build(render_queue_t* queue)
{
	queue->is_ready = true;
}

/*
	Algorithm
	-----------

	globalSet.bind(); 								// bind global set
	foreach(RenderPass pass in renderPasses)
	{
		begin(pass);
		int subPassIndex = 0;
		while(subPassIndex < pass.subPassCount)
		{
			pass.bindRenderSet(subPassIndex); 		// bind render set
			foreach(Shader shader in pass.shaders)
			{
				shader.bindPipeline(subPassIndex);
				foreach(Material material in shader.materials)
				{
					material.bindMaterialSet(); 	// bind material set
					foreach(Object object in material.objects)
					{
						object.bindObjectSet();	 	// bind object set
						object.drawIndexed();
					}
				}
			}
			if(subPassIndex < pass.subPassCount)
				pass.next();
			++subPassIndex;
		}
		end(pass);
	}

 */

RENDERER_API void render_queue_dispatch(render_queue_t* queue)
{
	assert_wrn(queue->is_ready, "Render Queue isn't ready but you are still trying to dispatch it\n");
	// get the pointers to render pass pool, shader library and material library
	render_pass_pool_t* pass_pool = renderer_get_render_pass_pool(queue->renderer);
	shader_library_t* shader_library = renderer_get_shader_library(queue->renderer);
	material_library_t* material_library = renderer_get_material_library(queue->renderer);

	// get the number of unique render passes in this render queue
	buf_ucount_t pass_count = dictionary_get_count(&queue->render_pass_handles);
	
	// iterate through each render pass
	for(int i = 0; i < pass_count; i++)
	{
		// get the pointer to the render pass object
		render_pass_handle_t handle;
		dictionary_get_key_at(&queue->render_pass_handles, i, &handle);

		render_pass_t* pass = render_pass_pool_getH(pass_pool, handle);

		// begin the render pass
		render_pass_begin(pass);
		
		// get the number of sub passes that this render pass has
		int sub_pass_count = render_pass_get_sub_pass_count(pass_pool);
			
		// get the shader handles for this render pass & this render queue
		dictionary_t* shaders;
		dictionary_get_value_at(&queue->render_pass_handles, i, &shaders);
		int shader_count = dictionary_get_count(shaders);

		// iterate through each subpass
		for(int j = 0; j < sub_pass_count; )
		{
			// bind the render descriptor set for this sub pass at index 'j' [at RENDER_SET]
			shader_handle_t shader_handle;
			dictionary_get_key_at(shaders, 0, &shader_handle);
			vulkan_render_pass_bind_set(pass, j, shader_get_pipeline(shader_library_getH(shader_library, shader_handle), j)->pipeline_layout);

			// iterate through each shader in this render pass for this sub pass at index 'j'
			// NOTE: each shader has multiple pipelines, i.e. for each sub pass, 
			// 		 so every pipeline at index 'k' has identical pipeline layout in each shader
			for(int k = 0; k < shader_count; k++)
			{
				shader_handle_t shader_handle;
				dictionary_get_key_at(shaders, k, &shader_handle);

				// get the pointer to the shader object
				shader_t* shader = shader_library_getH(shader_library, shader_handle);
				
				// bind pipeline for this subpass
				shader_bind_pipeline(shader, j); 		// sub pass index

				vulkan_pipeline_layout_t* pipeline_layout = shader_get_pipeline(shader, j)->pipeline_layout

				// get the material handles for this shader
				dictionary_t* materials;
				dictionary_get_value_at(shaders, k, &materials);
				int material_count = dictionary_get_count(materials);
				
				// iterate through each material
				for(int l = 0; l < material_count; l++)
				{
					material_handle_t material_handle;
					dictionary_get_key_at(materials, l, &material_handle);

					// get the pointer to the material object
					material_t* material = material_library_getH(material_library, material_handle);
					
					// bind the material descriptor set for this sub pass [at MATERIAL_SET]
					vulkan_material_bind_info_t bind_info = { j, pipeline_layout };
					material_bind_resource(material, &bind_info);
					
					// get the render objects for this material
					BUFFER* objects;
					dictionary_get_value_at(materials, l, &objects);
					int object_count = buf_get_element_count(objects);
					
					// iterate through each object
					for(int m = 0; m < object_count; m++)
					{
						render_object_t* object;
						buf_get_at(objects, i, &object);

						// bind the object descriptor set for this object [at OBJECT_SET]
						render_object_bind_resource(object, pipeline_layout);

						// draw the object
						render_object_draw(object);
					}
				}
			}

			j++;
			if(j < sub_pass_count)
				render_pass_next(pass_pool);
		}
		render_pass_end(pass);
	}
}
