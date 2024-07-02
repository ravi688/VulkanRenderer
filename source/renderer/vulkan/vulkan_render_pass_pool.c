/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_pool.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/internal/vulkan/vulkan_render_pass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_subpass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <string.h>

/* constructors & destructors */
SGE_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new(memory_allocator_t* allocator)
{
	vulkan_render_pass_pool_t* pool = memory_allocator_alloc_obj(allocator, MEMORy_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_POOL, vulkan_render_pass_pool_t);
	memzero(pool, vulkan_render_pass_pool_t);
	VULKAN_OBJECT_INIT(pool, VULKAN_OBJECT_TYPE_RENDER_PASS_POOL, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return pool;
}
SGE_API void vulkan_render_pass_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_pool_t OUT pool)
{
	VULKAN_OBJECT_MEMZERO(pool, vulkan_render_pass_pool_t);

	pool->renderer = renderer;
	pool->relocation_table = memory_allocator_buf_create(renderer->allocator, sizeof(vulkan_render_pass_handle_t), 1, 0);
	pool->slots = memory_allocator_buf_create(renderer->allocator, sizeof(vulkan_render_pass_pool_slot_t), 1, 0);
	VULKAN_OBJECT_INIT(&pool->pass_graph, VULKAN_OBJECT_TYPE_RENDER_PASS_GRAPH, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_render_pass_graph_create_no_alloc(renderer, &pool->pass_graph);
	log_msg("Vulkan render pass pool has been created successfully\n");
}

SGE_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_create(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_pool_t* pool = vulkan_render_pass_pool_new(renderer->allocator);
	vulkan_render_pass_pool_create_no_alloc(renderer, pool);
	return pool;
}

SGE_API void vulkan_render_pass_pool_destroy(vulkan_render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
		vulkan_render_pass_destroy(CAST_TO(vulkan_render_pass_pool_slot_t*, buf_get_ptr_at(&pool->slots, i))->render_pass);
	vulkan_render_pass_graph_destroy(&pool->pass_graph);
	log_msg("Vulkan render pass pool has been destroyed successfully\n");
}

SGE_API void vulkan_render_pass_pool_release_resources(vulkan_render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_render_pass_pool_slot_t* slot = buf_get_ptr_at(&pool->slots, i);
		vulkan_render_pass_release_resources(slot->render_pass);
		if(slot->create_info->subpass_count > 0)
		{
			for(u32 i = 0; i < slot->create_info->subpass_count; i++)
				if(slot->input.subpass_inputs[i].input_attachment_count > 0)
					memory_allocator_dealloc(pool->renderer->allocator, slot->input.subpass_inputs[i].input_attachments);
			memory_allocator_dealloc(pool->renderer->allocator, slot->input.subpass_inputs);
		}
		if(slot->input.input_attachment_count > 0)
			memory_allocator_dealloc(pool->renderer->allocator, slot->input.input_attachments);
		vulkan_render_pass_create_info_builder_destroy(slot->create_info_builder);
	}
	buf_free(&pool->relocation_table);
	buf_free(&pool->slots);
	vulkan_render_pass_graph_release_resources(&pool->pass_graph);
	if(VULKAN_OBJECT_IS_INTERNAL(pool))
		memory_allocator_dealloc(pool->renderer->allocator, pool);
}

/* logic functions */

static bool compare_struct_fields(u32 count, struct_field_t* f1, struct_field_t* f2)
{
	for(u32 i = 0; i < count; i++)
	{
		bool b1 = strcmp(f1->name, f2->name);
		bool b2 = b1 && (f1->type == f2->type);
		bool b3 = b2 && (f1->size == f2->size);
		bool b4 = b3 && (f1->alignment == f2->alignment);
		if(!b4)
			return false;
	}
	return true;
}

static bool struct_descriptor_compare(struct_descriptor_t* d1, struct_descriptor_t* d2)
{
	bool b1 = strcmp(d1->name, d2->name) == 0;
	bool b2 = b1 && (d1->type == d2->type);
	bool b3 = b2 && (d1->field_count == d2->field_count) && compare_struct_fields(d1->field_count, d1->fields, d2->fields);
	return b3;
}

static bool shader_resource_descriptor_compare(void* create_info, void* ref)
{
	vulkan_shader_resource_description_t* descriptor1 = CAST_TO(vulkan_shader_resource_description_t*, create_info);
	vulkan_shader_resource_description_t* descriptor2 = CAST_TO(vulkan_shader_resource_description_t*, ref);
	return (memcmp(descriptor1, descriptor1, sizeof(vulkan_shader_resource_description_t)) == 0)
			&& struct_descriptor_compare(&descriptor1->handle, &descriptor2->handle);
}

static bool compare_attachment_descriptions(u32 count, VkAttachmentDescription* d1, VkAttachmentDescription* d2)
{
	for(u32 i = 0; i < count; i++)
	{
		if(memcmp(&d1[0], &d2[0], sizeof(VkAttachmentDescription)) != 0)
			return false;
	}
	return true;
}

static bool compare_attachment_usages(u32 count, vulkan_attachment_next_pass_usage_t* d1, vulkan_attachment_next_pass_usage_t* d2)
{
	for(u32 i = 0; i < count; i++)
	{
		if(memcmp(&d1[i], &d2[i], sizeof(vulkan_attachment_next_pass_usage_t)) != 0)
			return false;
	}
	return true;
}

UNUSED_FUNCTION static bool compare_supplementary_attachments(u32 count, VkImageView* a1, VkImageView* a2)
{
	for(u32 i = 0; i < count; i++)
	{
		if(memcmp(&a1[i], &a2[i], sizeof(VkImageView)) != 0)
			return false;
	}
	return true;
}

static bool compare_attachment_reference(u32 count, VkAttachmentReference* r1, VkAttachmentReference* r2)
{
	for(u32 i = 0; i < count; i++)
	{
		if(memcmp(&r1[i], &r2[i], sizeof(VkAttachmentReference)) != 0)
			return false;
	}
	return true;
}

static bool compare_resource_descriptors(u32 count, vulkan_shader_resource_description_t* d1, vulkan_shader_resource_description_t* d2)
{
	for(u32 i = 0; i < count; i++)
	{
		if(!shader_resource_descriptor_compare(&d1[i], &d2[i]))
			return false;
	}
	return true;
}

static bool compare_subpass_create_infos(u32 count, vulkan_subpass_create_info_t* lc1, vulkan_subpass_create_info_t* lc2)
{
	for(u32 i = 0; i < count; i++)
	{
		vulkan_subpass_create_info_t* c1 = &lc1[i];
		vulkan_subpass_create_info_t* c2 = &lc2[i];
		bool b1 = c1->pipeline_bind_point == c2->pipeline_bind_point;
		bool b2 = b1 && (c1->color_attachment_count == c2->color_attachment_count) 
				&& compare_attachment_reference(c1->color_attachment_count, c1->color_attachments, c2->color_attachments);
		bool b3 = b2 && (c1->input_attachment_count == c2->input_attachment_count)
				&& compare_attachment_reference(c1->input_attachment_count, c1->input_attachments, c2->input_attachments);
		bool b4;
		if((c1->depth_stencil_attachment != NULL) && (c2->depth_stencil_attachment != NULL))
			b4 = b3 && (memcmp(c1->depth_stencil_attachment, c2->depth_stencil_attachment, sizeof(VkAttachmentReference)) == 0);
		else
			b4 = b3 && ((c1->depth_stencil_attachment == NULL) && (c2->depth_stencil_attachment == NULL));
		bool b5 = b4 && (c1->preserve_attachment_count == c2->preserve_attachment_count)
				&& (memcmp(c1->preserve_attachments, c2->preserve_attachments, sizeof(u32) * c1->preserve_attachment_count) == 0);
		bool b6 = b5 && (c1->sub_render_set_binding_count == c2->sub_render_set_binding_count)
				&& compare_resource_descriptors(c1->sub_render_set_binding_count, c1->sub_render_set_bindings, c2->sub_render_set_bindings);
		if(!b6)
			return false;
	}
	return true;
}

static bool render_pass_create_info_compare(void* create_info, void* ref)
{
	vulkan_render_pass_create_info_t* c1 = CAST_TO(vulkan_render_pass_create_info_t*, create_info);
	vulkan_render_pass_create_info_t* c2 = CAST_TO(vulkan_render_pass_pool_slot_t*, ref)->create_info;
	bool b1 = c1->framebuffer_count == c2->framebuffer_count;
	bool b2 = b1 && (c1->framebuffer_layout_description.attachment_count == c2->framebuffer_layout_description.attachment_count)
			&& compare_attachment_descriptions(c1->framebuffer_layout_description.attachment_count, c1->framebuffer_layout_description.attachment_descriptions, c2->framebuffer_layout_description.attachment_descriptions);
	bool b3 = b2 && compare_attachment_usages(c1->framebuffer_layout_description.attachment_count, c1->framebuffer_layout_description.attachment_usages, c2->framebuffer_layout_description.attachment_usages);
	// bool b4 = b3 && (c1->supplementary_attachment_count == c2->supplementary_attachment_count)
	// 		&& compare_supplementary_attachments(c1->supplementary_attachment_count, c1->vo_supplementary_attachments, c2->vo_supplementary_attachments);
	bool b5 = b3 && (c1->subpass_count == c2->subpass_count) 
			&& compare_subpass_create_infos(c1->subpass_count, c1->subpasses, c2->subpasses);
	bool b6 = b5 && (c1->render_set_binding_count == c2->render_set_binding_count)
			&& compare_resource_descriptors(c1->render_set_binding_count, c1->render_set_bindings, c2->render_set_bindings);
	return b6;
}

static vulkan_render_pass_create_info_builder_t* vulkan_render_pass_create_info_deep_copy(memory_allocator_t* allocator, vulkan_render_pass_create_info_t* src)
{
	vulkan_render_pass_create_info_builder_t* builder = vulkan_render_pass_create_info_builder_create(allocator);
	vulkan_render_pass_create_info_builder_add(builder, 1);
	vulkan_render_pass_create_info_builder_bind(builder, 0);

	/* clone vulkan_framebuffer_attachments_layout_description_t */
	vulkan_render_pass_create_info_builder_set_framebuffer_count(builder, src->framebuffer_count);
	vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(builder, src->framebuffer_layout_description.supplementary_attachment_bucket_count, src->framebuffer_layout_description.supplementary_attachment_bucket_depth);
	if(src->framebuffer_layout_description.supplementary_attachment_count > 0)
		vulkan_render_pass_create_info_builder_set_supplementary_attachments(builder, 0, src->framebuffer_layout_description.vo_supplementary_attachments, src->framebuffer_layout_description.supplementary_attachment_count);
	if(src->framebuffer_layout_description.attachment_description_count > 0)
	{
		vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, src->framebuffer_layout_description.attachment_descriptions, src->framebuffer_layout_description.attachment_description_count);
		_debug_assert__(src->framebuffer_layout_description.attachment_usages != NULL);
		vulkan_render_pass_create_info_builder_add_attachment_usages(builder, src->framebuffer_layout_description.attachment_usages, src->framebuffer_layout_description.attachment_description_count);
	}

	/* clone subpass create infos */
	_debug_assert__(src->subpass_count > 0);
	if(src->subpass_count > 0)
	{
		vulkan_subpass_create_info_builder_t* subpass_builder = vulkan_subpass_create_info_builder_create(allocator);
		vulkan_subpass_create_info_builder_add(subpass_builder, src->subpass_count);
		for(u32 i = 0; i < src->subpass_count; i++)
		{
			AUTO subpass = &src->subpasses[i];
			vulkan_subpass_create_info_builder_bind(subpass_builder, i);
			vulkan_subpass_create_info_builder_set_bind_point(subpass_builder, subpass->pipeline_bind_point);
			if(subpass->color_attachment_count > 0)
				vulkan_subpass_create_info_builder_add_color_attachments(subpass_builder, subpass->color_attachments, subpass->color_attachment_count);
			if(subpass->input_attachment_count > 0)
				vulkan_subpass_create_info_builder_add_input_attachments(subpass_builder, subpass->input_attachments, subpass->input_attachment_count);
			if(subpass->depth_stencil_attachment != NULL)
				vulkan_subpass_create_info_builder_set_depth_stencil_attachment(subpass_builder, *subpass->depth_stencil_attachment);
			if(subpass->preserve_attachment_count > 0)
				vulkan_subpass_create_info_builder_add_preserve_attachments(subpass_builder, subpass->preserve_attachments, subpass->preserve_attachment_count);
			if(subpass->sub_render_set_binding_count > 0)
			{
				vulkan_shader_resource_description_builder_t* srd_builder = vulkan_shader_resource_description_builder_create_inverse(allocator, subpass->sub_render_set_bindings, subpass->sub_render_set_binding_count);
				vulkan_subpass_create_info_builder_set_render_set_bindings_builder(subpass_builder, srd_builder, true);
			}
		}

		vulkan_render_pass_create_info_builder_set_subpasses_builder(builder, subpass_builder, true);
	}

	/* clone subpass dependencies */
	if(src->subpass_dependency_count > 0)
		vulkan_render_pass_create_info_builder_add_dependencies(builder, src->subpass_dependencies, src->subpass_dependency_count);

	/* clone render set bindings */
	if(src->render_set_binding_count > 0)
	{
		vulkan_shader_resource_description_builder_t* srd_builder = vulkan_shader_resource_description_builder_create_inverse(allocator, src->render_set_bindings, src->render_set_binding_count);
		vulkan_render_pass_create_info_builder_set_render_set_bindings_builder(builder, srd_builder, true);
	}

	return builder;
}

static vulkan_render_pass_handle_t vulkan_render_pass_pool_create_slot(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_input_info_t* input)
{	
	buf_ucount_t handle = buf_find_index_of(&pool->slots, create_info, render_pass_create_info_compare);
	if(handle == BUF_INVALID_INDEX)
	{
		// generate a new handle
		handle = buf_get_element_count(&pool->relocation_table);
		buf_ucount_t index = buf_get_element_count(&pool->slots);
		buf_push(&pool->relocation_table, &index);

		debug_log_info("Creating new Render pass (handle: %llu)", handle);

		memory_allocator_t* allocator = pool->renderer->allocator;
		vulkan_render_pass_create_info_builder_t* copy_create_info_builder = vulkan_render_pass_create_info_deep_copy(allocator, create_info);

		vulkan_render_pass_pool_slot_t slot = 
		{
			.handle = handle,
			.create_info = vulkan_render_pass_create_info_builder_get(copy_create_info_builder),
			.create_info_builder = copy_create_info_builder,
			.render_pass = NULL
		};

		/* deep copy the vulkan_renderpass_input_info_t object */
		if(input->input_attachment_count != 0)
		{
			slot.input.input_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, input->input_attachment_count);
			slot.input.input_attachment_count = input->input_attachment_count;
			memcopyv(slot.input.input_attachments, input->input_attachments, u32, input->input_attachment_count);
		}

		if(input->subpass_inputs != NULL)
		{
			slot.input.subpass_inputs = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_INPUT_INFO_ARRAY, vulkan_subpass_input_info_t, create_info->subpass_count);
			for(u32 i = 0; i < create_info->subpass_count; i++)
			{
				slot.input.subpass_inputs[i].input_attachments = (input->subpass_inputs[i].input_attachment_count == 0) ? NULL : memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, input->subpass_inputs[i].input_attachment_count);
				slot.input.subpass_inputs[i].input_attachment_count = input->subpass_inputs[i].input_attachment_count;
				if(slot.input.subpass_inputs[i].input_attachments != NULL)
					memcopyv(slot.input.subpass_inputs[i].input_attachments, input->subpass_inputs[i].input_attachments, u32, input->subpass_inputs[i].input_attachment_count);
			}
		}

		buf_push(&pool->slots, &slot);
		return handle;
	}
	else
	{
		log_msg("Returning handle to the existing Render pass\n");
	}
	// if the slot already exists then return the slot's handle
	return CAST_TO(vulkan_render_pass_pool_slot_t*, buf_get_ptr_at(&pool->slots, handle))->handle;
}

SGE_API vulkan_render_pass_pool_slot_t* vulkan_render_pass_pool_get_slotH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_handle_t index;
	buf_get_at(&pool->relocation_table, handle, &index);
	return buf_get_ptr_at(&pool->slots, index);
}

SGE_API void vulkan_render_pass_pool_create_path(vulkan_render_pass_pool_t* pool)
{
	vulkan_render_pass_graph_create_path(&pool->pass_graph);
	pool->prev_pass_handle = VULKAN_RENDER_PASS_HANDLE_INVALID;
}

SGE_API vulkan_render_pass_handle_t vulkan_render_pass_pool_create_pass(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_input_info_t* input)
{
	/* create a render pass if not already exists */
	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, vulkan_render_pass_pool_create_slot(pool, create_info, input));
	if(slot->render_pass == NULL)
	{
		slot->render_pass = vulkan_render_pass_create(pool->renderer, create_info);
		slot->render_pass->handle = slot->handle;
	}

	// register this render pass to all the cameras
	vulkan_camera_system_t* camera_system = pool->renderer->camera_system;
	u32 camera_count = vulkan_camera_system_get_count(camera_system);
	for(u32 i = 0; i < camera_count; i++)
		vulkan_camera_register_render_pass(vulkan_camera_system_get_at(camera_system, i), slot->render_pass, vulkan_render_pass_pool_get_prev_pass_handle(pool),  &slot->create_info->framebuffer_layout_description);
	
	vulkan_render_pass_graph_add(&pool->pass_graph, slot->handle);
	debug_log_info("RenderPassPool: Add");
	vulkan_render_pass_graph_dump(&pool->pass_graph);

	pool->prev_pass_handle = slot->handle;

	return slot->handle;
}

SGE_API vulkan_render_pass_t* vulkan_render_pass_pool_getH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, handle);
	return (slot == NULL) ? NULL : slot->render_pass;
}
