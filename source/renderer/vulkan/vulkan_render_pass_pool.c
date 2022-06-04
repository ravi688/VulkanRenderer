
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <string.h>

/* constructors & destructors */
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new()
{
	vulkan_render_pass_pool_t* pool = heap_new(vulkan_render_pass_pool_t);
	memset(pool, 0, sizeof(vulkan_render_pass_pool_t));
	return pool;
}
RENDERER_API void vulkan_render_pass_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_pool_t OUT pool)
{
	memzero(pool, vulkan_render_pass_pool_t);

	pool->renderer = renderer;
	pool->relocation_table = buf_create(sizeof(vulkan_render_pass_handle_t), 1, 0);
	pool->slots = buf_create(sizeof(vulkan_render_pass_pool_slot_t), 1, 0);
	log_msg("Vulkan render pass pool has been created successfully\n");
}

RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_create(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_pool_t* pool = vulkan_render_pass_pool_new();
	vulkan_render_pass_pool_create_no_alloc(renderer, pool);
	return pool;
}

RENDERER_API void vulkan_render_pass_pool_destroy(vulkan_render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
		vulkan_render_pass_destroy(CAST_TO(vulkan_render_pass_pool_slot_t*, buf_get_ptr_at(&pool->slots, i))->render_pass);
	log_msg("Vulkan render pass pool has been destroyed successfully\n");
}

static void vulkan_render_pass_create_info_deep_free(vulkan_render_pass_create_info_t* create_info)
{
	// TODO:
}

RENDERER_API void vulkan_render_pass_pool_release_resources(vulkan_render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_render_pass_pool_slot_t* slot = buf_get_ptr_at(&pool->slots, i);
		vulkan_render_pass_release_resources(slot->render_pass);
		vulkan_render_pass_create_info_deep_free(slot->create_info);
	}
	buf_free(&pool->relocation_table);
	buf_free(&pool->slots);
	// TODO
	// heap_free(pool);
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
	vulkan_shader_resource_descriptor_t* descriptor1 = CAST_TO(vulkan_shader_resource_descriptor_t*, create_info);
	vulkan_shader_resource_descriptor_t* descriptor2 = CAST_TO(vulkan_shader_resource_descriptor_t*, ref);
	return (memcmp(descriptor1, descriptor1, sizeof(vulkan_shader_resource_descriptor_t)) == 0)
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

static bool compare_supplementary_attachments(u32 count, VkImageView* a1, VkImageView* a2)
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

static bool compare_resource_descriptors(u32 count, vulkan_shader_resource_descriptor_t* d1, vulkan_shader_resource_descriptor_t* d2)
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
	bool b2 = b1 && (c1->attachment_count == c2->attachment_count)
			&& compare_attachment_descriptions(c1->attachment_count, c1->attachment_descriptions, c2->attachment_descriptions);
	bool b3 = b2 && compare_attachment_usages(c1->attachment_count, c1->attachment_usages, c2->attachment_usages);
	bool b4 = b3 && (c1->supplementary_attachment_count == c2->supplementary_attachment_count)
			&& compare_supplementary_attachments(c1->supplementary_attachment_count, c1->supplementary_attachments, c2->supplementary_attachments);
	bool b5 = b4 && (c1->subpass_count == c2->subpass_count) 
			&& compare_subpass_create_infos(c1->subpass_count, c1->subpasses, c2->subpasses);
	bool b6 = b5 && (c1->render_set_binding_count == c2->render_set_binding_count)
			&& compare_resource_descriptors(c1->render_set_binding_count, c1->render_set_bindings, c2->render_set_bindings);
	return b6;
}

static void vulkan_resource_descriptor_deep_copy(vulkan_shader_resource_descriptor_t* dst, vulkan_shader_resource_descriptor_t* src)
{
	memcpy(dst, src, sizeof(vulkan_shader_resource_descriptor_t));
	if(src->handle.field_count > 0)
	{
		dst->handle.fields = heap_newv(struct_field_t, src->handle.field_count);
		memcpy(dst->handle.fields, src->handle.fields, sizeof(struct_field_t) * src->handle.field_count);
	}
}

static void vulkan_subpass_create_info_deep_copy(vulkan_subpass_create_info_t* dst, vulkan_subpass_create_info_t* src)
{
	memcpy(dst, src, sizeof(vulkan_subpass_create_info_t));
	dst->color_attachments = src->color_attachment_count ? heap_newv(VkAttachmentReference, src->color_attachment_count) : NULL;
	memcpy(dst->color_attachments, src->color_attachments, sizeof(VkAttachmentReference) * src->color_attachment_count);
	dst->input_attachments = src->input_attachment_count ? heap_newv(VkAttachmentReference, src->input_attachment_count) : NULL;
	memcpy(dst->input_attachments, src->input_attachments, sizeof(VkAttachmentReference) * src->input_attachment_count);
	dst->preserve_attachments = src->preserve_attachment_count ? heap_newv(u32, src->preserve_attachment_count) : NULL;
	memcpy(dst->preserve_attachments, src->preserve_attachments, sizeof(u32) * src->preserve_attachment_count);
			
	dst->sub_render_set_bindings = src->sub_render_set_binding_count ? heap_newv(vulkan_shader_resource_descriptor_t, src->sub_render_set_binding_count) : NULL;
	for(u32 j = 0; j < src->sub_render_set_binding_count; j++)
		vulkan_resource_descriptor_deep_copy(&dst->sub_render_set_bindings[j], &src->sub_render_set_bindings[j]);	
}

static void vulkan_render_pass_create_info_deep_copy(vulkan_render_pass_create_info_t* dst, vulkan_render_pass_create_info_t* src)
{
	memcpy(dst, src, sizeof(vulkan_render_pass_create_info_t));
	dst->attachment_descriptions = src->attachment_description_count ? heap_newv(VkAttachmentDescription, src->attachment_description_count) : NULL;
	memcpy(dst->attachment_descriptions, src->attachment_descriptions, sizeof(VkAttachmentDescription) * src->attachment_description_count);

	assert(src->subpass_count > 0);
	dst->subpasses = heap_newv(vulkan_subpass_create_info_t, src->subpass_count);
	memcpy(dst->subpasses, src->subpasses, sizeof(vulkan_subpass_create_info_t) * src->subpass_count);
	for(u32 i = 0; i < src->subpass_count; i++)
		vulkan_subpass_create_info_deep_copy(&dst->subpasses[i], &src->subpasses[i]);

	dst->render_set_bindings = src->render_set_binding_count ? heap_newv(vulkan_shader_resource_descriptor_t, src->render_set_binding_count) : NULL;
	for(u32 i = 0; i < src->render_set_binding_count; i++)
		vulkan_resource_descriptor_deep_copy(&dst->render_set_bindings[i], &src->render_set_bindings[i]);
}

static vulkan_render_pass_handle_t vulkan_render_pass_pool_create_slot(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info)
{	
	buf_ucount_t handle = buf_find_index_of(&pool->slots, create_info, render_pass_create_info_compare);
	if(handle == BUF_INVALID_INDEX)
	{
		log_msg("Creating new Render pass\n");
		// generate a new handle
		handle = buf_get_element_count(&pool->relocation_table);
		buf_ucount_t index = buf_get_element_count(&pool->slots);
		buf_push(&pool->relocation_table, &index);

		vulkan_render_pass_create_info_t* copy_create_info = heap_new(vulkan_render_pass_create_info_t);
		vulkan_render_pass_create_info_deep_copy(copy_create_info, create_info);

		vulkan_render_pass_pool_slot_t slot = 
		{
			.handle = handle,
			.create_info = copy_create_info,
			.render_pass = NULL
		};
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

static vulkan_render_pass_pool_slot_t* vulkan_render_pass_pool_get_slotH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_handle_t index;
	buf_get_at(&pool->relocation_table, handle, &index);
	return buf_get_ptr_at(&pool->slots, index);
}

RENDERER_API vulkan_render_pass_handle_t vulkan_render_pass_pool_create_pass(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info)
{
	// for now this must be called once
	// ASSERT_CALLED_ONCE();

	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, vulkan_render_pass_pool_create_slot(pool, create_info));
	if(slot->render_pass == NULL)
	{
		slot->render_pass = vulkan_render_pass_create(pool->renderer, create_info);
		slot->render_pass->handle = slot->handle;
	}
	return slot->handle;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_pool_getH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, handle);
	return (slot == NULL) ? NULL : slot->render_pass;
}
