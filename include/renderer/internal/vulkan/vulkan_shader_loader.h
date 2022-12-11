#pragma once

#include <renderer/defines.h>

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
