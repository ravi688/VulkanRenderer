
#pragma once

#include <vulkan/vulkan_wrapper.h>

typedef renderer_t renderer_t;

typedef struct vulkan_shader_t
{
	VkShaderModule module;
	VkPipelineShaderStageCreateInfo stage;
} vulkan_shader_t;


vulkan_shader_t* vulkan_shader_create(renderer_t* renderer, const char* file_name, vulkan_vulkan_shader_type_t vulkan_shader_type);
void vulkan_shader_destroy(vulkan_shader_t* shader, renderer_t* renderer);

