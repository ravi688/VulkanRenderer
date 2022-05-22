
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>

typedef struct vulkan_subpass_create_info_t
{
	/* bind point of this subpass in the pipeline */
	VkPipelineBindPoint pipeline_bind_point;

	/* list of color attachments */
	VkAttachmentReference* color_attachments;
	u32 color_attachment_count;

	/* list of input attachments */
	VkAttachmentReference* input_attachments;
	u32 input_attachment_count;

	/* depth stencil attachment */
	VkAttachmentReference* depth_stencil_attachment;

	/* list of attachments to be preserved */
	VkAttachmentReference* preserve_attachments;
	u32 preserve_attachment_count;

	/* used for creating descriptor set layout for the sub render set */
	vulkan_shader_resource_descriptor_t* sub_render_set_bindings;
	u32 sub_render_set_binding_count;

} vulkan_subpass_create_info_t;

typedef struct vulkan_attachment_usage_t vulkan_attachment_usage_t;

typedef struct vulkan_render_pass_create_info_t
{
	/* 	number of framebuffers
		usually this should be 1
		But if this render pass is writing to the image views which has to be presented later on
		then this has to be equal to the number of swap chain images because of asynchronous presentation engine.
	 */
	u32 framebuffer_count;

	/* list of attachment descriptions */
	VkAttachmentDescription* attachment_descriptions;
	/* list of attachment usages */
	vulkan_attachment_usage_t* attachment_usages;
	u32 attachment_description_count;

	/* list of extra attachments in the framebuffer such as swapchain image view */
	VkImageView* supplementary_attachments;
	VkAttachmentDescription* supplementary_attachment_descriptions;
	/* this could be left NULL */
	vulkan_attachment_usage_t* supplementary_attachment_usages;
	u32 supplementary_attachment_count;

	/* list of subpass create infos */
	vulkan_subpass_create_info_t* subpasses;
	u32 subpass_count;

	/* used for creating descriptor set layout for the render set */
	vulkan_shader_resource_descriptor_t* render_set_bindings;
	u32 render_set_binding_count;

} vulkan_render_pass_create_info_t;

typedef buf_ucount_t vulkan_render_pass_handle_t;
#define VULKAN_RENDER_PASS_HANDLE_INVALID (~0ULL)
typedef struct vulkan_attachment_t vulkan_attachment_t;

typedef struct vulkan_render_pass_t
{
	vulkan_renderer_t* renderer;

	/* handle to this render pass in the render pass pool */
	vulkan_render_pass_handle_t handle;	

	/* vulkan object handle */
	VkRenderPass vo_handle;

	/* attachments for one frame buffer */
	vulkan_attachment_t* attachments;
	u32 attachment_count;

	/* supplementary attachments for a frame buffer 
		NOTE: this is not a deep copy or internal allocation, 
		instead it points to the create info's supplementary attachments.
		This way updating the supplementary attachments results in updates to internal framebuffers for this render pass
		which is useful when the render window resizes.
	 */
	VkImageView* supplementary_attachments;
	u32 supplementary_attachment_count;

	/* clear value for each attachment in the frame buffer */
	VkClearValue* vo_clear_values;
	/* this is equal to attachment_count + supplementary_attachment_count */
	u32 clear_value_count;

	/* framebuffers for this render pass */
	VkFramebuffer* vo_framebuffers;
	u32 framebuffer_count;

	/* sub render set layouts for this render pass */
	vulkan_descriptor_set_layout_t* sub_render_set_layouts;

	/* list of sub render sets, i.e. for each sub pass */
	vulkan_descriptor_set_t* sub_render_sets;

	/* number of subpasses in this render pass */	
	u32 subpass_count;

	/* index of the current subpass */
	u32 current_subpass_index;

	/* render set layout for this render pass */
	vulkan_descriptor_set_layout_t render_set_layout;

	/* render set for this render pass */
	vulkan_descriptor_set_t render_set;

} vulkan_render_pass_t;

BEGIN_CPP_COMPATIBLE

/*
	description:
		allocates memory on heap and intiializes it to zero
	params:
		nothing
	returns:
		pointer to newly created vulkan_render_pass_t object on heap
 */
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new();

/*
	description:
		creates a vulkan render pass, it calls vulkan_render_pass_new() internally to allocate memory
	params:
		renderer: pointer to a valid vulkan_renderer_t object
		create_info: pointer to vulkan_render_pass_create_info_t object
	returns:
		pointer to newly created vulkan_render_pass_t object on heap
 */
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info);

/*
	description:
		creates a vulkan render pass, it calls vulkan_render_pass_new() internally to allocate memory
	params:
		renderer: pointer to a valid vulkan_renderer_t object
		create_info: pointer to vulkan_render_pass_create_info_t object
		OUT render_pass: pointer to the pre allocated vulkan_render_pass_t object (by vulkan_render_pass_new())
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_t OUT render_pass);

/*
	description:
		destroys the vulkan objects
	params:
		render_pass: pointer to a valid vulkan_render_pass_t object, must have been created by vulkan_render_pass_create
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_destroy(vulkan_render_pass_t* render_pass);

/*
	description:
		releases heap allocated memory
	params:
		render_pass: pointer to a valid vulkan_render_pass_t object, must have been created by vulkan_render_pass_create or  allocated by vulkan_render_pass_new
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_t* render_pass);

/*
	description:
		begins the render pass
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
		begin_info: ptr to the vulkan_render_pass_begin_info_t object which contains the clear values and reference to the framebuffer
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, u32 framebuffer_index);

/*
	description:
		ends the render pass
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_end(vulkan_render_pass_t* render_pass);

/*
	description:
		switches to the next subpass if exists
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_next(vulkan_render_pass_t* render_pass);

END_CPP_COMPATIBLE
