/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_object.c is a part of VulkanRenderer

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


#include <sge/internal/vulkan/vulkan_render_object.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_render_queue.h>
#include <sge/internal/vulkan/vulkan_graphics_pipeline.h>
#include <sge/glsl_memory_layout.h>
#include <glslcommon/glsl_types.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <sge/assert.h>

typedef struct vulkan_mesh_t vulkan_mesh_t;
SGE_API void vulkan_mesh_draw_indexed(vulkan_mesh_t* mesh);
SGE_API void vulkan_mesh_set_material(vulkan_mesh_t* mesh, vulkan_material_t* material);

typedef struct text_mesh_t text_mesh_t;
SGE_API void text_mesh_draw(text_mesh_t* text);
SGE_API void text_mesh_set_material(text_mesh_t* text, vulkan_material_t* material);

typedef struct vulkan_bitmap_text_t vulkan_bitmap_text_t;
SGE_API void vulkan_bitmap_text_draw(vulkan_bitmap_text_t* text);
SGE_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material);


SGE_API vulkan_render_object_t* vulkan_render_object_new(memory_allocator_t* allocator)
{
	vulkan_render_object_t* obj = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_OBJECT, vulkan_render_object_t);
	memzero(obj, vulkan_render_object_t);
	VULKAN_OBJECT_INIT(obj, VULKAN_OBJECT_TYPE_RENDER_OBJECT, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return obj;
}

static void setup_gpu_resources(vulkan_render_object_t* object)
{
	// create descriptor set
	vulkan_descriptor_set_create_info_t set_create_info =
	{
		.vo_pool = object->renderer->vo_descriptor_pool,
		.layout = &object->renderer->object_set_layout
	};
	vulkan_descriptor_set_create_no_alloc_ext(object->renderer, &set_create_info, &object->object_set);

	// setup object struct definiton
	OBJECT_INIT(&object->struct_definition, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);
	struct_descriptor_begin(object->renderer->allocator, &object->struct_definition, "objectInfo", GLSL_TYPE_UNIFORM_BUFFER, GLSL_MEMORY_LAYOUT_CALLBACKS);
		struct_descriptor_add_field(&object->struct_definition, "transform", GLSL_TYPE_MAT4);
		struct_descriptor_add_field(&object->struct_definition, "normal", GLSL_TYPE_MAT4);
	struct_descriptor_end(object->renderer->allocator, &object->struct_definition);

	// create uniform buffers and write to the descriptor set OBJECT_SET at binding TRANSFORM_BINDING
	vulkan_buffer_create_info_t create_info = 
	{
		.size = struct_descriptor_sizeof(&object->struct_definition),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = object->renderer->vo_sharing_mode,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	_debug_assert__(create_info.size == (64 * 2));
	VULKAN_OBJECT_INIT(&object->buffer, VULKAN_OBJECT_TYPE_BUFFER, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_buffer_create_no_alloc(object->renderer, &create_info, &object->buffer);
	struct_descriptor_map(&object->struct_definition, vulkan_buffer_map(&object->buffer));
	vulkan_descriptor_set_write_uniform_buffer(&object->object_set, VULKAN_DESCRIPTOR_BINDING_TRANSFORM, &object->buffer);

	object->transform_handle = struct_descriptor_get_field_handle(&object->struct_definition, "transform");
	object->normal_handle = struct_descriptor_get_field_handle(&object->struct_definition, "normal");
}

SGE_API vulkan_render_object_t* vulkan_render_object_create(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info)
{
	vulkan_render_object_t* object = vulkan_render_object_new(renderer->allocator);
	vulkan_render_object_create_no_alloc(renderer, create_info, object);
	return object;
}
SGE_API void vulkan_render_object_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info, vulkan_render_object_t OUT object)
{
	VULKAN_OBJECT_MEMZERO(object, vulkan_render_object_t);

	object->renderer = renderer;
	object->is_active = true;
	object->material = create_info->material;
	object->user_data = create_info->user_data;

	switch(create_info->type)
	{
		case VULKAN_RENDER_OBJECT_TYPE_MESH:
			object->draw = CAST_TO(draw_call_handler_t, vulkan_mesh_draw_indexed);
			object->set_material = CAST_TO(material_set_handler_t, vulkan_mesh_set_material);
			break;
		case VULKAN_RENDER_OBJECT_TYPE_TEXT_MESH:
			object->draw = CAST_TO(draw_call_handler_t, text_mesh_draw);
			object->set_material = CAST_TO(material_set_handler_t, text_mesh_set_material);
			break;
		case VULKAN_RENDER_OBJECT_TYPE_TEXT: 	// TODO
			object->draw = CAST_TO(draw_call_handler_t, vulkan_bitmap_text_draw);
			object->set_material = CAST_TO(material_set_handler_t, vulkan_bitmap_text_set_material);
			break;
		default:
			if(create_info->draw_handler == NULL)
				LOG_FETAL_ERR("Unsupported vulkan render object type %u and draw_handler is also NULL\n", create_info->type);
			object->draw = create_info->draw_handler;
	}

	setup_gpu_resources(object);
	
	vulkan_render_object_set_transform(object, mat4_identity());
}

SGE_API void vulkan_render_object_destroy(vulkan_render_object_t* obj)
{
	if(obj->queue != NULL)
		vulkan_render_queue_removeH(obj->queue, obj);
	vulkan_descriptor_set_destroy(&obj->object_set);
	struct_descriptor_unmap(&obj->struct_definition);
	vulkan_buffer_unmap(&obj->buffer);
	vulkan_buffer_destroy(&obj->buffer);
}

SGE_API void vulkan_render_object_release_resources(vulkan_render_object_t* obj)
{
	struct_descriptor_destroy(obj->renderer->allocator, &obj->struct_definition);
	vulkan_descriptor_set_release_resources(&obj->object_set);
	vulkan_buffer_release_resources(&obj->buffer);
	if(VULKAN_OBJECT_IS_INTERNAL(obj))
		memory_allocator_dealloc(obj->renderer->allocator, obj);
}

SGE_API void vulkan_render_object_attach(vulkan_render_object_t* obj, void* user_data)
{
	obj->user_data = user_data;
	if((user_data != NULL) && (obj->set_material != NULL) && (obj->material != NULL))
		obj->set_material(obj->user_data, obj->material);
}

SGE_API void vulkan_render_object_draw(vulkan_render_object_t* obj, vulkan_graphics_pipeline_t* pipeline)
{
	AUTO command_buffer = obj->renderer->vo_command_buffers[obj->renderer->current_frame_index];
	/* vkCmdSetScissor only works if the scissor is in the list of dynamic states. 
	 * so we need to check here if the graphics pipeline supports dynamic scissor */
	if(vulkan_graphics_pipeline_is_scissor_dynamic(pipeline))
	{
		/* if this render object has got a custom scissor then use that one. */
		if(obj->scissor_rect.has_value)
		{
			VkRect2D vk_rect = 
			{ 
				.offset = { obj->scissor_rect.value.offset.x, obj->scissor_rect.value.offset.y },
				.extent = { obj->scissor_rect.value.extent.width, obj->scissor_rect.value.extent.height }
			};
			vkCmdSetScissor(command_buffer, 0, 1, &vk_rect);
		}
		/* otherwise, fallback to the one used by graphics pipeline. 
		 * NOTE: this is needed, because previous vkCmdSetScissor calls modify the state of scissor and remains as that 
		 * until another vkCmdSetScissor changes it again. */
		else
		{
			VkRect2D vk_rect = vulkan_graphics_pipeline_get_scissor_rect(pipeline);
			vkCmdSetScissor(command_buffer, 0, 1, &vk_rect);
		}
	}
	obj->draw(obj->user_data);
}

SGE_API void vulkan_render_object_set_material(vulkan_render_object_t* obj, vulkan_material_t* material)
{
	if(obj->material == material) return;
	/* cache the queue as vulkan_render_queue_removeH internally sets the obj->queue to NULL */
	AUTO queue = obj->queue;
	/* remove the outdated data from the render queue if this object had been added earlier and material was non-null
	 * because, if material was null but still the user tried to add this render object into a queue, then the queue would have 
	 * ignored the addition request because the object was not-renderable */
	if((queue != NULL) && (obj->material != NULL))
		vulkan_render_queue_removeH(obj->queue, obj);
	/* update the material of this object */
	obj->material = material;
	/* now add the object again into the render queue with the updated data (material) */
	if(queue != NULL)
		vulkan_render_queue_add(queue, obj);
	
	if(obj->user_data != NULL)
		obj->set_material(obj->user_data, material);
}

SGE_API vulkan_material_t* vulkan_render_object_get_material(vulkan_render_object_t* obj)
{
	return obj->material;
}

SGE_API void vulkan_render_object_set_transform(vulkan_render_object_t* obj, mat4_t transform)
{
	obj->transform = transform;
	obj->normalMatrix = mat4_inverse(transform);
	mat4_move(transform, mat4_transpose(transform));
	struct_descriptor_set_mat4(&obj->struct_definition, obj->transform_handle, CAST_TO(float*, &transform));
	struct_descriptor_set_mat4(&obj->struct_definition, obj->normal_handle, CAST_TO(float*, &obj->normalMatrix));
}

SGE_API mat4_t vulkan_render_object_get_transform(vulkan_render_object_t* obj)
{
	return obj->transform;
}

SGE_API mat4_t vulkan_render_object_get_normal(vulkan_render_object_t* obj)
{
	return obj->normalMatrix;
}
