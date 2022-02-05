
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_stage_shader.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_new()
{
	vulkan_stage_shader_t* shader = heap_new(vulkan_stage_shader_t);
	memset(shader, 0, sizeof(vulkan_stage_shader_t));
	return refp(vulkan_stage_shader_t, shader, 0);
}

RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_create(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type)
{
	vulkan_stage_shader_t* shader = vulkan_stage_shader_new();
	vulkan_stage_shader_create_no_alloc(renderer, spirv, length, shader_type, shader);
	return shader;
}

RENDERER_API void vulkan_stage_shader_create_no_alloc(renderer_t* renderer, void* spirv, uint32_t length, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader)
{
	assert(renderer->logical_device->handle != VK_NULL_HANDLE);
	assert(shader != NULL);
	shader->module = vk_get_shader_module(renderer->logical_device->handle, spirv, length);
	shader->stage = vk_get_pipeline_shader_stage_create_info(refp(vulkan_stage_shader_t, shader, 0)->module, shader_type, "main");
}

RENDERER_API void vulkan_stage_shader_load_and_create_no_alloc(renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type, vulkan_stage_shader_t* shader)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	refp(vulkan_stage_shader_t, shader, 0)->module = vk_get_shader_module_load(renderer->logical_device->handle, file_name);
	refp(vulkan_stage_shader_t, shader, 0)->stage = vk_get_pipeline_shader_stage_create_info(refp(vulkan_stage_shader_t, shader, 0)->module, shader_type, "main");
}

RENDERER_API vulkan_stage_shader_t* vulkan_stage_shader_load_and_create(renderer_t* renderer, const char* file_name, vulkan_shader_type_t shader_type)
{
	ASSERT(renderer->logical_device->handle != VK_NULL_HANDLE, "renderer->logical_device->handle == VK_NULL_HANDLE\n");
	vulkan_stage_shader_t* shader = vulkan_stage_shader_new();
	vulkan_stage_shader_load_and_create_no_alloc(renderer, file_name, shader_type, shader);
	return shader;
}


RENDERER_API void vulkan_stage_shader_destroy(vulkan_stage_shader_t* shader, renderer_t* renderer)
{
	vkDestroyShaderModule(renderer->logical_device->handle, refp(vulkan_stage_shader_t, shader, 0)->module, NULL);
}

RENDERER_API void vulkan_stage_shader_release_resources(vulkan_stage_shader_t* shader)
{
	heap_free(shader);
}
