
#include <renderer/shader.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

RENDERER_API shader_t* shader_create(renderer_t* renderer, shader_create_info_t* create_info)
{
	return CAST_TO(shader_t*, vulkan_shader_create(renderer->vulkan_handle, create_info));
}

RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path)
{
	vulkan_shader_load_info_t load_info =
	{
		.path = file_path,
		.is_vertex_attrib_from_file = true
	};
	return CAST_TO(shader_t*, vulkan_shader_load(renderer->vulkan_handle, &load_info));
}

RENDERER_API void shader_destroy(shader_t* shader)
{
	vulkan_shader_destroy(CAST_TO(vulkan_shader_t*, shader));
}

RENDERER_API void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(CAST_TO(vulkan_shader_t*, shader));
}
