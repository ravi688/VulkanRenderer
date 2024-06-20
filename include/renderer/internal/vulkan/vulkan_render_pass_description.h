/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_description.h is a part of VulkanRenderer

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

#include <renderer/defines.h>
#include <glslcommon/glsl_types.h>
#include <renderer/internal/vulkan/vulkan_types.h> 	// vulkan_render_pass_type_t
#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_attachment.h> // vulkan_attachment_type_t

typedef struct vulkan_shader_resource_description_t vulkan_shader_resource_description_t;
typedef struct vulkan_vertex_buffer_layout_description_t vulkan_vertex_buffer_layout_description_t;

/* NOTE: sub_render_set_binding_count is always equal to input_attachment_count */
typedef struct vulkan_subpass_description_t
{
	/* VERTEX ATTRIBUTE DESCRIPTIONS */
	vulkan_vertex_buffer_layout_description_t* vertex_infos;
	u32 vertex_info_count;

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

	/* SUBPASS DEPENDENCIES */
	VkSubpassDependency* subpass_dependencies;
	u32 subpass_dependency_count;

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


RENDERER_API void vulkan_render_pass_description_begin(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, vulkan_render_pass_type_t type);
RENDERER_API void vulkan_render_pass_description_add_input(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, glsl_type_t type, u32 index, u32 binding);
RENDERER_API void vulkan_render_pass_description_add_attachment(vulkan_render_pass_description_t* description, vulkan_attachment_type_t type);
RENDERER_API void vulkan_render_pass_description_begin_subpass(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, u32 pipeline_index);

typedef enum vulkan_attachment_reference_type_t
{
	VULKAN_ATTACHMENT_REFERENCE_TYPE_UNDEFINED = 0,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR = 1,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL,
	VUKLAN_ATTACHMENT_REFERENCE_TYPE_PRESERVE
} vulkan_attachment_reference_type_t;

RENDERER_API void vulkan_render_pass_description_add_attachment_reference(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, vulkan_attachment_reference_type_t type, u32 reference, u32 binding);
RENDERER_API void vulkan_render_pass_description_end_subpass(memory_allocator_t* allocator, vulkan_render_pass_description_t* description);
RENDERER_API void vulkan_render_pass_description_add_subpass_dependency(vulkan_render_pass_description_t* description, VkSubpassDependency* dependency);
RENDERER_API void vulkan_render_pass_description_end(memory_allocator_t* allocator, vulkan_render_pass_description_t* description);

RENDERER_API void vulkan_render_pass_description_destroy_allocations(memory_allocator_t* allocator, vulkan_render_pass_description_t* description);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void begin_pass(memory_allocator_t* allocator, BUFFER* list, vulkan_render_pass_type_t type)
{
	vulkan_render_pass_description_begin(allocator, buf_create_element(list), type);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void add_input_sampler2d(memory_allocator_t* allocator, BUFFER* list, u32 index, u32 binding)
{
	vulkan_render_pass_description_add_input(allocator, buf_peek_ptr(list), GLSL_TYPE_SAMPLER_2D, index, binding);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void add_attachment(BUFFER* list, vulkan_attachment_type_t type)
{
	vulkan_render_pass_description_add_attachment(buf_peek_ptr(list), type);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void add_dependency(BUFFER* list, VkSubpassDependency* dependency)
{
	vulkan_render_pass_description_add_subpass_dependency(buf_peek_ptr(list), dependency);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void begin_subpass(memory_allocator_t* allocator, BUFFER* list, u32 pipeline_index)
{
	vulkan_render_pass_description_begin_subpass(allocator, buf_peek_ptr(list), pipeline_index);
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void add_attachment_reference(memory_allocator_t* allocator, BUFFER* list, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	vulkan_render_pass_description_add_attachment_reference(allocator, buf_peek_ptr(list), type, reference, binding);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void end_subpass(memory_allocator_t* allocator, BUFFER* list)
{
	vulkan_render_pass_description_end_subpass(allocator, buf_peek_ptr(list));
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void end_pass(memory_allocator_t* allocator, BUFFER* list)
{
	vulkan_render_pass_description_end(allocator, buf_peek_ptr(list));
}
