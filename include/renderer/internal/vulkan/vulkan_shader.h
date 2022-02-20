
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_stage_shader.h>
#include <renderer/struct_descriptor.h>
#include <bufferlib/buffer.h>
#include <shader_compiler/settings_parser.h>

typedef struct vulkan_descriptor_set_t vulkan_descriptor_set_t;

typedef struct vulkan_shader_resource_descriptor_t
{
	struct_descriptor_t handle;
	bool is_push_constant;
	union
	{
		struct
		{
			bool is_per_vertex_attribute;
			bool is_per_instance_attribute;
		};
		u16 is_attribute;
	};
	bool is_opaque;
	bool is_uniform;
	union
	{
		u8 binding_number;
		u8 vertex_attrib_location_number;
	};
	union 
	{
		u8 set_number;
		u8 vertex_attrib_binding_number;
		u8 push_constant_range_offset;
	};
	//NOTE: These bit descriptions are based on the vulkan_vulkan_shader_stage_t and vulkan_shader_stage_t enums
	u8 stage_flags; 	// BIT(0) = vertex shader, BIT(1) = fragment shader, BIT(2) = geometry shader, BIT(3) = tessellation shader
} vulkan_shader_resource_descriptor_t;

typedef struct vulkan_shader_t
{
	vulkan_renderer_t* renderer;
	vulkan_stage_shader_t** stage_shaders;
	u8 stage_count;
	//For now we will be using only one descriptor set and descriptor layout
	vulkan_descriptor_set_t* vk_set;
	VkDescriptorSetLayout vk_set_layout;
	vulkan_shader_resource_descriptor_t* descriptors;
	u16 descriptor_count;
	GraphicsPipeline pipelineSettings;
} vulkan_shader_t;

BEGIN_CPP_COMPATIBLE

#define vulkan_shader_load(renderer, file_path) vulkan_shader_load_and_create(renderer, file_path)

RENDERER_API vulkan_shader_t* vulkan_shader_new();
RENDERER_API vulkan_shader_t* vulkan_shader_create(vulkan_renderer_t* renderer, BUFFER* vulkan_shader_binary);
RENDERER_API vulkan_shader_t* vulkan_shader_load_and_create(vulkan_renderer_t* renderer, const char* file_path);
RENDERER_API void vulkan_shader_destroy(vulkan_shader_t* shader);
RENDERER_API void vulkan_shader_release_resources(vulkan_shader_t* shader);

END_CPP_COMPATIBLE
