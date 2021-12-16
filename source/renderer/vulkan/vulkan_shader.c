
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_shader_t* vulkan_shader_new()
{
	vulkan_shader_t* shader = heap_new(vulkan_shader_t);
	memset(shader, 0, sizeof(vulkan_shader_t));
	return refp(vulkan_shader_t, shader, 0);
}

vulkan_shader_t* vulkan_shader_create(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t vulkan_shader_type)
{
	vulkan_shader_t* shader = vulkan_shader_new();
	vulkan_shader_create_no_alloc(renderer, spirv, length, vulkan_shader_type, shader);
	return shader;
}

void vulkan_shader_create_no_alloc(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t vulkan_shader_type, vulkan_shader_t* shader)
{
	assert(renderer->vk_device != VK_NULL_HANDLE);
	assert(shader != NULL);
	shader->module = vk_get_shader_module(renderer->vk_device, spirv, length, vulkan_shader_type);
	shader->stage = vk_get_pipeline_shader_stage_create_info(refp(vulkan_shader_t, shader, 0)->module, vulkan_shader_type, "main");
}

void vulkan_shader_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, vulkan_shader_type_t vulkan_shader_type, vulkan_shader_t* shader)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	refp(vulkan_shader_t, shader, 0)->module = vk_get_shader_module_load(renderer->vk_device, file_name);
	refp(vulkan_shader_t, shader, 0)->stage = vk_get_pipeline_shader_stage_create_info(refp(vulkan_shader_t, shader, 0)->module, vulkan_shader_type, "main");
}

vulkan_shader_t* vulkan_shader_load_and_create(renderer_t* renderer, const char* file_name, vulkan_shader_type_t vulkan_shader_type)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_shader_t* shader = vulkan_shader_new();
	vulkan_shader_load_and_create_no_alloc(renderer, file_name, vulkan_shader_type, shader);
	return shader;
}


void vulkan_shader_destroy(vulkan_shader_t* shader, renderer_t* renderer)
{
	vkDestroyShaderModule(renderer->vk_device, refp(vulkan_shader_t, shader, 0)->module, NULL);
}

void vulkan_shader_release_resources(vulkan_shader_t* shader)
{
	heap_free(shader);
}
