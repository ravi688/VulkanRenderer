
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;
typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;
typedef struct vulkan_descriptor_set_create_info_t vulkan_descriptor_set_create_info_t;

typedef struct vulkan_subpass_create_info_t
{
	// bind point of this subpass in the pipeline
	VkPipelineBindPoint pipeline_bind_point;


	// list of color attachments
	VkAttachmentReference* color_attachments;
	u32 color_attachment_count;

	// list of input attachments
	VkAttachmentReference* input_attachments;
	u32 input_attachment_count;

	// depth stencil attachment
	VkAttachmentReference* depth_stencil_attachment;

	// list of attachments to be preserved
	VkAttachmentReference* preserve_attachments;
	u32 preserve_attachment_count;

	// render set create info
	vulkan_descriptor_set_create_info_t* set_info;
} vulkan_subpass_create_info_t;

typedef struct vulkan_render_pass_create_info_t
{
	// list of attachment descriptions
	VkAttachmentDescription* attachment_descriptions;
	u32 attachment_description_count;

	// list of subpass create infos
	vulkan_subpass_create_info_t* subpasses;
	u32 subpass_count;
} vulkan_render_pass_create_info_t;

typedef struct vulkan_render_pass_begin_info_t
{
	VkFramebuffer framebuffer;	// framebuffer to render onto
	u32 width;					// width of the framebuffer in pixels
	u32 height;					// height of the framebuffer in pixels

	// clear values
	u32 clear_stencil;			// stencil buffer clear value
	float clear_depth;			// depth buffer clear value
	bool is_depth; 				// is depth attachment is attached
	color_t* clear_colors;		// clear colors for the color attachments
	u32 clear_color_count; 		// number of clear colors, it must be equal to the attachment_description_count
} vulkan_render_pass_begin_info_t;

typedef struct vulkan_render_pass_t
{
	vulkan_renderer_t* context;
	VkRenderPass handle;
	u32 subpass_count;
	u32 current_subpass_index;

	// list of render set, i.e set for each sub pass
	vulkan_descriptor_set_t* sets;
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
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_create(vulkan_renderer_t* context, vulkan_render_pass_create_info_t* create_info);

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
RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, vulkan_render_pass_begin_info_t* begin_info);

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

/*
	description:
		returns the sub pass count
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
	returns:
		u32, sub pass count
 */
RENDERER_API u32 vulkan_render_pass_get_subpass_count(vulkan_render_pass_t* render_pass);

/*
	description:
		binds the descriptor set (render set) for the sub pass at 'subpass_index' or current subpass
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
		subpass_index: sub pass index or VULKAN_SUBPASS_INDEX_CURRENT
	returns:
		nothing
 */
#define VULKAN_SUBPASS_INDEX_CURRENT (~0UL)
RENDERER_API void vulkan_render_pass_bind_set(vulkan_render_pass_t* render_pass, u32 subpass_index, vulkan_pipeline_layout_t* pipeline_layout);

/*
	description:
		returns ptr to the vulkan_descriptor_set_t object corresponding the 'subpass_index'
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
		subpass_index: sub pass index or VULKAN_SUBPASS_INDEX_CURRENT
	returns:
		nothing
 */
RENDERER_API vulkan_descriptor_set_t* vulkan_render_pass_get_set(vulkan_render_pass_t* render_pass, u32 subpass_index);

/*
	description:
		generates a unique hash value for vulkan_render_pass_create_info_t
	params:
		create_info: ptr to the vulkan_render_pass_create_info_t object
	returns:
		64 bit hash value
 */
RENDERER_API u64 vulkan_render_pass_create_info_get_hash(vulkan_render_pass_create_info_t* create_info);

END_CPP_COMPATIBLE
