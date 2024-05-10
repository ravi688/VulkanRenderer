#include <renderer/internal/vulkan/vulkan_subpass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_subpass_create_info_builder_t* vulkan_subpass_create_info_builder_create(memory_allocator_t* allocator)
{
	vulkan_subpass_create_info_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_CREATE_INFO_BUILDER, vulkan_subpass_create_info_builder_t);
	memzero(builder, vulkan_subpass_create_info_builder_t);
	builder->allocator = allocator;
	builder->create_info_array = memory_allocator_buf_new(allocator, vulkan_subpass_create_info_t);
	builder->build_info_array = memory_allocator_buf_new(allocator, vulkan_subpass_create_info_build_info_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API void vulkan_subpass_create_info_destroy(vulkan_subpass_create_info_builder_t* builder)
{
	buf_free(&builder->create_info_array);
	u32 build_info_count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < build_info_count; i++)
	{
		AUTO build_info = buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_subpass_create_info_build_info_t, i);
		buf_free(&build_info->color_attachments);
		buf_free(&build_info->input_attachments);
		buf_free(&build_info->preserve_attachments);
	}
	buf_free(&builder->build_info_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_subpass_create_info_builder_add(vulkan_subpass_create_info_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->create_info_array, count);
	buf_push_pseudo(&builder->build_info_array, count);
	AUTO build_info = CAST_TO(vulkan_subpass_create_info_build_info_t*, buf_peek_ptr(&builder->build_info_array));
	build_info->color_attachments = memory_allocator_buf_new(builder->allocator, VkAttachmentReference);
	build_info->is_color_attachments_internal = true;
	build_info->input_attachments = memory_allocator_buf_new(builder->allocator, VkAttachmentReference);
	build_info->is_input_attachments_internal = true;
	build_info->preserve_attachments = memory_allocator_buf_new(builder->allocator, u32);
	build_info->is_preserve_attachments_internal = true;
}

RENDERER_API void vulkan_subpass_create_info_builder_bind(vulkan_subpass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	builder->bind_index = index;
}

static vulkan_subpass_create_info_build_info_t* get_build_info(vulkan_subpass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_subpass_create_info_build_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_subpass_create_info_build_info_t* get_bound_build_info(vulkan_subpass_create_info_builder_t* builder)
{
	return get_build_info(builder, builder->bind_index);
}

static vulkan_subpass_create_info_t* get_create_info(vulkan_subpass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->create_info_array, vulkan_subpass_create_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_subpass_create_info_t* get_bound_create_info(vulkan_subpass_create_info_builder_t* builder)
{
	return get_create_info(builder, builder->bind_index);
}


RENDERER_API vulkan_subpass_create_info_t* vulkan_subpass_create_info_builder_get(vulkan_subpass_create_info_builder_t* builder)
{
	if(buf_get_element_count(&builder->build_info_array) == 0)
		return NULL;
	_debug_assert__(buf_get_element_count(&builder->build_info_array) == buf_get_element_count(&builder->create_info_array));
	u32 count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO build_info = get_build_info(builder, i);
		AUTO create_info = get_create_info(builder, i);
		/* color attachments */
		create_info->color_attachment_count = buf_get_element_count(&build_info->color_attachments);
		if(create_info->color_attachment_count > 0)
			create_info->color_attachments = CAST_TO(VkAttachmentReference*, buf_get_ptr(&build_info->color_attachments));
		else
			create_info->color_attachments = NULL;
		/* input attachments */
		create_info->input_attachment_count = buf_get_element_count(&build_info->input_attachments);
		if(create_info->input_attachment_count > 0)
			create_info->input_attachments = CAST_TO(VkAttachmentReference*, buf_get_ptr(&build_info->input_attachments));
		else
			create_info->input_attachments = NULL;
		/* depth stencil attachment */
		if(build_info->is_depth_stencil_attachment_set)
			create_info->depth_stencil_attachment = &build_info->depth_stencil_attachment;
		else
			create_info->depth_stencil_attachment = NULL;
		/* preserve attachments */
		create_info->preserve_attachment_count = buf_get_element_count(&build_info->preserve_attachments);
		if(create_info->preserve_attachment_count > 0)
			create_info->preserve_attachments = CAST_TO(u32*, buf_get_ptr(&build_info->preserve_attachments));
		else
			create_info->preserve_attachments = NULL;
		/* sub render set bindings */
		if(build_info->is_use_render_set_bindings_builder)
		{
			create_info->sub_render_set_bindings = vulkan_shader_resource_description_builder_get(build_info->render_set_bindings_builder);
			create_info->sub_render_set_binding_count = vulkan_shader_resource_description_builder_get_count(build_info->render_set_bindings_builder);
		}
	}
	return CAST_TO(vulkan_subpass_create_info_t*, buf_get_ptr(&builder->create_info_array));
}

RENDERER_API u32 vulkan_subpass_create_info_builder_get_count(vulkan_subpass_create_info_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->build_info_array));
}


RENDERER_API void vulkan_subpass_create_info_builder_set_render_set_bindings(vulkan_subpass_create_info_builder_t* builder, vulkan_shader_resource_description_t* bindings, u32 binding_count)
{
	AUTO create_info = get_bound_create_info(builder);
	create_info->sub_render_set_bindings = bindings;
	create_info->sub_render_set_binding_count = binding_count;
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_render_set_bindings_internal = false;
	build_info->is_use_render_set_bindings_builder = false;
}

RENDERER_API void vulkan_subpass_create_info_builder_set_render_set_bindings_builder(vulkan_subpass_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->render_set_bindings_builder = srd_builder;
	build_info->is_render_set_bindings_internal = false;
	build_info->is_use_render_set_bindings_builder = true;
}


RENDERER_API void vulkan_subpass_create_info_builder_set_bind_point(vulkan_subpass_create_info_builder_t* builder, VkPipelineBindPoint bind_point)
{
	AUTO create_info = get_bound_create_info(builder);
	create_info->pipeline_bind_point = bind_point;
}

RENDERER_API void vulkan_subpass_create_info_builder_add_color_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count)
{
	AUTO build_info = get_bound_build_info(builder);
	buf_pushv(&build_info->color_attachments, attachments, attachment_count);
}

RENDERER_API void vulkan_subpass_create_info_builder_add_input_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count)
{
	AUTO build_info = get_bound_build_info(builder);
	buf_pushv(&build_info->input_attachments, attachments, attachment_count);
}

RENDERER_API void vulkan_subpass_create_info_builder_add_preserve_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count)
{
	AUTO build_info = get_bound_build_info(builder);
	buf_pushv(&build_info->preserve_attachments, attachments, attachment_count);
}

RENDERER_API void vulkan_subpass_create_info_builder_set_depth_stencil_attachment(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference attachment)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->depth_stencil_attachment = attachment;
	build_info->is_depth_stencil_attachment_set = true;
}
