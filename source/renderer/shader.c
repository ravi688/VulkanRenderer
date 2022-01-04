
#include <renderer/shader.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary)
{
	return vulkan_shader_create(renderer, shader_binary);
}

shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path)
{
	return vulkan_shader_load_and_create(renderer, file_path);
}

void shader_destroy(shader_t* shader, renderer_t* renderer)
{
	vulkan_shader_destroy(shader, renderer);
}

void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(shader);
}
