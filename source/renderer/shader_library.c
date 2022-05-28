#include <renderer/shader_library.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>

/* constructors & destructors */
RENDERER_API shader_library_t* shader_library_new()
{
	return vulkan_shader_library_new();
}

RENDERER_API shader_library_t* shader_library_create(renderer_t* renderer)
{
	return vulkan_shader_library_create(renderer->vulkan_handle);
}

RENDERER_API shader_library_t* shader_library_load_folder(renderer_t* renderer, const char* folder_path)
{
	return vulkan_shader_library_load_folder(renderer->vulkan_handle, folder_path);
}

RENDERER_API void shader_library_destroy(shader_library_t* library)
{
	vulkan_shader_library_destroy(library);
}

RENDERER_API void shader_library_release_resources(shader_library_t* library)
{
	vulkan_shader_library_release_resources(library);
}


/* logic functions */
RENDERER_API shader_handle_t shader_library_create_shader(shader_library_t* library, shader_create_info_t* create_info, const char* shader_name)
{
	return vulkan_shader_library_create_shader(library, CAST_TO(vulkan_shader_create_info_t*, create_info), shader_name);
}

RENDERER_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path, const char* shader_name)
{
	return vulkan_shader_library_load_shader(library, file_path, shader_name);
}

RENDERER_API bool shader_library_destroy_shader(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_destroy_shader(library, shader_name);
}

RENDERER_API bool shader_library_destroy_shaderH(shader_library_t* library, shader_handle_t handle)
{
	return vulkan_shader_library_destroy_shaderH(library, handle);
}

/* getters */

RENDERER_API shader_handle_t shader_library_get_handle(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_get_handle(library, shader_name);
}

RENDERER_API shader_t* shader_library_getH(shader_library_t* library, shader_handle_t handle)
{
	return vulkan_shader_library_getH(library, handle);
}

RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_get(library, shader_name);
}
