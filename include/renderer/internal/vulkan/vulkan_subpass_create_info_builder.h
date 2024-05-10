#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h> // vulkan_subpass_create_info_t

#include <bufferlib/buffer.h> // buffer_t

typedef struct vulkan_shader_resource_description_builder_t vulkan_shader_resource_description_builder_t;

typedef struct vulkan_subpass_create_info_build_info_t
{
	/* list of VkAttachmentReference objects for color attachments */
	buffer_t color_attachments;
	/* list of VkAttachmentReference objects for input attachments */
	buffer_t input_attachments;
	/* list of VkAttachmentReference objects for preserve attachments */
	buffer_t preserve_attachments;
	/* depth stencil attachment */
	VkAttachmentReference depth_stencil_attachment;
	/* non-null if set by vulkan_subpass_create_info_builder_set_render_set_bindings_builder() */
	vulkan_shader_resource_description_builder_t* render_set_bindings_builder;
	/* sub_render_set_bindings_internal */
	bool is_render_set_bindings_internal;
	bool is_color_attachments_internal;
	bool is_input_attachments_internal;
	bool is_preserve_attachments_internal;
	bool is_depth_stencil_attachment_set;
	bool is_use_render_set_bindings_builder;
} vulkan_subpass_create_info_build_info_t;

typedef struct memory_allocator_t memory_allocator_t;

typedef struct vulkan_subpass_create_info_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_subpass_create_info_t objects */
	buffer_t create_info_array;
	/* array of vulkan_subpass_create_build_info_t objects */
	buffer_t build_info_array;
	/* currently bound element in the above array, can be set by vulkan_subpass_create_info_builder_bind() */
	u32 bind_index;
} vulkan_subpass_create_info_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_subpass_create_info_builder_t* vulkan_subpass_create_info_builder_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_subpass_create_info_destroy(vulkan_subpass_create_info_builder_t* builder);

RENDERER_API void vulkan_subpass_create_info_builder_add(vulkan_subpass_create_info_builder_t* builder, u32 count);
RENDERER_API void vulkan_subpass_create_info_builder_bind(vulkan_subpass_create_info_builder_t* builder, u32 index);

RENDERER_API vulkan_subpass_create_info_t* vulkan_subpass_create_info_builder_get(vulkan_subpass_create_info_builder_t* builder);
RENDERER_API u32 vulkan_subpass_create_info_builder_get_count(vulkan_subpass_create_info_builder_t* builder);


RENDERER_API void vulkan_subpass_create_info_builder_set_render_set_bindings(vulkan_subpass_create_info_builder_t* builder, vulkan_shader_resource_description_t* bindings, u32 binding_count);
RENDERER_API void vulkan_subpass_create_info_builder_set_render_set_bindings_builder(vulkan_subpass_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder);
RENDERER_API void vulkan_subpass_create_info_builder_set_bind_point(vulkan_subpass_create_info_builder_t* builder, VkPipelineBindPoint bind_point);
RENDERER_API void vulkan_subpass_create_info_builder_add_color_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count);
RENDERER_API void vulkan_subpass_create_info_builder_add_input_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count);
RENDERER_API void vulkan_subpass_create_info_builder_add_preserve_attachments(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference* attachments, u32 attachment_count);
RENDERER_API void vulkan_subpass_create_info_builder_set_depth_stencil_attachment(vulkan_subpass_create_info_builder_t* builder, VkAttachmentReference attachment);

END_CPP_COMPATIBLE