/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_loader.h is a part of VulkanRenderer

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

#include <sge/defines.h>

typedef enum vulkan_shader_binary_version_t
{
	VULKAN_SHADER_BINARY_VERSION_UNDEFINED = 0UL,
	VULKAN_SHADER_BINARY_VERSION_2021,
	VULKAN_SHADER_BINARY_VERSION_2022,
	VULKAN_SHADER_BINARY_VERSION_MAX
} vulkan_shader_binary_version_t;

#define VULKAN_SHADER_HEADER_STR "V3D Shader Binary"

typedef enum vulkan_resource_descriptor_type_t
{
	/* resource descriptor is a block with push_constant layout qualifier.
		{ fragment | vertex | geometry | tessellation } [push_constant] uniform block_name { }
	 */
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT,
	/* resource descriptor is an opaque handle, such as samplerCube 
		{ fragment | vertex | geometry | tessellation } [S,B] uniform samplerCube name;
	 */
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE,
	/* resource descriptor is a uniform buffer
		{ fragment | vertex | geometry | tessellation } [S,B] uniform block_name { }
	 */
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	/* resource descriptor is a sampler2D 
		{ fragment | vertex | geometry | tessellation } [S,B] buffer block_name { }
	 */
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	/* resource descriptor is a per vertex attribute
	 */
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB,
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_MAX,

	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_STORAGE_BUFFER),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB),
	VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB_BIT = BIT32(VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB)

} vulkan_resource_descriptor_type_t;

/* decode descriptor_info (u32) */
#define VULKAN_SHADER_STAGE_BITS(descriptor_info) (((descriptor_info) & 0xFF00UL) >> 8)
#define VULKAN_GLSL_TYPE_BITS(descriptor_info) ((descriptor_info) & 0xFFUL)
#define VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_BITS(descriptor_info) (((descriptor_info) & 0xFF0000UL) >> 16)
