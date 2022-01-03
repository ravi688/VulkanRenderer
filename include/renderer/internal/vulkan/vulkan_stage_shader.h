
#pragma once

#include <vulkan/vulkan_wrapper.h>

typedef struct renderer_t renderer_t;

typedef struct vulkan_stage_shader_t
{
	VkShaderModule module;
	VkPipelineShaderStageCreateInfo stage;
} vulkan_stage_shader_t;


vulkan_stage_shader_t* vulkan_stage_shader_new();
vulkan_stage_shader_t* vulkan_stage_shader_create(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type);
void vulkan_stage_shader_create_no_alloc(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader);
vulkan_stage_shader_t* vulkan_stage_shader_load_and_create(renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type);
void vulkan_stage_shader_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader);

void vulkan_stage_shader_destroy(vulkan_stage_shader_t* shader, renderer_t* renderer);
void vulkan_stage_shader_release_resources(vulkan_stage_shader_t* shader);
