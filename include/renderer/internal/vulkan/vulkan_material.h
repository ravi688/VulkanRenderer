
#pragma once

#include <vulkan/vulkan_wrapper.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_vertex_info.h>

typedef struct renderer_t renderer_t;
typedef struct vulkan_graphics_pipeline_t vulkan_graphics_pipeline_t;
typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;

typedef struct vulkan_material_create_info_t
{
	vulkan_shader_t* shader;
	uint32_t vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
	uint32_t push_constant_range_count;
	VkPushConstantRange* push_constant_ranges;
	VkDescriptorSetLayout vk_set_layout;
	bool is_transparent;
} vulkan_material_create_info_t;

typedef struct vulkan_material_t
{ 
	//For event arguments
	renderer_t* renderer;
	void* self_reference; 	//self_reference

	//For recreating the graphics pipeline on render window resize
	uint32_t vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
	uint32_t push_constant_range_count;
	VkPushConstantRange* push_constant_ranges;
	bool is_transparent;
	
	vulkan_shader_t* shader;
	vulkan_graphics_pipeline_t* graphics_pipeline;
} vulkan_material_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API vulkan_material_t* vulkan_material_new();
RENDERER_API vulkan_material_t* vulkan_material_create(renderer_t* renderer, vulkan_material_create_info_t* create_info);
RENDERER_API void vulkan_material_create_no_alloc(renderer_t* renderer, vulkan_material_create_info_t* create_info, vulkan_material_t* material);
RENDERER_API void vulkan_material_destroy(vulkan_material_t* material);
RENDERER_API void vulkan_material_release_resources(vulkan_material_t* material);

RENDERER_API void vulkan_material_bind(vulkan_material_t* material);
RENDERER_API void vulkan_material_push_constants(vulkan_material_t* material, void* bytes);
RENDERER_API void vulkan_material_set_texture(vulkan_material_t* material, u32 binding_index, vulkan_texture_t* texture);
RENDERER_API void vulkan_material_set_uniform_buffer(vulkan_material_t* material, u32 binding_index, vulkan_buffer_t* buffer);

END_CPP_COMPATIBLE
