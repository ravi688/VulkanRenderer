
#pragma once

#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>
#include <glslcommon/glsl_types.h>

typedef struct vulkan_shader_resource_description_t
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
} vulkan_shader_resource_description_t;


RENDERER_API void vulkan_shader_resource_description_add_opaque(vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 set_number, u32 binding_number);

RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_begin_uniform(vulkan_shader_resource_description_t* description, const char* name, u32 set_number, u32 binding_number);

RENDERER_API void vulkan_shader_resource_description_end_uniform(vulkan_shader_resource_description_t* description);
