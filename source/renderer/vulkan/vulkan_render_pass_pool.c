
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/memory_allocator.h>

/* constructors & destructors */
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new()
{
	vulkan_render_pass_pool_t* pool = heap_new(vulkan_render_pass_pool_t);
	memset(pool, 0, sizeof(vulkan_render_pass_pool_t));
	return pool;
}

RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_create(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_pool_t* pool = vulkan_render_pass_pool_new();
	pool->renderer = renderer;
	pool->relocation_table = buf_create(sizeof(vulkan_render_pass_handle_t), 1, 0);
	pool->slots = buf_create(sizeof(vulkan_render_pass_pool_slot_t), 1, 0);
	return pool;
}

RENDERER_API void vulkan_render_pass_pool_destroy(vulkan_render_pass_pool_t* pool)
{
	buf_ucount_t count = buf_get_element_count(&pool->slots);
	for(buf_ucount_t i = 0; i < count; i++)
		vulkan_render_pass_destroy(CAST_TO(vulkan_render_pass_pool_slot_t*, buf_get_ptr_at(&pool->slots, i))->render_pass);
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
	heap_free(pool);
}

/* logic functions */
static bool create_info_compare(void* create_info, void* ref)
{
	vulkan_render_pass_create_info_t* create_info1 = CAST_TO(vulkan_render_pass_create_info_t*, create_info);
	vulkan_render_pass_create_info_t* create_info2 = CAST_TO(vulkan_render_pass_create_info_t*, ref);

	// TODO:

	static u32 counter = 0;
	++counter;
	if(counter == 1)
		return false;
	else
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
	buf_ucount_t handle = buf_find_index_of(&pool->slots, create_info, create_info_compare);
	if(handle == BUF_INVALID_INDEX)
	{
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
	ASSERT_CALLED_ONCE();

	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, vulkan_render_pass_pool_create_slot(pool, create_info));
	if(slot->render_pass == NULL)
		slot->render_pass = vulkan_render_pass_create(pool->renderer, create_info);
	return slot->handle;
}

RENDERER_API vulkan_render_pass_t* vulkan_render_pass_pool_getH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle)
{
	vulkan_render_pass_pool_slot_t* slot = vulkan_render_pass_pool_get_slotH(pool, handle);
	return (slot == NULL) ? NULL : slot->render_pass;
}
