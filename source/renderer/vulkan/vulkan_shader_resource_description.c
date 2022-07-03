
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_types.h>

RENDERER_API void vulkan_shader_resource_description_add_opaque(vulkan_shader_resource_description_t* description, const char* name, glsl_type_t type, u32 set_number, u32 binding_number)
{
	description->is_opaque = true;
	description->set_number = set_number;
	description->binding_number = binding_number;
	// NOTE: shader stage must be FRAGMENT for subpassInput
	description->stage_flags = (1 << VULKAN_SHADER_TYPE_FRAGMENT); //| (1 << VULKAN_SHADER_TYPE_VERTEX);

	struct_descriptor_begin(&description->handle, name, type);
	struct_descriptor_end(&description->handle);
}

RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_begin_uniform(vulkan_shader_resource_description_t* description, const char* name, u32 set_number, u32 binding_number)
{
	description->is_uniform = true;
	description->set_number = set_number;
	description->binding_number = binding_number;
	description->stage_flags = (1 << VULKAN_SHADER_TYPE_VERTEX) | (1 << VULKAN_SHADER_TYPE_FRAGMENT);

	struct_descriptor_begin(&description->handle, name, GLSL_TYPE_BLOCK);
	return &description->handle;
}

RENDERER_API void vulkan_shader_resource_description_end_uniform(vulkan_shader_resource_description_t* description)
{
	struct_descriptor_end(&description->handle);
}

