
#include <renderer/shader.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

RENDERER_API shader_t* shader_create(renderer_t* renderer, BUFFER* shader_binary)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API void shader_destroy(shader_t* shader)
{
	vulkan_shader_destroy(shader);
}

RENDERER_API void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(shader);
}
