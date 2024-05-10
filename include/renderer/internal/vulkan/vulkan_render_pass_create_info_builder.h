#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h> // vulkan_render_pass_create_info_t

#include <bufferlib/buffer.h> // buffer_t

typedef struct vulkan_subpass_create_info_t vulkan_subpass_create_info_t;
typedef struct vulkan_subpass_create_info_builder_t vulkan_subpass_create_info_builder_t;
typedef struct vulkan_shader_resource_description_builder_t vulkan_shader_resource_description_builder_t;

typedef struct vulkan_render_pass_create_info_build_info_t
{
	/* list of VkAttachmentDescription objects */
	buffer_t attachment_descriptions;
	/* list of vulkan_attachment_next_pass_usage_t objects */
	buffer_t attachment_usages;
	/* non-null if set by vulkan_render_pass_create_info_builder_add_render_set_bindings_builder() */
	vulkan_shader_resource_description_builder_t* render_set_bindings_builder;
	bool is_destroy_render_set_bindings_builder;
	/* non-null if allocated by vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket() */
	VkImageView* vo_supplementary_attachments;
	/* non-null if set by vulkan_render_pass_create_info_builder_set_subpasses_builder*/
	vulkan_subpass_create_info_builder_t* subpasses_builder;
	bool is_destroy_subpasses_builder;

	bool is_use_render_set_bindings_builder;
	bool is_use_subpasses_builder;
	bool is_supplementary_attachments_internal;
	bool is_attachment_descriptions_internal;
	bool is_attachment_usages_internal;
	bool is_supbass_dependencies_internal;
} vulkan_render_pass_create_info_build_info_t;

typedef struct vulkan_render_pass_create_info_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_render_pass_create_info_t objects */
	buffer_t create_info_array;
	/* array of vulkan_render_pass_create_build_info_t objects */
	buffer_t build_info_array;
	/* currently bound element in the above array, can be set by vulkan_render_pass_create_info_builder_bind() */
	u32 bind_index;
} vulkan_render_pass_create_info_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_render_pass_create_info_builder_t* vulkan_render_pass_create_info_builder_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_render_pass_create_info_builder_destroy(vulkan_render_pass_create_info_builder_t* builder);

RENDERER_API void vulkan_render_pass_create_info_builder_add(vulkan_render_pass_create_info_builder_t* builder, u32 count);
RENDERER_API void vulkan_render_pass_create_info_builder_bind(vulkan_render_pass_create_info_builder_t* builder, u32 index);

RENDERER_API vulkan_render_pass_create_info_t* vulkan_render_pass_create_info_builder_get(vulkan_render_pass_create_info_builder_t* builder);
RENDERER_API u32 vulkan_render_pass_create_info_builder_get_count(vulkan_render_pass_create_info_builder_t* builder);


RENDERER_API void vulkan_render_pass_create_info_builder_set_framebuffer_count(vulkan_render_pass_create_info_builder_t* builder, u32 framebuffer_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(vulkan_render_pass_create_info_builder_t* builder, u32 bucket_count, u32 bucket_depth);
RENDERER_API void vulkan_render_pass_create_info_builder_set_supplementary_attachments(vulkan_render_pass_create_info_builder_t* builder, u32 start_index, VkImageView* views, u32 view_count);
RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_descriptions(vulkan_render_pass_create_info_builder_t* builder, VkAttachmentDescription* descriptions, u32 description_count);
RENDERER_API void vulkan_render_pass_create_info_builder_add_attachment_usages(vulkan_render_pass_create_info_builder_t* builder, vulkan_attachment_next_pass_usage_t* usages, u32 usage_count);

RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_t* bindings, u32 binding_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_render_set_bindings_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder, bool is_destroy);
RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_t* infos, u32 info_count);
RENDERER_API void vulkan_render_pass_create_info_builder_set_subpasses_builder(vulkan_render_pass_create_info_builder_t* builder, vulkan_subpass_create_info_builder_t* sci_builder, bool is_destroy);
RENDERER_API void vulkan_render_pass_create_info_builder_set_dependencies(vulkan_render_pass_create_info_builder_t* builder, VkSubpassDependency* dependencies, u32 dependency_count);



END_CPP_COMPATIBLE