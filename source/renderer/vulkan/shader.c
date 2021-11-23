
#include <renderer/renderer.h>
#include <renderer/shader.h>
#include <renderer/internal/vulkan.h>
#include <vulkan/vulkan_wrapper.h>
#include <memory_allocator/memory_allocator.h>

typedef struct shader_t
{
	VkShaderModule vk_module;
} shader_t;


shader_t* shader_new(renderer_t* r, const char* file_path)
{
	shader_t* shader = heap_new(shader_t);
	shader->vk_module = vk_get_shader_module(renderer->vk_device, file_path);
	return shader;
}

void shader_destroy(renderer_t* r, shader_t* shader)
{
	vkDestroyShaderModule(r->vk_device, shader->vk_module, NULL);
	heap_free(shader);
}
