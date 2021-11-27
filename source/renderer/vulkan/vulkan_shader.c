
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

#include <renderer/assert.h>
#include <memory_allocator/memory_allocator.h>
#include <memory.h>

vulkan_shader_t* vulkan_shader_create(renderer_t* renderer, const char* file_name, vulkan_vulkan_shader_type_t vulkan_shader_type)
{
	ASSERT(renderer->vk_device != VK_NULL_HANDLE, "renderer->vk_device == VK_NULL_HANDLE\n");
	vulkan_shader_t* shader = heap_new(vulkan_shader_t);
	memset(shader, 0, sizeof(vulkan_shader_t));
	shader->module = vk_get_shader_module(renderer->vk_device, file_name);
	shader->stage = vk_get_pipeline_shader_stage_create_info(shader->module, vulkan_shader_type, "main");
	return shader;
}


void vulkan_shader_destroy(vulkan_shader_t* shader, renderer_t* renderer)
{
	vkDestroyShaderModule(renderer->vk_device, shader->module, NULL);
	heap_free(shader);
}
