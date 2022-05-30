
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

/* TODO: rename it to vulkan_attribute_binding_info_t*/
typedef struct vulkan_vertex_info_t
{
	u32 input_rate;
	u32 size;
	u32 attribute_count;
	u16 binding;
	u16* attribute_locations;
	VkFormat* attribute_formats;
	u32* attribute_offsets;
} vulkan_vertex_info_t;

typedef enum vulkan_shader_type_t
{
	VULKAN_SHADER_TYPE_VERTEX = 0,
	VULKAN_SHADER_TYPE_FRAGMENT = 1,
	VULKAN_SHADER_TYPE_GEOMETRY = 2,
	VULKAN_SHADER_TYPE_TESSELLATION = 3
} vulkan_shader_type_t;

/* TODO: remove it from here because there are not references */
typedef struct vertex_attribute_binding_info_t
{
	u32 input_rate;
	u32 attribute_count;
	VkFormat* attribute_formats;
	u32* attribute_offsets;
} vertex_attribute_binding_info_t;

