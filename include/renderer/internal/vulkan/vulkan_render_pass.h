/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/color.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_handles.h> 		// vulkan_render_pass_handle_t
#include <renderer/internal/vulkan/vulkan_object.h>
#include <renderer/event.h>

#define VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN (~0U)

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
	u32* preserve_attachments;
	u32 preserve_attachment_count;

	/* used for creating descriptor set layout for the sub render set */
	vulkan_shader_resource_description_t* sub_render_set_bindings;
	u32 sub_render_set_binding_count;

} vulkan_subpass_create_info_t;

typedef enum vulkan_attachment_next_pass_usage_t vulkan_attachment_next_pass_usage_t;

/* structure to hold all the information for creating vulkan_attachment_t objects and setting up framebuffer attachments */
typedef struct vulkan_framebuffer_attachments_layout_description_t
{
	/* 2d list, list of extra attachments in the foreach framebuffer, such as swapchain image view */
	VkImageView* vo_supplementary_attachments;
	/* must equal to supplementary_attachment_bucket_count * supplementary_attachment_bucket_depth */
	u32 supplementary_attachment_count;
	u32 supplementary_attachment_bucket_count;
	u32 supplementary_attachment_bucket_depth;
	/* list of attachment descriptions */
	union
	{
		VkAttachmentDescription* attachment_descriptions;
		VkAttachmentDescription* attachments;
	};
	/* list of attachment usages */
	vulkan_attachment_next_pass_usage_t* attachment_usages;
	union
	{
		u32 attachment_description_count;
		u32 attachment_count;
	};
} vulkan_framebuffer_attachments_layout_description_t;

typedef struct vulkan_render_pass_create_info_t
{
	/*
		usually it would be equal to the number of swapchain images if there is only one RenderPass
		but if there are multiple passes and successive passes consumes previous results then it woulbe equal to one
	*/
	u32 framebuffer_count;

	/* description of attachments and their usages for this render pass */
	vulkan_framebuffer_attachments_layout_description_t framebuffer_layout_description;

	/* list of subpass create infos */
	vulkan_subpass_create_info_t* subpasses;
	u32 subpass_count;

	/* list of subpass dependencies
	 */
	VkSubpassDependency* subpass_dependencies;
	u32 subpass_dependency_count;

	/* used for creating descriptor set layout for the render set */
	vulkan_shader_resource_description_t* render_set_bindings;
	u32 render_set_binding_count;

} vulkan_render_pass_create_info_t;

typedef struct vulkan_attachment_t vulkan_attachment_t;
typedef struct vulkan_camera_t vulkan_camera_t;

typedef struct vulkan_render_pass_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;

	vulkan_render_pass_handle_t handle;

	/* vulkan object handle */
	VkRenderPass vo_handle;

	union
	{
		/* attachment_count or clear_value_count = suplementary_attachment_count + allocated_attachment_count */
		u32 attachment_count;
		u32 clear_value_count;
	};

	/* clear value for each attachment in the frame buffer */
	VkClearValue* vo_clear_values;

	u32 required_framebuffer_count;

	/* sub render set layouts for this render pass */
	vulkan_descriptor_set_layout_t* sub_render_set_layouts;

	/* number of subpasses in this render pass */	
	u32 subpass_count;

	/* index of the current subpass */
	u32 current_subpass_index;

	/* render set layout for this render pass */
	vulkan_descriptor_set_layout_t render_set_layout;

	/* formats of each attachment [only for internal use] */
	VkFormat* vo_formats;

	/* current render area (set/updated whenever vulkan_render_pass_begin(...) is called) */
	VkRect2D vo_current_render_area;

} vulkan_render_pass_t;

#define VULKAN_RENDER_PASS(ptr) VULKAN_OBJECT_UP_CAST(vulkan_render_pass_t*, VULKAN_OBJECT_TYPE_VULKAN_RENDER_PASS, ptr)
#define VULKAN_RENDER_PASS_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_render_pass_t*, VULKAN_OBJECT_TYPE_VULKAN_RENDER_PASS, ptr)

BEGIN_CPP_COMPATIBLE

/*
	description:
		allocates memory on heap and intiializes it to zero
	params:
		nothing
	returns:
		pointer to newly created vulkan_render_pass_t object on heap
 */
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_new(memory_allocator_t* allocator);

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

RENDERER_API void vulkan_render_pass_set_clear_indirect(vulkan_render_pass_t* render_pass, color_t color, float depth, VkClearValue* indirect_buffer);
RENDERER_API void vulkan_render_pass_set_clear(vulkan_render_pass_t* render_pass, color_t color, float depth);

/*
	description:
		begins the render pass
	params:
		render_pass: ptr to a valid vulkan_render_pass_t object
		framebuffer_index: index of the framebuffer to be used as a render target, it could be VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN for swapchain images
		camera: output camera
	returns:
		nothing
 */
RENDERER_API void vulkan_render_pass_begin(vulkan_render_pass_t* render_pass, u32 framebuffer_index, vulkan_camera_t* camera);

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
