
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/alloc.h>
#include <bufferlib/buffer.h>


static void* create_element(BUFFER* list)
{
	buf_push_pseudo(list, 1);
	return buf_peek_ptr(list);	
}

#define create_buffer(type) __create_buffer(sizeof(type))
static BUFFER* __create_buffer(u32 size)
{
	BUFFER* buffer = heap_new(BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

RENDERER_API void vulkan_render_pass_description_begin(vulkan_render_pass_description_t* description, vulkan_render_pass_type_t type)
{
	description->type = type;
	description->attachments = CAST_TO(vulkan_attachment_type_t*, create_buffer(vulkan_attachment_type_t));
	description->input_attachments = CAST_TO(u32*, create_buffer(u32));
	description->subpass_descriptions = CAST_TO(vulkan_subpass_description_t*, create_buffer(vulkan_subpass_description_t));
	description->subpass_dependencies = CAST_TO(VkSubpassDependency*, create_buffer(VkSubpassDependency));
	description->render_set_bindings = CAST_TO(vulkan_shader_resource_description_t*, create_buffer(vulkan_shader_resource_description_t));
}

RENDERER_API void vulkan_render_pass_description_add_input(vulkan_render_pass_description_t* description, glsl_type_t type, u32 index, u32 binding)
{
	buf_push_auto(CAST_TO(BUFFER*, description->input_attachments), index);
	vulkan_shader_resource_description_add_opaque(create_element(CAST_TO(BUFFER*, description->render_set_bindings)), "internal", type, VULKAN_DESCRIPTOR_SET_RENDER, binding);
}

RENDERER_API void vulkan_render_pass_description_add_attachment(vulkan_render_pass_description_t* description, vulkan_attachment_type_t type)
{
	buf_push_auto(CAST_TO(BUFFER*, description->attachments), type);
}

RENDERER_API void vulkan_render_pass_description_begin_subpass(vulkan_render_pass_description_t* description, u32 pipeline_index)
{
	vulkan_subpass_description_t* subpass = create_element(CAST_TO(BUFFER*, description->subpass_descriptions));
	subpass->pipeline_description_index = pipeline_index;
	subpass->depth_stencil_attachment = U32_MAX;
	subpass->color_attachments = CAST_TO(u32*, create_buffer(u32));
	subpass->input_attachments = CAST_TO(u32*, create_buffer(u32));
	subpass->preserve_attachments = CAST_TO(u32*, create_buffer(u32));

	subpass->sub_render_set_bindings = CAST_TO(vulkan_shader_resource_description_t*, create_buffer(vulkan_shader_resource_description_t));
}

RENDERER_API void vulkan_render_pass_description_add_attachment_reference(vulkan_render_pass_description_t* description, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, description->subpass_descriptions));
	switch(type)
	{
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR:
			buf_push_auto(CAST_TO(BUFFER*, subpass->color_attachments), reference);
			break;
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT:
			buf_push_auto(CAST_TO(BUFFER*, subpass->input_attachments), reference);
			vulkan_shader_resource_description_add_opaque(create_element(CAST_TO(BUFFER*, subpass->sub_render_set_bindings)), "internal", GLSL_TYPE_SUBPASS_INPUT, VULKAN_DESCRIPTOR_SET_SUB_RENDER, binding);
			break;
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL:
			subpass->depth_stencil_attachment = reference;
			break;
		case VUKLAN_ATTACHMENT_REFERENCE_TYPE_PRESERVE:
			buf_push_auto(CAST_TO(BUFFER*, subpass->preserve_attachments), reference);		
			break;
		default:
			LOG_FETAL_ERR("Unsupported vulkan attachment reference type: %u\n", type);
	}
}

RENDERER_API void vulkan_render_pass_description_end_subpass(vulkan_render_pass_description_t* description)
{
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, description->subpass_descriptions));
	BUFFER* buffer;
	
	buffer = CAST_TO(BUFFER*, subpass->color_attachments);
	subpass->color_attachment_count = buf_get_element_count(buffer);
	subpass->color_attachments = (subpass->color_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
	
	buffer = CAST_TO(BUFFER*, subpass->input_attachments);
	subpass->input_attachment_count = buf_get_element_count(buffer);
	subpass->input_attachments = (subpass->input_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);

	buffer = CAST_TO(BUFFER*, subpass->preserve_attachments);
	subpass->preserve_attachment_count = buf_get_element_count(buffer);
	subpass->preserve_attachments = (subpass->preserve_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);

	buffer = CAST_TO(BUFFER*, subpass->sub_render_set_bindings);
	subpass->sub_render_set_binding_count = buf_get_element_count(buffer);
	subpass->sub_render_set_bindings = (subpass->sub_render_set_binding_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
}

RENDERER_API void vulkan_render_pass_description_add_subpass_dependency(vulkan_render_pass_description_t* description, VkSubpassDependency* dependency)
{
	VkSubpassDependency* dst_dependency = create_element(CAST_TO(BUFFER*, description->subpass_dependencies));
	memcpy(dst_dependency, dependency, sizeof(VkSubpassDependency));
}

RENDERER_API void vulkan_render_pass_description_end(vulkan_render_pass_description_t* description)
{
	BUFFER* buffer = CAST_TO(BUFFER*, description->attachments);
	description->attachment_count = buf_get_element_count(buffer);
	description->attachments = (description->attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->input_attachments);
	description->input_attachment_count = buf_get_element_count(buffer);
	description->input_attachments = (description->input_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->subpass_descriptions);
	description->subpass_count = buf_get_element_count(buffer);
	description->subpass_descriptions = (description->subpass_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->subpass_dependencies);
	description->subpass_dependency_count = buf_get_element_count(buffer);
	description->subpass_dependencies = (description->subpass_dependency_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, description->render_set_bindings);
	description->render_set_binding_count = buf_get_element_count(buffer);
	description->render_set_bindings = (description->render_set_binding_count == 0) ? NULL : buf_get_ptr(buffer);
	heap_free(buffer);
}
