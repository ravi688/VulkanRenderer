

#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan_wrapper.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;

typedef struct vulkan_descriptor_set_create_info_t
{
	VkDescriptorPool pool;
	VkDescriptorSetLayout layout;
} vulkan_descriptor_set_create_info_t;

typedef struct vulkan_descriptor_set_t
{
	VkDescriptorSet handle;
	VkDescriptorPool pool; //the pool from it has been allocated
} vulkan_descriptor_set_t;



vulkan_descriptor_set_t* vulkan_descriptor_set_new();
vulkan_descriptor_set_t* vulkan_descriptor_set_create(renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info);
void vulkan_descriptor_set_create_no_alloc(renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set);
void vulkan_descriptor_set_destroy(vulkan_descriptor_set_t* set, renderer_t* renderer);
void vulkan_descriptor_set_release_resources(vulkan_descriptor_set_t* set);

void vulkan_descriptor_set_bind(vulkan_descriptor_set_t* set, renderer_t* renderer, vulkan_pipeline_layout_t* pipeline_layout);
void vulkan_descriptor_set_write_texture(vulkan_descriptor_set_t* set, renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture);
void vulkan_descriptor_set_write_uniform_buffer(vulkan_descriptor_set_t* set, renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer);
