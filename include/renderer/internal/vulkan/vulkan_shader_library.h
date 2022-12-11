
#pragma once

#include <renderer/defines.h>
#include <renderer/string.h>
#include <bufferlib/buffer.h>

#include <renderer/internal/vulkan/vulkan_handles.h> 		// vulkan_shader_handle_t

typedef struct vulkan_shader_t vulkan_shader_t;
typedef struct vulkan_shader_create_info_t vulkan_shader_create_info_t;
typedef struct vulkan_shader_load_info_t vulkan_shader_load_info_t;

typedef struct vulkan_shader_library_slot_t
{
	string_t name;
	vulkan_shader_t* shader;
	vulkan_shader_handle_t handle;
} vulkan_shader_library_slot_t;

typedef struct vulkan_shader_library_t
{
	vulkan_renderer_t* renderer;
	BUFFER relocation_table; 	// list of buf_ucount_t
	BUFFER shaders; 			// list of vulkan_shader_library_slot_t
} vulkan_shader_library_t;


/* constructors & destructors */
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_new();
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_shader_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t OUT library);
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_load_folder(vulkan_renderer_t* renderer, const char* folder_path);
RENDERER_API void vulkan_shader_library_destroy(vulkan_shader_library_t* library);
RENDERER_API void vulkan_shader_library_release_resources(vulkan_shader_library_t* library);

/* logic functions */

RENDERER_API vulkan_shader_handle_t vulkan_shader_library_create_shader(vulkan_shader_library_t* library, vulkan_shader_create_info_t* create_info, const char* shader_name);
RENDERER_API vulkan_shader_handle_t vulkan_shader_library_load_shader(vulkan_shader_library_t* library, vulkan_shader_load_info_t* load_info, const char* shader_name);
RENDERER_API bool vulkan_shader_library_destroy_shader(vulkan_shader_library_t* library, const char* shader_name);
RENDERER_API bool vulkan_shader_library_destroy_shaderH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);

/* getters */
RENDERER_API vulkan_shader_handle_t vulkan_shader_library_get_handle(vulkan_shader_library_t* library, const char* shader_name);
RENDERER_API const char* vulkan_shader_library_get_nameH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);
RENDERER_API vulkan_shader_t* vulkan_shader_library_getH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle);
RENDERER_API vulkan_shader_t* vulkan_shader_library_get(vulkan_shader_library_t* library, const char* shader_name);
