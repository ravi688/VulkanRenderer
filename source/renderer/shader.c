
#include <renderer/shader.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

RENDERER_API shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary)
{
	return vulkan_shader_create(renderer->handle, shader_binary);
}

RENDERER_API shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path)
{
	return vulkan_shader_load_and_create(renderer->handle, file_path);
}

RENDERER_API void shader_destroy(shader_t* shader)
{
	vulkan_shader_destroy(shader);
}

RENDERER_API void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(shader);
}
