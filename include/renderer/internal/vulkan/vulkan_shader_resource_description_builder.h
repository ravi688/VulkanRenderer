#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <bufferlib/buffer.h>  // buffer_t


typedef struct vulkan_shader_resource_description_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_shader_resource_description_t objects */
	buffer_t description_array;
	u32 bind_index;
} vulkan_shader_resource_description_builder_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_shader_resource_description_builder_t* vulkan_shader_resource_description_builder_create(memory_allocator_t* allocator);
RENDERER_API void vulkan_shader_resource_description_builder_destroy(vulkan_shader_resource_description_builder_t* builder);

RENDERER_API void vulkan_shader_resource_description_builder_add(vulkan_shader_resource_description_builder_t* builder, u32 count);
RENDERER_API void vulkan_shader_resource_description_builder_bind(vulkan_shader_resource_description_builder_t* builder, u32 index);

RENDERER_API vulkan_shader_resource_description_t* vulkan_shader_resource_description_builder_get(vulkan_shader_resource_description_builder_t* builder);
RENDERER_API u32 vulkan_shader_resource_description_builder_get_count(vulkan_shader_resource_description_builder_t* builder);

RENDERER_API void vulkan_shader_resource_description_builder_create_opaque(vulkan_shader_resource_description_builder_t* builder, const char* name, glsl_type_t type, u32 set_number, u32 binding_number);
RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_builder_create_uniform(vulkan_shader_resource_description_builder_t* builder, const char* name, u32 set_number, u32 binding_number);
RENDERER_API void vulkan_shader_resource_description_builder_end_uniform(vulkan_shader_resource_description_builder_t* builder);

END_CPP_COMPATIBLE
