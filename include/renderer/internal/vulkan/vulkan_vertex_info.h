
#pragma once

#include <renderer/defines.h>

typedef enum VkFormat VkFormat;

typedef struct vulkan_vertex_info_t
{
	u32 size;
	u32 attribute_count;
	VkFormat* attribute_formats;
	u32* attribute_offsets;
} vulkan_vertex_info_t;
