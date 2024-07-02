/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_description.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <bufferlib/buffer.h>


static void* create_element(BUFFER* list)
{
	buf_push_pseudo(list, 1);
	return buf_peek_ptr(list);	
}

#define create_buffer(allocator, type) __create_buffer(allocator, sizeof(type))
static BUFFER* __create_buffer(memory_allocator_t* allocator, u32 size)
{
	BUFFER* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_BUFFER, BUFFER);
	*buffer = memory_allocator_buf_create(allocator, size, 0, 0);
	return buffer;
}

SGE_API void vulkan_render_pass_description_begin(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, vulkan_render_pass_type_t type)
{
	memzero(description, vulkan_render_pass_description_t);
	description->type = type;
	description->attachments = CAST_TO(vulkan_attachment_type_t*, create_buffer(allocator, vulkan_attachment_type_t));
	description->input_attachments = CAST_TO(u32*, create_buffer(allocator, u32));
	description->subpass_descriptions = CAST_TO(vulkan_subpass_description_t*, create_buffer(allocator, vulkan_subpass_description_t));
	description->subpass_dependencies = CAST_TO(VkSubpassDependency*, create_buffer(allocator, VkSubpassDependency));
	description->render_set_bindings = CAST_TO(vulkan_shader_resource_description_t*, create_buffer(allocator, vulkan_shader_resource_description_t));
}

SGE_API void vulkan_render_pass_description_add_input(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, glsl_type_t type, u32 index, u32 binding)
{
	buf_push_auto(CAST_TO(BUFFER*, description->input_attachments), index);
	vulkan_shader_resource_description_create_opaque(allocator, create_element(CAST_TO(BUFFER*, description->render_set_bindings)), "internal", type, VULKAN_DESCRIPTOR_SET_RENDER, binding);
}

SGE_API void vulkan_render_pass_description_add_attachment(vulkan_render_pass_description_t* description, vulkan_attachment_type_t type)
{
	buf_push_auto(CAST_TO(BUFFER*, description->attachments), type);
}

SGE_API void vulkan_render_pass_description_begin_subpass(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, u32 pipeline_index)
{
	vulkan_subpass_description_t* subpass = create_element(CAST_TO(BUFFER*, description->subpass_descriptions));
	subpass->pipeline_description_index = pipeline_index;
	subpass->depth_stencil_attachment = U32_MAX;
	subpass->color_attachments = CAST_TO(u32*, create_buffer(allocator, u32));
	subpass->input_attachments = CAST_TO(u32*, create_buffer(allocator, u32));
	subpass->preserve_attachments = CAST_TO(u32*, create_buffer(allocator, u32));

	subpass->sub_render_set_bindings = CAST_TO(vulkan_shader_resource_description_t*, create_buffer(allocator, vulkan_shader_resource_description_t));
}

SGE_API void vulkan_render_pass_description_add_attachment_reference(memory_allocator_t* allocator, vulkan_render_pass_description_t* description, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, description->subpass_descriptions));
	switch(type)
	{
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR:
			buf_push_auto(CAST_TO(BUFFER*, subpass->color_attachments), reference);
			break;
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT:
			buf_push_auto(CAST_TO(BUFFER*, subpass->input_attachments), reference);
			vulkan_shader_resource_description_create_opaque(allocator, create_element(CAST_TO(BUFFER*, subpass->sub_render_set_bindings)), "internal", GLSL_TYPE_SUBPASS_INPUT, VULKAN_DESCRIPTOR_SET_SUB_RENDER, binding);
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

SGE_API void vulkan_render_pass_description_end_subpass(memory_allocator_t* allocator, vulkan_render_pass_description_t* description)
{
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, description->subpass_descriptions));
	BUFFER* buffer;
	
	buffer = CAST_TO(BUFFER*, subpass->color_attachments);
	subpass->color_attachment_count = buf_get_element_count(buffer);
	subpass->color_attachments = (subpass->color_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	
	buffer = CAST_TO(BUFFER*, subpass->input_attachments);
	subpass->input_attachment_count = buf_get_element_count(buffer);
	subpass->input_attachments = (subpass->input_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);

	buffer = CAST_TO(BUFFER*, subpass->preserve_attachments);
	subpass->preserve_attachment_count = buf_get_element_count(buffer);
	subpass->preserve_attachments = (subpass->preserve_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);

	buffer = CAST_TO(BUFFER*, subpass->sub_render_set_bindings);
	subpass->sub_render_set_binding_count = buf_get_element_count(buffer);
	subpass->sub_render_set_bindings = (subpass->sub_render_set_binding_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
}

SGE_API void vulkan_render_pass_description_add_subpass_dependency(vulkan_render_pass_description_t* description, VkSubpassDependency* dependency)
{
	VkSubpassDependency* dst_dependency = create_element(CAST_TO(BUFFER*, description->subpass_dependencies));
	memcopy(dst_dependency, dependency, VkSubpassDependency);
}

SGE_API void vulkan_render_pass_description_end(memory_allocator_t* allocator, vulkan_render_pass_description_t* description)
{
	BUFFER* buffer = CAST_TO(BUFFER*, description->attachments);
	description->attachment_count = buf_get_element_count(buffer);
	description->attachments = (description->attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->input_attachments);
	description->input_attachment_count = buf_get_element_count(buffer);
	description->input_attachments = (description->input_attachment_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->subpass_descriptions);
	description->subpass_count = buf_get_element_count(buffer);
	description->subpass_descriptions = (description->subpass_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->subpass_dependencies);
	description->subpass_dependency_count = buf_get_element_count(buffer);
	description->subpass_dependencies = (description->subpass_dependency_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->render_set_bindings);
	description->render_set_binding_count = buf_get_element_count(buffer);
	description->render_set_bindings = (description->render_set_binding_count == 0) ? NULL : buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
}

SGE_API void vulkan_render_pass_description_destroy_allocations(memory_allocator_t* allocator, vulkan_render_pass_description_t* description)
{
	if(description->attachment_count > 0)
		memory_allocator_dealloc(allocator, description->attachments);
	if(description->input_attachment_count > 0)
		memory_allocator_dealloc(allocator, description->input_attachments);
	if(description->subpass_count > 0)
	{
		for(u32 i = 0; i < description->subpass_count; i++)
		{
			AUTO subpass = &description->subpass_descriptions[i];
			if(subpass->color_attachment_count > 0)
				memory_allocator_dealloc(allocator, subpass->color_attachments);
			if(subpass->input_attachment_count > 0)
				memory_allocator_dealloc(allocator, subpass->input_attachments);
			if(subpass->preserve_attachment_count > 0)
				memory_allocator_dealloc(allocator, subpass->preserve_attachments);
			if(subpass->sub_render_set_binding_count > 0)
			{
				for(u32 i = 0; i < subpass->sub_render_set_binding_count; i++)
					struct_descriptor_destroy(allocator, &subpass->sub_render_set_bindings[i].handle);
				memory_allocator_dealloc(allocator, subpass->sub_render_set_bindings);
			}
		}
	}
	if(description->subpass_count > 0)
		memory_allocator_dealloc(allocator, description->subpass_descriptions);
	if(description->subpass_dependency_count > 0)
		memory_allocator_dealloc(allocator, description->subpass_dependencies);
	if(description->render_set_binding_count > 0)
		memory_allocator_dealloc(allocator, description->render_set_bindings);
}
