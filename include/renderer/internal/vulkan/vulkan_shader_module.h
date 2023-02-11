
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_types.h>

typedef struct vulkan_shader_module_create_info_t
{
	/* ptr the spirv binary */
	void* spirv;
	/* length of the spirv binary */
	u32 length;
	/* type of the shader*/
	vulkan_shader_type_t type;

} vulkan_shader_module_create_info_t;

typedef struct vulkan_shader_module_load_info_t
{
	/* file path to load the spirv from */
	const char* file_path;
	/* type of the shader */
	vulkan_shader_type_t type;
	
} vulkan_shader_module_load_info_t;

/* vulkan_stage_shader_t renamed to vulkan_shader_module_t */
typedef struct vulkan_shader_module_t
{
	vulkan_renderer_t* renderer;
	VkShaderModule vo_module;
	VkPipelineShaderStageCreateInfo vo_stage;
} vulkan_shader_module_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_new(memory_allocator_t* allocator);
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_create(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info);
RENDERER_API void vulkan_shader_module_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info, vulkan_shader_module_t OUT shader);
RENDERER_API vulkan_shader_module_t* vulkan_shader_module_load(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info);
RENDERER_API void vulkan_shader_module_load_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info, vulkan_shader_module_t OUT shader);
RENDERER_API void vulkan_shader_module_destroy(vulkan_shader_module_t* shader);
RENDERER_API void vulkan_shader_module_release_resources(vulkan_shader_module_t* shader);

END_CPP_COMPATIBLE
