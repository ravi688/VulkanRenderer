
#pragma once

#include <renderer/defines.h>
#include <renderer/glsl_types.h>
#include <renderer/internal/vulkan/vulkan_types.h> 	// vulkan_render_pass_type_t

typedef struct vulkan_shader_resource_description_t vulkan_shader_resource_description_t;
typedef enum vulkan_attachment_type_t vulkan_attachment_type_t;

/* NOTE: sub_render_set_binding_count is always equal to input_attachment_count */
typedef struct vulkan_subpass_description_t
{
	/* SUB RENDER SET BINDING DESCRIPTIONS */
	vulkan_shader_resource_description_t* sub_render_set_bindings;
	u32 sub_render_set_binding_count; 	// must be equal to input_attachment_count

	/* graphics pipeline description */
	u32 pipeline_description_index;

	/* ATTACHMENT POINTERS */
	
	/* list of color attachments */
	u32* color_attachments;
	u32 color_attachment_count;

	/* list of input attachments */
	u32* input_attachments;
	u32 input_attachment_count; 	// must be equal to sub_render_set_binding_count

	/* index of the depth stencil attachment */
	u32 depth_stencil_attachment;

	/* list of preserve attachments */
	u32* preserve_attachments;
	u32 preserve_attachment_count;

} vulkan_subpass_description_t;

/* NOTE: render_set_binding_count is always equal to input_attachment_count */
typedef struct vulkan_render_pass_description_t
{
	/* RENDER SET BINDING DESCRIPTIONS */
	vulkan_shader_resource_description_t* render_set_bindings;
	u32 render_set_binding_count; 	// must be equal to input_attachment_count

	/* SUBPASS DESCRIPTIONS */
	/* ptr to the list of subpass descriptions */
	vulkan_subpass_description_t* subpass_descriptions;
	/* number of subpasses in this render pass */
	union
	{	
		u32 subpass_description_count;
		u32 subpass_count;
	};

	/* ATTACHMENTS */
	vulkan_attachment_type_t* attachments;
	u32 attachment_count;

	/* INPUTS */

	/* list of input attachments from the previous render pass */
	u32* input_attachments;
	u32 input_attachment_count; 	// must be equal to render_set_binding_count

	/* type of this render pass */
	vulkan_render_pass_type_t type;

} vulkan_render_pass_description_t;


RENDERER_API void vulkan_render_pass_description_begin(vulkan_render_pass_description_t* description, vulkan_render_pass_type_t type);
RENDERER_API void vulkan_render_pass_description_add_input(vulkan_render_pass_description_t* description, glsl_type_t type, u32 index, u32 binding);
RENDERER_API void vulkan_render_pass_description_add_attachment(vulkan_render_pass_description_t* description, vulkan_attachment_type_t type);
RENDERER_API void vulkan_render_pass_description_begin_subpass(vulkan_render_pass_description_t* description, u32 pipeline_index);

typedef enum vulkan_attachment_reference_type_t
{
	VULKAN_ATTACHMENT_REFERENCE_TYPE_UNDEFINED = 0,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR = 1,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL,
	VUKLAN_ATTACHMENT_REFERENCE_TYPE_PRESERVE
} vulkan_attachment_reference_type_t;

RENDERER_API void vulkan_render_pass_description_add_attachment_reference(vulkan_render_pass_description_t* description, vulkan_attachment_reference_type_t type, u32 reference, u32 binding);
RENDERER_API void vulkan_render_pass_description_end_subpass(vulkan_render_pass_description_t* description);
RENDERER_API void vulkan_render_pass_description_end(vulkan_render_pass_description_t* description);
