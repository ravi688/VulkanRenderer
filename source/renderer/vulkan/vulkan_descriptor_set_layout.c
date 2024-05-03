/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_descriptor_set_layout.c is a part of VulkanRenderer

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


#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>  					// vkCall
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_types.h> 			// vulkan_shader_type_t
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

static VkDescriptorSetLayout get_null_set_layout(vulkan_renderer_t* renderer)
{
	VkDescriptorSetLayoutCreateInfo layout_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 0
	};

	VkDescriptorSetLayout layout;
	vkCall(vkCreateDescriptorSetLayout(renderer->logical_device->vo_handle, &layout_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &layout));
	return layout;
}

RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_new(memory_allocator_t* allocator)
{
	vulkan_descriptor_set_layout_t* layout = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_DESCRIPTOR_SET_LAYOUT, vulkan_descriptor_set_layout_t);
	memzero(layout, vulkan_descriptor_set_layout_t);
	layout->vo_handle = VK_NULL_HANDLE;
	return layout;
}

RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count)
{
	vulkan_descriptor_set_layout_t* layout = vulkan_descriptor_set_layout_new(renderer->allocator);
	vulkan_descriptor_set_layout_create_no_alloc(renderer, bindings, binding_count, layout);
	return layout;
}

RENDERER_API void vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* descriptors, u32 descriptor_count, vulkan_descriptor_set_layout_t OUT layout)
{
	if(descriptor_count == 0)
	{
		layout->renderer = renderer;
		layout->vo_handle = get_null_set_layout(renderer);
		return;
	}
	// allocate memory
	VkDescriptorSetLayoutBinding* bindings = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_DESCRIPTOR_SET_LAYOUT_BINDING_ARRAY, VkDescriptorSetLayoutBinding, descriptor_count);
	safe_memzerov(bindings, VkDescriptorSetLayoutBinding, descriptor_count);

	u32 binding_count = 0;
	for(u32 i = 0; i < descriptor_count; i++)
	{
		vulkan_shader_resource_description_t* descriptor = &descriptors[i];

		// if this is a push constant range or a vertex attribute then skip it
		if(descriptor->is_push_constant || vulkan_shader_resource_description_is_attribute(descriptor))
			continue;

		VkDescriptorSetLayoutBinding* binding = &bindings[binding_count];
		binding->binding = descriptor->binding_number;
		binding->descriptorCount = 1;	//for now we are not using arrays
		binding->stageFlags = 0;
		// TODO: stage_flags should be directly set as VK_SHADER_STAGE_* not other enumeration
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
			binding->stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
			binding->stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
			binding->stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
			binding->stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		switch(descriptor->handle.type)
		{
			case GLSL_TYPE_UNIFORM_BUFFER:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
			
			case GLSL_TYPE_SAMPLER_2D:
			case GLSL_TYPE_SAMPLER_3D:
			case GLSL_TYPE_SAMPLER_CUBE:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
			case GLSL_TYPE_SUBPASS_INPUT:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			break;
			
			default:
				LOG_FETAL_ERR("Cannot create set layout binding for the type \"%u\", because it is not recognized\n", descriptor->handle.type);
			break;
		}
		binding_count++;
	}
	vulkan_descriptor_set_layout_create_no_alloc(renderer, bindings, binding_count, layout);
	memory_allocator_dealloc(renderer->allocator, bindings);	
}

RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create_from_resource_descriptors(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* bindings, u32 binding_count)
{
	vulkan_descriptor_set_layout_t* layout = vulkan_descriptor_set_layout_new(renderer->allocator);
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, bindings, binding_count, layout);
	return layout;
}

RENDERER_API void vulkan_descriptor_set_layout_create_no_alloc(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout)
{
	memzero(layout, vulkan_descriptor_set_layout_t);
	
	VkDescriptorSetLayoutCreateInfo layout_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = binding_count,
		.pBindings = bindings
	};
	VkDescriptorSetLayout set_layout;
	vkCall(vkCreateDescriptorSetLayout(renderer->logical_device->vo_handle, &layout_create_info, VULKAN_ALLOCATION_CALLBACKS(renderer), &set_layout));

	layout->renderer = renderer;
	layout->vo_handle = set_layout;
}

RENDERER_API void vulkan_descriptor_set_layout_destroy(vulkan_descriptor_set_layout_t* layout)
{
	if(layout->vo_handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(layout->renderer->logical_device->vo_handle, layout->vo_handle, VULKAN_ALLOCATION_CALLBACKS(layout->renderer));
		layout->vo_handle = VK_NULL_HANDLE;
	}
}

RENDERER_API void vulkan_descriptor_set_layout_release_resources(vulkan_descriptor_set_layout_t* layout)
{
	// TODO
	// heap_free(layout);
}

