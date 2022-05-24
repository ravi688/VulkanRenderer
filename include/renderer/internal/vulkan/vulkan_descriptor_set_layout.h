
#pragma once

#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>

typedef struct vulkan_shader_resource_descriptor_t
{
	struct_descriptor_t handle;
	bool is_push_constant;						// is this a push constant ?
	union
	{
		struct
		{
			bool is_per_vertex_attribute; 		// does this represents per vertex attribute ?
			bool is_per_instance_attribute; 	// does this represents per instance attribute ?
		};
		u16 is_attribute;						// is this an attribute ?
	};
	bool is_opaque;								// is this an opaque handle to a resource ?
	bool is_uniform;							// is this an uniform resource ?
	union
	{
		u8 binding_number; 						// if this is a descriptor then what is the binding number ?
		u8 vertex_attrib_location_number;		// if this is an attribute then what is the location number ?
	};
	union 
	{
		u8 set_number; 							// if this is a descriptor then what is the set number ?
		u8 vertex_attrib_binding_number;		// if this is an attribute then what is the binding number ?
		u8 push_constant_range_offset; 			// if this is a push constant then what is the range offset ?
	};
	//NOTE: These bit descriptions are based on the vulkan_shader_stage_t and vulkan_shader_stage_t enums
	u8 stage_flags; 	// BIT(0) = vertex shader, BIT(1) = fragment shader, BIT(2) = geometry shader, BIT(3) = tessellation shader
} vulkan_shader_resource_descriptor_t;

typedef struct vulkan_descriptor_set_layout_t
{
	vulkan_renderer_t* renderer;
	VkDescriptorSetLayout vo_handle;
} vulkan_descriptor_set_layout_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_new();
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count);
RENDERER_API void vulkan_descriptor_set_layout_create_no_alloc(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create_from_resource_descriptors(vulkan_renderer_t* renderer, vulkan_shader_resource_descriptor_t* bindings, u32 binding_count);
RENDERER_API void vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_resource_descriptor_t* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout);
RENDERER_API void vulkan_descriptor_set_layout_destroy(vulkan_descriptor_set_layout_t* layout);
RENDERER_API void vulkan_descriptor_set_layout_release_resources(vulkan_descriptor_set_layout_t* layout);

END_CPP_COMPATIBLE