#include <renderer/internal/vulkan/vulkan_shader_module.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <disk_manager/file_reader.h>

#include <renderer/assert.h>
#include <renderer/memory_allocator.h>

static VkShaderModule get_shader_module(VkDevice device, void* spirv, u32 length);
static VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_create_info(VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point);


RENDERER_API vulkan_shader_module_t* vulkan_shader_module_new()
{
	vulkan_shader_module_t* shader = heap_new(vulkan_shader_module_t);
	memset(shader, 0, sizeof(vulkan_shader_module_t));
	return shader;
}

RENDERER_API vulkan_shader_module_t* vulkan_shader_module_create(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info)
{
	vulkan_shader_module_t* shader = vulkan_shader_module_new();
	vulkan_shader_module_create_no_alloc(renderer, create_info, shader);
	return shader;
}

RENDERER_API void vulkan_shader_module_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_create_info_t* create_info, vulkan_shader_module_t* shader)
{
	shader->vo_module = get_shader_module(renderer->logical_device->vo_handle, spirv, length);
	shader->vo_stage = get_pipeline_shader_stage_create_info(shader->vo_module, shader_type, "main");
}

RENDERER_API void vulkan_shader_module_load_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info, vulkan_shader_module_t OUT shader)
{
	BUFFER* shader_bytes = load_binary_from_file(load_info->file_path);
	shader->vo_module = get_shader_module(renderer->logical_device->vo_handle, shader_bytes->bytes, shader_bytes->element_count);
	shader->vo_stage = get_pipeline_shader_stage_create_info(shader->vo_module, load_info->type, "main");
	buf_free(shader_bytes);
}

RENDERER_API vulkan_shader_module_t* vulkan_shader_module_load(vulkan_renderer_t* renderer, vulkan_shader_module_load_info_t* load_info)
{
	vulkan_shader_module_t* shader = vulkan_shader_module_new();
	vulkan_shader_module_load_no_alloc(renderer, load_info, shader);
	return shader;
}

RENDERER_API void vulkan_shader_module_destroy(vulkan_shader_module_t* shader, vulkan_renderer_t* renderer)
{
	vkDestroyShaderModule(renderer->logical_device->vo_handle, shader->vo_module, NULL);
}

RENDERER_API void vulkan_shader_module_release_resources(vulkan_shader_module_t* shader)
{
	heap_free(shader);
}

 static VkShaderModule get_shader_module(VkDevice device, void* spirv, u32 length)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = length;
	createInfo.pCode = spirv;
	VkShaderModule shader_module;
	vkCall(vkCreateShaderModule(device, &createInfo, NULL, &shader_module));
	return shader_module;
}

static VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_create_info(VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point)
{
	VkPipelineShaderStageCreateInfo createInfo = { }; 
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.pName = entry_point; 
	createInfo.module = shader_module; 
	switch(vulkan_shader_type)
	{
		case VULKAN_SHADER_TYPE_VERTEX:
			createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case VULKAN_SHADER_TYPE_FRAGMENT:
			createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case VULKAN_SHADER_TYPE_GEOMETRY:
			createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case VULKAN_SHADER_TYPE_TESSELLATION:
			createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		default:
			LOG_FETAL_ERR("Unsupported shader type");
			break;
	}
	return createInfo;
}
