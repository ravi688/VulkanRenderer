
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>
#include <renderer/debug.h>

/* constructors & destructors */
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new()
{
	vulkan_render_pass_pool_t* pool = heap_new(vulkan_render_pass_pool_t);
	memset(pool, 0, sizeof(vulkan_render_pass_pool_t));
	return pool;
}
RENDERER_API void vulkan_render_pass_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_pool_t OUT pool)
{
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

static bool shader_resource_descriptor_compare(void* create_info, void* ref)
{
	vulkan_shader_resource_descriptor_t* descriptor1 = CAST_TO(vulkan_shader_resource_descriptor_t*, create_info);
	vulkan_shader_resource_descriptor_t* descriptor2 = CAST_TO(vulkan_shader_resource_descriptor_t*, ref);
	
	return true;
}

static bool subpass_create_info_compare(void* create_info, void* ref)
{
	vulkan_subpass_create_info_t* create_info1 = CAST_TO(vulkan_subpass_create_info_t*, create_info);
	vulkan_subpass_create_info_t* create_info2 = CAST_TO(vulkan_subpass_create_info_t*, create_info);

	return true;
}

static bool render_pass_create_info_compare(void* create_info, void* ref)
{
	vulkan_render_pass_create_info_t* create_info1 = CAST_TO(vulkan_render_pass_create_info_t*, create_info);
	vulkan_render_pass_create_info_t* create_info2 = CAST_TO(vulkan_render_pass_create_info_t*, ref);

	return true;
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
		log_msg("Creating new vulkan render pass because no render pass exists in the pool with the same create info\n");
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
		log_wrn("Vulkan render pass is already exist in the pool with the same create info\n");
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
