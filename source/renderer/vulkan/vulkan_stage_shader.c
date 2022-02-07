#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_stage_shader.h>
#include <disk_manager/file_reader.h>

#include <renderer/assert.h>
#include <renderer/memory_allocator.h>
#include <memory.h>

static VkShaderModule get_shader_module(VkDevice device, void* spirv, uint32_t length);
static VkPipelineShaderStageCreateInfo get_pipeline_shader_stage_create_info(VkShaderModule shader_module, vulkan_shader_type_t vulkan_shader_type, const char* entry_point);


RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_new()
{
	vulkan_stage_shader_t* shader = heap_new(vulkan_stage_shader_t);
	memset(shader, 0, sizeof(vulkan_stage_shader_t));
	return shader;
}

RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_create(vulkan_renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type)
{
	vulkan_stage_shader_t* shader = vulkan_stage_shader_new();
	vulkan_stage_shader_create_no_alloc(renderer, spirv, length, shader_type, shader);
	return shader;
}

RENDERER_API void vulkan_stage_shader_create_no_alloc(vulkan_renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	assert(shader != NULL);
	
	shader->module = get_shader_module(renderer->logical_device->handle, spirv, length);
	shader->stage = get_pipeline_shader_stage_create_info(shader->module, shader_type, "main");
}

RENDERER_API void vulkan_stage_shader_load_and_create_no_alloc(vulkan_renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	BUFFER* shader_bytes = load_binary_from_file(file_name);
	shader->module = get_shader_module(renderer->logical_device->handle, shader_bytes->bytes, shader_bytes->element_count);
	shader->stage = get_pipeline_shader_stage_create_info(shader->module, shader_type, "main");
	buf_free(shader_bytes);
}

RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_load_and_create(vulkan_renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	
	vulkan_stage_shader_t* shader = vulkan_stage_shader_new();
	vulkan_stage_shader_load_and_create_no_alloc(renderer, file_name, shader_type, shader);
	
	return shader;
}


RENDERER_API void vulkan_stage_shader_destroy(vulkan_stage_shader_t* shader, vulkan_renderer_t* renderer)
{
	vkDestroyShaderModule(renderer->logical_device->handle, shader->module, NULL);
}

RENDERER_API void vulkan_stage_shader_release_resources(vulkan_stage_shader_t* shader)
{
	heap_free(shader);
}

 static VkShaderModule get_shader_module(VkDevice device, void* spirv, uint32_t length)
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
