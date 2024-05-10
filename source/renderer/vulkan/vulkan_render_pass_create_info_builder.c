#include <renderer/internal/vulkan/vulkan_render_pass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_subpass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/internal/vulkan/vulkan_attachment.h> // vulkan_attachment_next_pass_usage_t
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_render_pass_create_info_builder_t* vulkan_render_pass_create_info_builder_create(memory_allocator_t* allocator)
{
	vulkan_render_pass_create_info_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_CREATE_INFO_BUILDER, vulkan_render_pass_create_info_builder_t);
	memzero(builder, vulkan_render_pass_create_info_builder_t);
	builder->allocator = allocator;
	builder->create_info_array = memory_allocator_buf_new(allocator, vulkan_render_pass_create_info_t);
	builder->build_info_array = memory_allocator_buf_new(allocator, vulkan_render_pass_create_info_build_info_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API void vulkan_render_pass_create_info_builder_destroy(vulkan_render_pass_create_info_builder_t* builder)
{
	buf_free(&builder->create_info_array);
	u32 build_info_count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < build_info_count; i++)
	{
		AUTO build_info = buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_render_pass_create_info_build_info_t, i);
		_debug_assert__(build_info->is_attachment_descriptions_internal);
		_debug_assert__(build_info->is_attachment_usages_internal);
		buf_free(&build_info->attachment_descriptions);
		buf_free(&build_info->attachment_usages);
		if(build_info->is_supplementary_attachments_internal)
			memory_allocator_dealloc(builder->allocator, build_info->vo_supplementary_attachments);
	}
	buf_free(&builder->build_info_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_render_pass_create_info_builder_add(vulkan_render_pass_create_info_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->create_info_array, count);
	buf_push_pseudo(&builder->build_info_array, count);
	AUTO build_info = CAST_TO(vulkan_render_pass_create_info_build_info_t*, buf_peek_ptr(&builder->build_info_array));
	build_info->attachment_descriptions = memory_allocator_buf_new(builder->allocator, VkAttachmentDescription);
	build_info->is_attachment_descriptions_internal = true;
	build_info->attachment_usages = memory_allocator_buf_new(builder->allocator, vulkan_attachment_next_pass_usage_t);
	build_info->is_attachment_usages_internal = true;
}
RENDERER_API void vulkan_render_pass_create_info_builder_bind(vulkan_render_pass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	builder->bind_index = index;
}

static vulkan_render_pass_create_info_build_info_t* get_build_info(vulkan_render_pass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_render_pass_create_info_build_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_render_pass_create_info_build_info_t* get_bound_build_info(vulkan_render_pass_create_info_builder_t* builder)
{
	return get_build_info(builder, builder->bind_index);
}

static vulkan_render_pass_create_info_t* get_create_info(vulkan_render_pass_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->create_info_array, vulkan_render_pass_create_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_render_pass_create_info_t* get_bound_create_info(vulkan_render_pass_create_info_builder_t* builder)
{
	return get_create_info(builder, builder->bind_index);
}

RENDERER_API vulkan_render_pass_create_info_t* vulkan_render_pass_create_info_builder_get(vulkan_render_pass_create_info_builder_t* builder)
{
	if(buf_get_element_count(&builder->build_info_array) == 0)
		return NULL;
	_debug_assert__(buf_get_element_count(&builder->build_info_array) == buf_get_element_count(&builder->create_info_array));
	u32 count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO build_info = get_build_info(builder, i);
		AUTO create_info = get_create_info(builder, i);
		create_info->framebuffer_layout_description.vo_supplementary_attachments = build_info->vo_supplementary_attachments;
		/* attachment descriptions */
		create_info->framebuffer_layout_description.attachment_description_count = buf_get_element_count(&build_info->attachment_descriptions);
		if(create_info->framebuffer_layout_description.attachment_description_count > 0)
			create_info->framebuffer_layout_description.attachment_descriptions = CAST_TO(VkAttachmentDescription*, buf_get_ptr(&build_info->attachment_descriptions));
		else
			create_info->framebuffer_layout_description.attachment_descriptions = NULL;
		/* attachment usages */
		_debug_assert__(buf_get_element_count(&build_info->attachment_usages) == create_info->framebuffer_layout_description.attachment_description_count);
		if(create_info->framebuffer_layout_description.attachment_description_count > 0)
			create_info->framebuffer_layout_description.attachment_usages = CAST_TO(vulkan_attachment_next_pass_usage_t*, buf_get_ptr(&build_info->attachment_usages));
		else
			create_info->framebuffer_layout_description.attachment_usages = NULL;
		/* subpasses */
		if(build_info->is_use_subpasses_builder)
		{
			create_info->subpass_count = vulkan_subpass_create_info_builder_get_count(build_info->subpasses_builder);
			if(create_info->subpass_count > 0)
				create_info->subpasses = vulkan_subpass_create_info_builder_get(build_info->subpasses_builder);
			else
				create_info->subpasses = NULL;
		}
		/* render set bindings */
		if(build_info->is_use_render_set_bindings_builder)
		{
			create_info->render_set_binding_count = vulkan_shader_resource_description_builder_get_count(build_info->render_set_bindings_builder);
			if(create_info->render_set_binding_count > 0)
				create_info->render_set_bindings = vulkan_shader_resource_description_builder_get(build_info->render_set_bindings_builder);
			else
				create_info->render_set_bindings = NULL;
		}
	}
	return CAST_TO(vulkan_render_pass_create_info_t*, buf_get_ptr(&builder->create_info_array));
}

RENDERER_API u32 vulkan_render_pass_create_info_builder_get_count(vulkan_render_pass_create_info_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->build_info_array));
}


RENDERER_API void vulkan_render_pass_create_info_builder_set_framebuffer_count(vulkan_render_pass_create_info_builder_t* builder, u32 framebuffer_count)
{
	AUTO create_info = get_bound_create_info(builder);
	create_info->framebuffer_count = framebuffer_count;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(vulkan_render_pass_create_info_builder_t* builder, u32 bucket_count, u32 bucket_depth)
{
	AUTO build_info = get_bound_build_info(builder);
	AUTO create_info = get_bound_create_info(builder);
	create_info->framebuffer_layout_description.supplementary_attachment_bucket_count = bucket_count;
	create_info->framebuffer_layout_description.supplementary_attachment_bucket_depth = bucket_depth;
	create_info->framebuffer_layout_description.supplementary_attachment_count = bucket_count * bucket_depth;
	if(build_info->is_supplementary_attachments_internal && (build_info->vo_supplementary_attachments != NULL))
	{
		memory_allocator_dealloc(builder->allocator, build_info->vo_supplementary_attachments);
		build_info->vo_supplementary_attachments = NULL;
		build_info->is_supplementary_attachments_internal = false;
	}
	if(create_info->framebuffer_layout_description.supplementary_attachment_count > 0)
		build_info->vo_supplementary_attachments = memory_allocator_alloc_obj_array(builder->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, create_info->framebuffer_layout_description.supplementary_attachment_count);
	else
		build_info->vo_supplementary_attachments = NULL;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachments(vulkan_render_pass_create_info_builder_t* builder, u32 start_index, VkImageView* views, u32 view_count)
{
	AUTO build_info = get_bound_build_info(builder);
	_debug_assert__(build_info->vo_supplementary_attachments != NULL);
	memcopyv(build_info->vo_supplementary_attachments + start_index, views, VkImageView, view_count);
}

RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_descriptions(vulkan_render_pass_create_info_builder_t* builder, VkAttachmentDescription* descriptions, u32 description_count)
{
	AUTO build_info = get_bound_build_info(builder);
	buf_pushv(&build_info->attachment_descriptions, descriptions, description_count);
}

RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_usages(vulkan_render_pass_create_info_builder_t* builder, vulkan_attachment_next_pass_usage_t* usages, u32 usage_count)
{
	AUTO build_info = get_bound_build_info(builder);
	buf_pushv(&build_info->attachment_usages, usages, usage_count);
}


RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_t* bindings, u32 binding_count)
{
	AUTO create_info = get_bound_create_info(builder);
	create_info->render_set_bindings = bindings;
	create_info->render_set_binding_count = binding_count;
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_render_set_bindings_builder = false;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->render_set_bindings_builder = srd_builder;
	build_info->is_use_render_set_bindings_builder = true;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_t* infos, u32 info_count)
{
	AUTO create_info = get_bound_create_info(builder);
	create_info->subpasses = infos;
	create_info->subpass_count = info_count;
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_subpasses_builder = false;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_builder_t* sci_builder)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->subpasses_builder = sci_builder;
	build_info->is_use_subpasses_builder = true;
}

RENDERER_API void vulkan_render_pass_create_info_builder_set_dependencies(vulkan_render_pass_create_info_builder_t* builder, VkSubpassDependency* dependencies, u32 dependency_count)
{
	AUTO create_info = get_bound_create_info(builder);
	AUTO build_info = get_bound_build_info(builder);
	create_info->subpass_dependencies = dependencies;
	create_info->subpass_dependency_count = dependency_count;
	build_info->is_supbass_dependencies_internal = false;
}
