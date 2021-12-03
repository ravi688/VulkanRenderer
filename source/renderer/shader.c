
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/shader.h>

shader_t* shader_new() { return vulkan_shader_new(); }

shader_t* shader_create(renderer_t* renderer, const char* file_name, shader_type_t shader_type)
{
	return vulkan_shader_create(renderer, file_name, shader_type);
}
void shader_create_no_alloc(renderer_t* renderer, const char* file_name, shader_type_t shader_type, shader_t* shader)
{
	vulkan_shader_create_no_alloc(renderer, file_name, shader_type, shader);
}

void shader_destroy(shader_t* shader, renderer_t* renderer)
{
	vulkan_shader_destroy(shader, renderer);
}

void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(shader);
}

