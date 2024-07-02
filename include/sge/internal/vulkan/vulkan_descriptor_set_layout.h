/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_descriptor_set_layout.h is a part of VulkanRenderer

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
#include <sge/defines.h>
#include <sge/internal/vulkan/vulkan_object.h>


typedef struct vulkan_shader_resource_description_t vulkan_shader_resource_description_t;

typedef struct vulkan_descriptor_set_layout_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkDescriptorSetLayout vo_handle;
} vulkan_descriptor_set_layout_t;

#define VULKAN_DESCRIPTOR_SET_LAYOUT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_descriptor_set_layout_t*, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, ptr)
#define VULKAN_DESCRIPTOR_SET_LAYOUT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_descriptor_set_layout_t*, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_new(memory_allocator_t* allocator);
SGE_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count);
SGE_API void vulkan_descriptor_set_layout_create_no_alloc(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
SGE_API void vulkan_descriptor_set_layout_create_no_alloc_ext(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
SGE_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create_from_resource_descriptors(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* bindings, u32 binding_count);
SGE_API void vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
SGE_API void vulkan_descriptor_set_layout_destroy(vulkan_descriptor_set_layout_t* layout);
SGE_API void vulkan_descriptor_set_layout_release_resources(vulkan_descriptor_set_layout_t* layout);

END_CPP_COMPATIBLE