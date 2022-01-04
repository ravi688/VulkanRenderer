
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/internal/vulkan/vulkan_stage_shader.h>
#include <renderer/struct_descriptor.h>
#include <buffer.h>

typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;

typedef struct vulkan_shader_resource_descriptor_t
{
	struct_descriptor_t handle;
	bool is_opaque;
	bool is_uniform;
	u8 binding_number;
	u8 set_number;
	//NOTE: These bit descriptions are based on the vulkan_vulkan_shader_stage_t and vulkan_shader_stage_t enums
	u8 stage_flags; 	// BIT(0) = vertex shader, BIT(1) = fragment shader, BIT(2) = geometry shader, BIT(3) = tessellation shader
} vulkan_shader_resource_descriptor_t;

typedef struct vulkan_shader_t
{
	vulkan_stage_shader_t** stage_shaders;
	u8 stage_count;
	//For now we will be using only one descriptor set and descriptor layout
	vulkan_descriptor_set_t* vk_set;
	VkDescriptorSetLayout vk_set_layout;
	vulkan_shader_resource_descriptor_t* descriptors;
	u16 descriptor_count;
} vulkan_shader_t;

#define vulkan_shader_load(renderer, file_path) vulkan_shader_load_and_create(renderer, file_path)

vulkan_shader_t* vulkan_shader_new();
vulkan_shader_t* vulkan_shader_create(renderer_t* renderer, BUFFER* vulkan_shader_binary);
vulkan_shader_t* vulkan_shader_load_and_create(renderer_t* renderer, const char* file_path);
void vulkan_shader_destroy(vulkan_shader_t* shader, renderer_t* renderer);
void vulkan_shader_release_resources(vulkan_shader_t* shader);
