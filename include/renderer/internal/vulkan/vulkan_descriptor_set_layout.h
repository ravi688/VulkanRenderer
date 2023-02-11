
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>


typedef struct vulkan_shader_resource_description_t vulkan_shader_resource_description_t;

typedef struct vulkan_descriptor_set_layout_t
{
	vulkan_renderer_t* renderer;
	VkDescriptorSetLayout vo_handle;
} vulkan_descriptor_set_layout_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_new(memory_allocator_t* allocator);
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count);
RENDERER_API void vulkan_descriptor_set_layout_create_no_alloc(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create_from_resource_descriptors(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* bindings, u32 binding_count);
RENDERER_API void vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_resource_description_t* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
RENDERER_API void vulkan_descriptor_set_layout_destroy(vulkan_descriptor_set_layout_t* layout);
RENDERER_API void vulkan_descriptor_set_layout_release_resources(vulkan_descriptor_set_layout_t* layout);

END_CPP_COMPATIBLE